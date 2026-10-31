#pragma once

#include "move.h"
#include "nob_addons.h"
#include <functional>
#include <ixwebsocket/IXWebSocketServer.h>
#include <map>
#include <memory>

class App;

class WSConnection {
public:
	WSConnection(App *app);
	~WSConnection();

	void startServer();
	void stopServer();

	// Broadcast an alle verbundenen Engines (z.B. um den gegnerischen Zug mitzuteilen)
	void broadcast(const std::string &message);

	// Callback, um die UI / das Spiel zu aktualisieren, wenn eine Engine einen Zug macht
	void setOnMoveReceivedCallback(std::function<void(Move)> callback);

private:
	ix::WebSocketServer ceb_websocket_server;
	std::function<void(Move)> onMoveReceived;

	// Speichert die Verbindungen (maximal 2 Engines)
	std::map<std::shared_ptr<ix::ConnectionState>, int> connected_engines;
};

inline WSConnection *wsConnection = nullptr;
