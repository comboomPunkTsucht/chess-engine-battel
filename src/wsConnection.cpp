#include "wsConnection.h"
#include "global.h"
#include "nob_addons.h"
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

WSConnection::WSConnection() {
	// Initialize the WebSocket connection
	ix::WebSocketHttpHeaders headers;
	headers["User-Agent"] = "ChessEngineBattleClient/1.0";
	ceb_websocket.setExtraHeaders(headers);
}

WSConnection::~WSConnection() {
	// Clean up resources if needed
	disconnect();
}

void WSConnection::connect() {
	// Connect to the WebSocket server
	ceb_websocket.setUrl(server_uri);
	ceb_websocket.start();
}

void WSConnection::disconnect() {
	// Disconnect from the WebSocket server
	ceb_websocket.stop();
}
void WSConnection::send(const char *message) {
	// Send a message to the WebSocket server

	ceb_websocket.send(message);
}

const char *WSConnection::receive() {
	// Receive a message from the WebSocket server
	return ""; // ceb_websocket.getNextMessage();
}
