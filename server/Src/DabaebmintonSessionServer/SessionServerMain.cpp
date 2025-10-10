#include "pch.h"

#include "HttpServer.h"

void InitLoggingLevel()
{
#ifdef NDEBUG
#else
	spdlog::set_level(spdlog::level::debug);
#endif
}

int main()
{
	InitLoggingLevel();

	thread httpServerRunningThread([] {
		auto& httpServer = HttpServer::GetInstance();
		httpServer.Init();
		httpServer.ListenBlock();
		});

	httpServerRunningThread.join();
}
