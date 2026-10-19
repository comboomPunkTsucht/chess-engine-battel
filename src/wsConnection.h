#pragma once

#include "global.h"
#include "nob_addons.h"
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

class WSConnection {
public:
	WSConnection();
	~WSConnection();

	void connect();
	void disconnect();
	void send(const char *message);
	const char *receive();

private:
	ix::WebSocket ceb_websocket;
	ix::WebSocketServer ceb_websocket_server;
}
