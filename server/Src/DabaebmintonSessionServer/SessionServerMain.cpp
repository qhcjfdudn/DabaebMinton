#include "pch.h"

#include "HttpServer.h"

int main()
{
	thread httpServerRunningThread([] {
		auto& httpServer = HttpServer::GetInstance();
		httpServer.Init();
		httpServer.ListenBlock();
		});

	httpServerRunningThread.join();
}
