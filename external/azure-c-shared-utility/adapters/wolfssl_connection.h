// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef WOLFSSL_CONNECTION_H
#define WOLFSSL_CONNECTION_H

#include "wolfssl/ssl.h"
#include "TCPSocketConnection.h"

class WolfSSLConnection : public TCPSocketConnection
{
public:
    WolfSSLConnection();
    virtual ~WolfSSLConnection();
    
public:
    int connect(const char* host, const int port);
    bool is_connected(void);
    int send(char* data, int length);
    int send_all(char* data, int length);
    int receive(char* data, int length);
    int receive_all(char* data, int length);
    bool close(bool shutdown = true);
    bool load_certificate(const unsigned char* certificate, size_t size);
    
protected:
    WOLFSSL_CTX* sslContext;
    WOLFSSL* ssl;
    bool isConnected;
};

#endif /* WOLFSSL_CONNECTION_H */
