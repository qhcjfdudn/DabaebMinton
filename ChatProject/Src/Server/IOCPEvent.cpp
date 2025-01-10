#include "ServerPCH.h"
#include "IOCPEvent.h"

IOCPEvent::IOCPEvent(int max_event_count = 1000) : MAX_EVENT_COUNT(max_event_count) {
	m_events = std::make_unique< OVERLAPPED_ENTRY[]>(MAX_EVENT_COUNT);
}