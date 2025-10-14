#include "ServerPCH.h"
#include "HttpServer.h"

#include "nlohmann/json.hpp"

#include "ServerEngine.h"
#include "GameManager.h"
#include "Observer.h"
#include "SessionToken.h"

#include "GameConfig.h"

HttpServer& HttpServer::GetInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::Init()
{
	using json = nlohmann::json;
	GameManager& gameManager = GameManager::GetInstance();

	_server.Get("/hello", [] (const httplib::Request& req, httplib::Response& res) {
		res.set_content("Hello Server!", "plain/text");
		});

	_server.Post("/stop", [&] (const httplib::Request& req, httplib::Response& res)
		{
			Observer::notify(ObserverEvent::EngineOff);

			Stop();
		});

	_server.Post("/game", [&] (const httplib::Request& req, httplib::Response& res)
		{
			json data = json::parse(req.body);
			json& clientList = data["clientList"];
			int len = static_cast<int>(clientList.size());

			if (len != GameConfig::MAX_PLAYERS) {
				res.status = 405;
				res.set_content("The size of clientList must be " + std::to_string(GameConfig::MAX_PLAYERS), "text/plain");

				return;
			}

			// Get Session

			const std::string_view sessionServerURL = "127.0.0.1:8081";
			httplib::Client cli(sessionServerURL.data());

			SessionToken sessions[GameConfig::MAX_PLAYERS];

			for (int i = 0; i < len; ++i)
			{
				json& client = clientList[i];
				SessionId_t sessionId = client["SessionId"];
				auto getRes = cli.Get("/session/" + std::to_string(sessionId)); // GET 요청

				json sessionBody = json::parse(getRes.value().body);
				
				PlayerId_t playerId = sessionBody["PlayerId"];
				steady_clock::time_point lastActive{ steady_clock::duration{ sessionBody["LastActive"] } };
				
				sessions[i] = SessionToken{ sessionId, playerId, lastActive };
				
				spdlog::info("[HttpServer::Post/game] SessionId: {}, PlayerId: {}, time_point: {}", sessionId, playerId, lastActive.time_since_epoch().count());
			}
			
			gameManager.CreateGame(sessions);

			spdlog::info("[HttpServer/game] new Game request.");
			
		});
}

bool HttpServer::ListenBlock()
{
	return _server.listen("0.0.0.0", 8080);
}

void HttpServer::Stop()
{
	if (_isStopping)
		return;

	_isStopping = true;
	_server.stop();
}

HttpServer::HttpServer() :
	_server{}
{
}