#include "ServerPCH.h"
#include "Observer.h"

void Observer::notify(const ObserverEvent& oe) const
{
	switch (oe)
	{
	case ObserverEvent::EngineOff:
		// 돌고 있는 thread가 있다면 여기에 추가해 종료 가능
		return;
	}
}
