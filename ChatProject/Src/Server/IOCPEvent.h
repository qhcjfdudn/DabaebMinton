#pragma once

class IOCPEvent
{
public:
	IOCPEvent(int max_event_count);

	const int MAX_EVENT_COUNT;

	unique_ptr<OVERLAPPED_ENTRY[]> m_events;
	int m_eventCount = 0;
};

