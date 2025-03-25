#include "ServerPCH.h"
#include "Observer.h"

#include "NetworkManagerServer.h"

void Observer::notify(ObserverEvent oe)
{
	switch (oe)
	{
	case ObserverEvent::EngineOff:
		NetworkManagerServer&nm = NetworkManagerServer::GetInstance();
		nm.isRunning = false;
	}
}
