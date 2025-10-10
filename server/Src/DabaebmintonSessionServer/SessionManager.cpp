#include "pch.h"
#include "SessionManager.h"

SessionManager& SessionManager::GetInstance()
{
	static SessionManager instance;
	return instance;
}

void SessionManager::CreateSession(PlayerId_t playerId)
{
	std::random_device random;
	std::mt19937 engine(random());
	std::uniform_int_distribution<SessionId_t> distribution(1, (std::numeric_limits<SessionId_t>::max)());

	SessionId_t sessionId = 0;

	do
	{
		sessionId = distribution(engine);
	} while (_sessionMap.find(sessionId) != _sessionMap.end());
	
	spdlog::debug("[SessionManager::CreateSession] sessionId: {}", sessionId);

	_sessionMap.emplace(sessionId, SessionToken{ sessionId, playerId });
}

SessionToken SessionManager::GetSession(SessionId_t sessionId)
{
	if (_sessionMap.find(sessionId) == _sessionMap.end())
	{
		spdlog::debug("[SessionManager::GetSession] Can't find SessionId: {}", sessionId);
		return emptySession;
	}

	return _sessionMap.at(sessionId);
}

void SessionManager::DeleteSession(SessionId_t sessionId)
{
	if (_sessionMap.find(sessionId) == _sessionMap.end())
	{
		spdlog::debug("[SessionManager::DeleteSession] Can't find SessionId: {}", sessionId);
		return;
	}

	SessionToken& session = _sessionMap.at(sessionId);
	spdlog::info("[SessionManager::DeleteSession] delete session: {}, playerId: {}", sessionId, session.GetPlayerId());

	_sessionMap.erase(sessionId);
}

SessionManager::SessionManager()
{
#ifdef _DEBUG
	// Client 접속 테스트를 편하게 하기 위한 개발 코드
	SessionToken dummy1{ 0, 123 }, dummy2{ 1, 234 };
	
	_sessionMap.emplace(0, dummy1);
	_sessionMap.emplace(1, dummy2);
#endif
}