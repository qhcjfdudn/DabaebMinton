#include "pch.h"
#include "HttpServer.h"

#include "nlohmann/json.hpp"
#include "SessionManager.h"

HttpServer& HttpServer::GetInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::Init()
{
	using json = nlohmann::json;
	
	auto& sessionManager = SessionManager::GetInstance();

	_server.Get("/hello", [] (const httplib::Request& req, httplib::Response& res) {
		res.set_content("Hello Session Server!", "plain/text");
		});

	_server.Get(R"(/session/(\d+))", [&] (const httplib::Request& req, httplib::Response& res) {
		auto sessionId = req.matches[1];
		std::string s = sessionId.str();

		SessionId_t value{};
		std::from_chars(s.data(), s.data() + s.size(), value);

		SessionToken session = sessionManager.GetSession(value);

		json sessionJson;
		sessionJson["SessionId"] = session.GetTokenId();
		sessionJson["PlayerId"] = session.GetPlayerId();
		sessionJson["LastActive"] = session.GetLastActive().time_since_epoch().count();

		res.set_content(sessionJson.dump(), "applicatoin/json");
		});

	_server.Put(R"(/session)", [&] (const httplib::Request& req, httplib::Response& res) {
		json data = json::parse(req.body);
		PlayerId_t playerId = data["PlayerId"];

		spdlog::info("[HttpServe::Put /session] playerId: {}", playerId);

		sessionManager.CreateSession(playerId);
		});

	_server.Delete(R"(/session/(\d+))", [&] (const httplib::Request& req, httplib::Response& res) {
		auto sessionId = req.matches[1];
		std::string s = sessionId.str();

		SessionId_t value{};
		std::from_chars(s.data(), s.data() + s.size(), value);

		sessionManager.DeleteSession(value);

		});

	_server.Post("/stop", [&] (const httplib::Request& req, httplib::Response& res)
		{
			Stop();
		});
}

bool HttpServer::ListenBlock()
{
	return _server.listen("0.0.0.0", 8081);
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
