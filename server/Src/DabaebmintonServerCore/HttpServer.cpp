#include "ServerPCH.h"

#include "HttpServer.h"
#include "nlohmann/json.hpp"

#include "ServerEngine.h"
#include "GameManager.h"
#include "Observer.h"

HttpServer& HttpServer::GetInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::Init()
{
	using json = nlohmann::json;

	_server.Get("/hello", [] (const httplib::Request& req, httplib::Response& res) {
		res.set_content("Hello Server!", "plain/text");
		});

	_server.Post("/game", [&] (const httplib::Request& req, httplib::Response& res)
		{
			json data = json::parse(req.body);
			json& clientList = data["clientList"];
			int len = static_cast<int>(clientList.size());

			if (len != 2) {
				res.status = 405;
				res.set_content("The size of clientList must be 2", "text/plain");

				return;
			}

			cout << "[HttpServer] new Game request." << endl;

			for (int i = 0; i < len; ++i) {
				json& client = clientList[i];
				cout << "[HttpServer] Client" << (i + 1) << ": " << client["ip"] << ":" << client["port"] << endl;
			}

			string ips[2];
			unsigned int ports[2];

			for (int i = 0; i < len; ++i) {
				json& client = clientList[i];
				ips[i] = client["ip"];
				ports[i] = client["port"];
			}

			Game* game = GameManager::GetInstance().CreateGame(ips, ports);
			// 이 game을 NetworkManager와 LevelRunningThread로 전달해야
			// FixedUpdate가 된다.
		});

	_server.Post("/stop", [&] (const httplib::Request& req, httplib::Response& res)
		{
			Observer::notify(ObserverEvent::EngineOff);
		});
}

bool HttpServer::ListenBlock()
{
	return _server.listen("0.0.0.0", 8080);
}

void HttpServer::Stop()
{
	_server.stop();
}

HttpServer::HttpServer() :
	_server{}
{
}