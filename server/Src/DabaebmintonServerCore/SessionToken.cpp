#include "ServerPCH.h"
#include "SessionToken.h"

SessionToken::SessionToken(SessionId_t tokenId, PlayerId_t playerId) :
	_tokenId{ tokenId },
	_playerId{ playerId },
	_lastActive{ steady_clock::now() },
	_isActive{ true }
{
}

SessionToken::SessionToken(SessionId_t tokenId, PlayerId_t playerId, steady_clock::time_point lastActive) :
	_tokenId{ tokenId },
	_playerId{ playerId },
	_lastActive{ lastActive },
	_isActive{ true }
{
}
