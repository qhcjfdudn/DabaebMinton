#pragma once

class IOCPEvent
{
public:
	static const int MAX_EVENT_COUNT = 1000;

	OVERLAPPED_ENTRY m_events[MAX_EVENT_COUNT];
	int m_eventCount = 0;
};

