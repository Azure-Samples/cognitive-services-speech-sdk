// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/crypto.h"
#include "openssl/opensslv.h"
#include <stdio.h>
#include <stdbool.h>
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/tlsio_openssl.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"

typedef enum TLSIO_STATE_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPENING_UNDERLYING_IO,
    TLSIO_STATE_IN_HANDSHAKE,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_CLOSING,
    TLSIO_STATE_ERROR
} TLSIO_STATE;

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE underlying_io;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_open_complete_context;
    void* on_io_close_complete_context;
    void* on_io_error_context;
    SSL* ssl;
    SSL_CTX* ssl_context;
    BIO* in_bio;
    BIO* out_bio;
    TLSIO_STATE tlsio_state;
    char* hostname;
    int port;
    char* certificate;
} TLS_IO_INSTANCE;

struct CRYPTO_dynlock_value 
{
    LOCK_HANDLE lock; 
};

static const IO_INTERFACE_DESCRIPTION tlsio_openssl_interface_description =
{
    tlsio_openssl_create,
    tlsio_openssl_destroy,
    tlsio_openssl_open,
    tlsio_openssl_close,
    tlsio_openssl_send,
    tlsio_openssl_dowork,
    tlsio_openssl_setoption
};


#if (OPENSSL_VERSION_NUMBER < 0x10100000L)

static LOCK_HANDLE * openssl_locks = NULL;

static void openssl_lock_unlock_helper(LOCK_HANDLE lock, int lock_mode, const char* file, int line)
{
    if (lock_mode & CRYPTO_LOCK)
    {
        if (Lock(lock) != 0)
        {
            LogError("Failed to lock openssl lock (%s:%d)", file, line);
        }
    }
    else
    {
        if (Unlock(lock) != 0)
        {
            LogError("Failed to unlock openssl lock (%s:%d)", file, line);
        }
    }
}


static struct CRYPTO_dynlock_value* openssl_dynamic_locks_create_cb(const char* file, int line)
{
    struct CRYPTO_dynlock_value* result;
    
    result = malloc(sizeof(struct CRYPTO_dynlock_value));
    
    if (result == NULL)
    {
        LogError("Failed to allocate lock!  Out of memory (%s:%d).", file, line);
    }
    else
    {
        result->lock = Lock_Init();
        if (result->lock == NULL)
        {
            LogError("Failed to create lock for dynamic lock (%s:%d).", file, line);
            
            free(result);
            result = NULL;
        }
    }
    
    return result;
}

static void openssl_dynamic_locks_lock_unlock_cb(int lock_mode, struct CRYPTO_dynlock_value* dynlock_value, const char* file, int line)
{
    openssl_lock_unlock_helper(dynlock_value->lock, lock_mode, file, line);
}

static void openssl_dynamic_locks_destroy_cb(struct CRYPTO_dynlock_value* dynlock_value, const char* file, int line)
{
    Lock_Deinit(dynlock_value->lock);
    free(dynlock_value);
}

static void openssl_dynamic_locks_uninstall(void)
{
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
}

static void openssl_dynamic_locks_install(void)
{
    CRYPTO_set_dynlock_destroy_callback(openssl_dynamic_locks_destroy_cb);
    CRYPTO_set_dynlock_lock_callback(openssl_dynamic_locks_lock_unlock_cb);
    CRYPTO_set_dynlock_create_callback(openssl_dynamic_locks_create_cb);
}

static void openssl_static_locks_lock_unlock_cb(int lock_mode, int lock_index, const char * file, int line)
{
    if (lock_index < 0 || lock_index >= CRYPTO_num_locks())
    {
        LogError("Bad lock index %d passed (%s:%d)", lock_index, file, line);
    }
    else
    {
        openssl_lock_unlock_helper(openssl_locks[lock_index], lock_mode, file, line);
    }
}

static void openssl_static_locks_uninstall(void)
{
    if (openssl_locks != NULL)
    {
        CRYPTO_set_locking_callback(NULL);
        
        for(int i = 0; i < CRYPTO_num_locks(); i++)
        {
            if (openssl_locks[i] != NULL)
            {
                Lock_Deinit(openssl_locks[i]);
            }
        }
        
        free(openssl_locks);
        openssl_locks = NULL;
    }
    else
    {
        LogError("Locks already uninstalled");
    }
}

static int openssl_static_locks_install(void)
{
    int result;
    
    if (openssl_locks != NULL)
    {
        LogError("Locks already initialized");
        result = __LINE__;
    }
    else
    {
        openssl_locks = malloc(CRYPTO_num_locks() * sizeof(LOCK_HANDLE));
        if(openssl_locks == NULL)
        {
            LogError("Failed to allocate locks");
            result = __LINE__;
        }
        else
        {
            int i;
            for(i = 0; i < CRYPTO_num_locks(); i++)
            {
                openssl_locks[i] = Lock_Init();
                if (openssl_locks[i] == NULL)
                {
                    LogError("Failed to allocate lock %d", i);
                    break;
                }
            }
            
            if (i != CRYPTO_num_locks())
            {
                result = __LINE__;
                
                for (int j = 0; j < i; j++)
                {
                    Lock_Deinit(openssl_locks[j]);
                }
            }
            else
            {
                CRYPTO_set_locking_callback(openssl_static_locks_lock_unlock_cb);
                
                result = 0;
            }
        }
    }
    return result;
}

#endif

static void log_ERR_get_error(const char* message)
{
    char buf[128];
    unsigned long error;

    if (message != NULL)
    {
        LogError("%s", message);
    }

    error = ERR_get_error();

    for(int i = 0; 0 != error; i++)
    {
        LogError("  [%d] %s", i, ERR_error_string(error, buf));
        error = ERR_get_error();
    }
}

static void indicate_error(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance->on_io_error == NULL)
    {
        LogError("NULL on_io_error.");
    }
    else
    {
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

static void indicate_open_complete(TLS_IO_INSTANCE* tls_io_instance, IO_OPEN_RESULT open_result)
{
    if (tls_io_instance->on_io_open_complete == NULL)
    {
        LogError("NULL on_io_open_complete.");
    }
    else
    {
        tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, open_result);
    }
}

static int write_outgoing_bytes(TLS_IO_INSTANCE* tls_io_instance, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    int pending = BIO_ctrl_pending(tls_io_instance->out_bio);
    if (pending <= 0)
    {
        result = 0;
    }
    else
    {
        unsigned char* bytes_to_send = malloc(pending);
        if (bytes_to_send == NULL)
        {
            result = __LINE__;
            LogError("NULL bytes_to_send.");
        }
        else
        {
            if (BIO_read(tls_io_instance->out_bio, bytes_to_send, pending) != pending)
            {
                result = __LINE__;
                log_ERR_get_error("BIO_read not in pending state.");
            }
            else
            {
                if (xio_send(tls_io_instance->underlying_io, bytes_to_send, pending, on_send_complete, callback_context) != 0)
                {
                    result = __LINE__;
                    LogError("Error in xio_send.");
                }
                else
                {
                    result = 0;
                }
            }

            free(bytes_to_send);
        }
    }

    return result;
}

static int send_handshake_bytes(TLS_IO_INSTANCE* tls_io_instance)
{
    int result;

    if (SSL_is_init_finished(tls_io_instance->ssl))
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
        indicate_open_complete(tls_io_instance, IO_OPEN_OK);
        result = 0;
    }
    else
    {
        SSL_do_handshake(tls_io_instance->ssl);
        if (SSL_is_init_finished(tls_io_instance->ssl))
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_OK);
            result = 0;
        }
        else
        {
            if (write_outgoing_bytes(tls_io_instance, NULL, NULL) != 0)
            {
                result = __LINE__;
                LogError("Error in write_outgoing_bytes.");
            }
            else
            {
                if (SSL_is_init_finished(tls_io_instance->ssl))
                {
                    tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
                    indicate_open_complete(tls_io_instance, IO_OPEN_OK);
                }

                result = 0;
            }
        }
    }

    return result;
}

static void on_underlying_io_close_complete(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    switch (tls_io_instance->tlsio_state)
    {
        default:
        case TLSIO_STATE_NOT_OPEN:
        case TLSIO_STATE_OPEN:
            break;

        case TLSIO_STATE_OPENING_UNDERLYING_IO:
        case TLSIO_STATE_IN_HANDSHAKE:
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            break;

        case TLSIO_STATE_CLOSING:
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

            if (tls_io_instance->on_io_close_complete != NULL)
            {
                tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
            }
            break;
    }
}

static void on_underlying_io_open_complete(void* context, IO_OPEN_RESULT open_result)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    if (tls_io_instance->tlsio_state == TLSIO_STATE_OPENING_UNDERLYING_IO)
    {
        if (open_result == IO_OPEN_OK)
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_IN_HANDSHAKE;

            if (send_handshake_bytes(tls_io_instance) != 0)
            {
                if (xio_close(tls_io_instance->underlying_io, on_underlying_io_close_complete, tls_io_instance) != 0)
                {
                    indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
                    LogError("Error in xio_close.");
                }
            }
        }
        else
        {
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPENING_UNDERLYING_IO.");
        }
    }
}

static void on_underlying_io_error(void* context)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    switch (tls_io_instance->tlsio_state)
    {
        default:
            break;

        case TLSIO_STATE_OPENING_UNDERLYING_IO:
        case TLSIO_STATE_IN_HANDSHAKE:
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            break;

        case TLSIO_STATE_OPEN:
            indicate_error(tls_io_instance);
            break;
    }
}

static int decode_ssl_received_bytes(TLS_IO_INSTANCE* tls_io_instance)
{
    int result = 0;
    unsigned char buffer[64];

    int rcv_bytes = 1;
    while (rcv_bytes > 0)
    {
        rcv_bytes = SSL_read(tls_io_instance->ssl, buffer, sizeof(buffer));
        if (rcv_bytes > 0)
        {
            if (tls_io_instance->on_bytes_received == NULL)
            {
                LogError("NULL on_bytes_received.");
            }
            else
            {
                tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, rcv_bytes);
            }
        }
    }

    return result;
}

static void on_underlying_io_bytes_received(void* context, const unsigned char* buffer, size_t size)
{
    TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)context;

    int written = BIO_write(tls_io_instance->in_bio, buffer, size);
    if (written != size)
    {
        tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
        indicate_error(tls_io_instance);
        log_ERR_get_error("Error in BIO_write.");
    }
    else
    {
        switch (tls_io_instance->tlsio_state)
        {
            default:
                break;

            case TLSIO_STATE_IN_HANDSHAKE:
                if (send_handshake_bytes(tls_io_instance) != 0)
                {
                    if (xio_close(tls_io_instance->underlying_io, on_underlying_io_close_complete, tls_io_instance) != 0)
                    {
                        indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
                        LogError("Error in xio_close.");
                    }
                }
                break;

            case TLSIO_STATE_OPEN:
                if (decode_ssl_received_bytes(tls_io_instance) != 0)
                {
                    tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                    indicate_error(tls_io_instance);
                    LogError("Error in decode_ssl_received_bytes.");
                }
                break;
        }
    }
}

static void destroy_openssl_instance(TLS_IO_INSTANCE* tls_io_instance)
{
    if (tls_io_instance != NULL)
    {
        SSL_free(tls_io_instance->ssl);
        SSL_CTX_free(tls_io_instance->ssl_context);
    }
}

static int add_certificate_to_store(TLS_IO_INSTANCE* tls_io_instance, const char* certValue)
{
    int result = 0;

    if (certValue != NULL)
    {
        BIO* cert_memory_bio;
        X509* xcert;

        X509_STORE* cert_store = SSL_CTX_get_cert_store(tls_io_instance->ssl_context);
        if (cert_store == NULL)
        {
            log_ERR_get_error("Failed calling SSL_CTX_get_cert_store.");
            result = __LINE__;
        }
        else
        {
            cert_memory_bio = BIO_new_mem_buf((void*)certValue, -1);
            if (cert_memory_bio == NULL)
            {
                log_ERR_get_error("Failed calling BIO_new_mem_buf.");
                result = __LINE__;
            }
            else
            {
                xcert = PEM_read_bio_X509(cert_memory_bio, NULL, NULL, NULL);
                if (xcert == NULL)
                {
                    log_ERR_get_error("Failed calling PEM_read_bio_X509.");
                    result = __LINE__;
                }
                else
                {
                    if (X509_STORE_add_cert(cert_store, xcert) != 1)
                    {
                        log_ERR_get_error("Failed calling X509_STORE_add_cert.");
                        X509_free(xcert);
                        result = __LINE__;
                    }
                    else
                    {
                        result = 0;
                    }
                }
                BIO_free(cert_memory_bio);
            }
        }
    }
    return result;
}

static int create_openssl_instance(TLS_IO_INSTANCE* tlsInstance)
{
    int result;

    tlsInstance->ssl_context = SSL_CTX_new(TLSv1_method());
    if (tlsInstance->ssl_context == NULL)
    {
        result = __LINE__;
        log_ERR_get_error("Failed allocating OpenSSL context.");
    }
    else if (add_certificate_to_store(tlsInstance, tlsInstance->certificate) != 0)
    {
        result = __LINE__;
    }
    else
    {
        tlsInstance->in_bio = BIO_new(BIO_s_mem());
        if (tlsInstance->in_bio == NULL)
        {
            SSL_CTX_free(tlsInstance->ssl_context);
            log_ERR_get_error("Failed BIO_new for in BIO.");
            result = __LINE__;
        }
        else
        {
            tlsInstance->out_bio = BIO_new(BIO_s_mem());
            if (tlsInstance->out_bio == NULL)
            {
                (void)BIO_free(tlsInstance->in_bio);
                SSL_CTX_free(tlsInstance->ssl_context);
                result = __LINE__;
                log_ERR_get_error("Failed BIO_new for out BIO.");
            }
            else
            {
                const IO_INTERFACE_DESCRIPTION* underlying_io_interface = socketio_get_interface_description();
                if (underlying_io_interface == NULL)
                {
                    (void)BIO_free(tlsInstance->in_bio);
                    (void)BIO_free(tlsInstance->out_bio);
                    SSL_CTX_free(tlsInstance->ssl_context);
                    result = __LINE__;
                    LogError("Failed getting socket IO interface description.");
                }
                else
                {
                    SOCKETIO_CONFIG socketio_config;
                    socketio_config.hostname = tlsInstance->hostname;
                    socketio_config.port = tlsInstance->port;
                    socketio_config.accepted_socket = NULL;

                    tlsInstance->underlying_io = xio_create(underlying_io_interface, &socketio_config);
                    if ((tlsInstance->underlying_io == NULL) ||
                        (BIO_set_mem_eof_return(tlsInstance->in_bio, -1) <= 0) ||
                        (BIO_set_mem_eof_return(tlsInstance->out_bio, -1) <= 0))
                    {
                        (void)BIO_free(tlsInstance->in_bio);
                        (void)BIO_free(tlsInstance->out_bio);
                        SSL_CTX_free(tlsInstance->ssl_context);
                        result = __LINE__;
                        LogError("Failed xio_create.");
                    }
                    else
                    {
                        SSL_CTX_set_verify(tlsInstance->ssl_context, SSL_VERIFY_PEER, NULL);

                        // Specifies that the default locations for which CA certificates are loaded should be used.
                        if (SSL_CTX_set_default_verify_paths(tlsInstance->ssl_context) != 1)
                        {
                            // This is only a warning to the user. They can still specify the certificate via SetOption.
                            LogInfo("WARNING: Unable to specify the default location for CA certificates on this platform.");
                        }

                        tlsInstance->ssl = SSL_new(tlsInstance->ssl_context);
                        if (tlsInstance->ssl == NULL)
                        {
                            (void)BIO_free(tlsInstance->in_bio);
                            (void)BIO_free(tlsInstance->out_bio);
                            SSL_CTX_free(tlsInstance->ssl_context);
                            log_ERR_get_error("Failed creating OpenSSL instance.");
                            result = __LINE__;
                        }
                        else
                        {
                            SSL_set_bio(tlsInstance->ssl, tlsInstance->in_bio, tlsInstance->out_bio);
                            SSL_set_connect_state(tlsInstance->ssl);
                            result = 0;
                        }
                    }
                }
            }
        }
    }
    return result;
}

int tlsio_openssl_init(void)
{
    (void)SSL_library_init();

    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    if (openssl_static_locks_install() != 0)
    {
        LogError("Failed to install static locks in OpenSSL!");
        return __LINE__;
    }

    openssl_dynamic_locks_install();
#endif
    return 0;
}

void tlsio_openssl_deinit(void)
{
#if (OPENSSL_VERSION_NUMBER < 0x10100000L)
    openssl_dynamic_locks_uninstall();
    openssl_static_locks_uninstall();
#endif

    ERR_free_strings();
}

CONCRETE_IO_HANDLE tlsio_openssl_create(void* io_create_parameters)
{
    TLSIO_CONFIG* tls_io_config = io_create_parameters;
    TLS_IO_INSTANCE* result;

    if (tls_io_config == NULL)
    {
        result = NULL;
        LogError("NULL tls_io_config.");
    }
    else
    {
        result = malloc(sizeof(TLS_IO_INSTANCE));
        if (result == NULL)
        {
            LogError("Failed allocating TLSIO instance.");
        }
        else
        {
            memset(result, 0, sizeof(TLS_IO_INSTANCE));
            mallocAndStrcpy_s(&result->hostname, tls_io_config->hostname);
            result->port = tls_io_config->port;

            result->ssl_context = NULL;
            result->ssl = NULL;
            result->in_bio = NULL;
            result->out_bio = NULL;
            result->certificate = NULL;

            result->on_bytes_received = NULL;
            result->on_bytes_received_context = NULL;

            result->on_io_open_complete = NULL;
            result->on_io_open_complete_context = NULL;

            result->on_io_close_complete = NULL;
            result->on_io_close_complete_context = NULL;

            result->on_io_error = NULL;
            result->on_io_error_context = NULL;

            result->tlsio_state = TLSIO_STATE_NOT_OPEN;
        }
    }

    return result;
}

void tlsio_openssl_destroy(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;
        if (tls_io_instance->certificate != NULL)
        {
            free(tls_io_instance->certificate);
            tls_io_instance->certificate = NULL;
        }
        free(tls_io_instance->hostname);
        xio_destroy(tls_io_instance->underlying_io);
        free(tls_io);
    }
}

int tlsio_openssl_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    if (tls_io == NULL)
    {
        result = __LINE__;
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            result = __LINE__;
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN.");
        }
        else
        {
            tls_io_instance->on_io_open_complete = on_io_open_complete;
            tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

            if (create_openssl_instance(tls_io_instance) != 0)
            {
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                result = __LINE__;
            }
            else if (xio_open(tls_io_instance->underlying_io, on_underlying_io_open_complete, tls_io_instance, on_underlying_io_bytes_received, tls_io_instance, on_underlying_io_error, tls_io_instance) != 0)
            {
                result = __LINE__;
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

int tlsio_openssl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
        result = __LINE__;
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if ((tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN) ||
            (tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING))
        {
            result = __LINE__;
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_NOT_OPEN or TLSIO_STATE_CLOSING.");
        }
        else
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_CLOSING;
            tls_io_instance->on_io_close_complete = on_io_close_complete;
            tls_io_instance->on_io_close_complete_context = callback_context;

            if (xio_close(tls_io_instance->underlying_io, on_underlying_io_close_complete, tls_io_instance) != 0)
            {
                result = __LINE__;
                tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
                LogError("Error in xio_close.");
            }
            else
            {
                destroy_openssl_instance(tls_io_instance);
                result = 0;
            }
        }
    }

    return result;
}

int tlsio_openssl_send(CONCRETE_IO_HANDLE tls_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    if (tls_io == NULL)
    {
        result = __LINE__;
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN)
        {
            result = __LINE__;
            LogError("Invalid tlsio_state. Expected state is TLSIO_STATE_OPEN.");
        }
        else
        {
            int res = SSL_write(tls_io_instance->ssl, buffer, size);
            if (res != size)
            {
                result = __LINE__;
                log_ERR_get_error("SSL_write error.");
            }
            else
            {
                if (write_outgoing_bytes(tls_io_instance, on_send_complete, callback_context) != 0)
                {
                    result = __LINE__;
                    LogError("Error in write_outgoing_bytes.");
                }
                else
                {
                    result = 0;
                }
            }
        }
    }

    return result;
}

void tlsio_openssl_dowork(CONCRETE_IO_HANDLE tls_io)
{
    if (tls_io == NULL)
    {
        LogError("NULL tls_io.");
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if ((tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN) &&
            (tls_io_instance->tlsio_state != TLSIO_STATE_ERROR))
        {
            xio_dowork(tls_io_instance->underlying_io);
        }
    }
}

int tlsio_openssl_setoption(CONCRETE_IO_HANDLE tls_io, const char* optionName, const void* value)
{
    int result;

    if (tls_io == NULL || optionName == NULL)
    {
        result = __LINE__;
    }
    else
    {
        TLS_IO_INSTANCE* tls_io_instance = (TLS_IO_INSTANCE*)tls_io;

        if (strcmp("TrustedCerts", optionName) == 0)
        {
            const char* cert = (const char*)value;

            if (tls_io_instance->certificate != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->certificate);
            }

            // Store the certificate
            size_t len = strlen(cert);
            tls_io_instance->certificate = malloc(len+1);
            if (tls_io_instance->certificate == NULL)
            {
                result = __LINE__;
            }
            else
            {
                strcpy(tls_io_instance->certificate, cert);
                result = 0;
            }

            // If we're previously connected then add the cert to the context
            if (tls_io_instance->ssl_context != NULL)
            {
                result = add_certificate_to_store(tls_io_instance, cert);
            }
        }
        else
        {
            if (tls_io_instance->underlying_io == NULL)
            {
                result = __LINE__;
            }
            else
            {
                result = xio_setoption(tls_io_instance->underlying_io, optionName, value);
            }
        }
    }

    return result;
}

const IO_INTERFACE_DESCRIPTION* tlsio_openssl_get_interface_description(void)
{
    return &tlsio_openssl_interface_description;
}
