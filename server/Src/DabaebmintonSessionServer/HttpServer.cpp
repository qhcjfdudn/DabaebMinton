#include "pch.h"
#include "HttpServer.h"

HttpServer& HttpServer::GetInstance()
{
	static HttpServer instance;
	return instance;
}

void HttpServer::Init()
{

}

bool HttpServer::ListenBlock()
{
	return _server.listen("0.0.0.0", 8081);
}

void HttpServer::Stop()
{
	_server.stop();
}

HttpServer::HttpServer() :
	_server{}
{
}
