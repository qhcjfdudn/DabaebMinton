#pragma once

#include "SessionToken.h"

class SessionManager
{
public:
	static SessionManager& GetInstance();

	void CreateSession(PlayerId_t playerId);
	SessionToken GetSession(SessionId_t sessionId);
	void DeleteSession(SessionId_t sessionId);

private:
	SessionManager();

	unordered_map<SessionId_t, SessionToken> _sessionMap;

	SessionToken emptySession{ 0, 0 };
};
