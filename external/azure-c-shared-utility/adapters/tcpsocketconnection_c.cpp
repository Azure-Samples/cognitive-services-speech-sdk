// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "mbed.h"

#include <stddef.h>
#include "TCPSocketConnection.h"
#include "azure_c_shared_utility/tcpsocketconnection_c.h"


TCPSOCKETCONNECTION_HANDLE tcpsocketconnection_create(void)
{
    return new TCPSocketConnection();
}

void tcpsocketconnection_set_blocking(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, bool blocking, unsigned int timeout)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	tsc->set_blocking(blocking, timeout);
}

void tcpsocketconnection_destroy(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	delete (TCPSocketConnection*)tcpSocketConnectionHandle;
}

int tcpsocketconnection_connect(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* host, const int port)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->connect(host, port);
}

bool tcpsocketconnection_is_connected(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->is_connected();
}

void tcpsocketconnection_close(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	tsc->close();
}

int tcpsocketconnection_send(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* data, int length)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->send((char*)data, length);
}

int tcpsocketconnection_send_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* data, int length)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->send_all((char*)data, length);
}

int tcpsocketconnection_receive(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char* data, int length)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->receive(data, length);
}

int tcpsocketconnection_receive_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char* data, int length)
{
	TCPSocketConnection* tsc = (TCPSocketConnection*)tcpSocketConnectionHandle;
	return tsc->receive_all(data, length);
}
