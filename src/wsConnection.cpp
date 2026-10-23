#include "wsConnection.h"
#include "global.h"
#include "nob_addons.h"
#include <string>

// Der Server wird direkt mit dem konfigurierten Port und der Adresse aus global.h initialisiert
WSConnection::WSConnection() : ceb_websocket_server(server_port, server_address) { }

WSConnection::~WSConnection() { stopServer(); }

void WSConnection::setOnMoveReceivedCallback(std::function<void(Move)> callback) { onMoveReceived = callback; }

void WSConnection::startServer() {
	ceb_websocket_server.setOnClientMessageCallback(
	    [this](std::shared_ptr<ix::ConnectionState> connectionState, ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
		    if (msg->type == ix::WebSocketMessageType::Open) {
			    // Erlaube nur genau 2 Engines
			    if (connected_engines.size() >= 2) {
				    webSocket.close(1000, "Server is full. Two engines are already connected.");
				    return;
			    }

			    int engine_id = connected_engines.size() + 1; // Engine 1 oder 2
			    connected_engines[connectionState] = engine_id;

			    nob_log(INFO, "Engine %d connected from %s", engine_id, connectionState->getRemoteIp().c_str());
		    } else if (msg->type == ix::WebSocketMessageType::Close) {
			    connected_engines.erase(connectionState);
			    nob_log(INFO, "An Engine disconnected.");
		    } else if (msg->type == ix::WebSocketMessageType::Message) {
			    // Eine Engine hat einen Zug gesendet!

			    nob_log(INFO, "Move received from Engine %d: %s", connected_engines[connectionState], msg->str.c_str());
			    // 1. Zug an die ANDERE Engine weiterleiten (damit sie weiß, was passiert ist)
			    for (auto client : ceb_websocket_server.getClients()) {
				    if (client.get() != &webSocket) { client->send(msg->str); }
			    }

			    // 2. Den Zug an dein Raylib-Game übergeben, um ihn grafisch anzuzeigen
			    if (onMoveReceived) {
				    // String (z.B. "12345") in uint32_t und dann in Move casten
				    Move m = {(uint32_t)std::stoul(msg->str)};
				    onMoveReceived(m);
			    }
		    }
	    });

	auto res = ceb_websocket_server.listen();
	if (!res.first) {
		nob_log(ERROR, "Failed to start websocket server: %s", res.second.c_str());
		return;
	}

	ceb_websocket_server.start();
	// server_uri wird in app.cpp / global.h via setServerUri() generiert
	nob_log(INFO, "Engine Server is running! Engines should connect to: %s", server_uri);
}

void WSConnection::stopServer() { ceb_websocket_server.stop(); }

void WSConnection::broadcast(const std::string &message) {
	for (auto client : ceb_websocket_server.getClients()) { client->send(message); }
}
