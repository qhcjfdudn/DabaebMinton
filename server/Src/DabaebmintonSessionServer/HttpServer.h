#pragma once

#include <httplib.h>

class HttpServer
{
public:
	static HttpServer& GetInstance();

	void Init();
	bool ListenBlock();
	void Stop();

private:
	HttpServer();

	httplib::Server _server;
};
