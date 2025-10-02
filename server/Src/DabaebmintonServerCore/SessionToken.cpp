#include "ServerPCH.h"
#include "SessionToken.h"

SessionToken::SessionToken(uint64_t tokenId, PlayerId_t playerId) :
	_tokenId{ tokenId },
	_playerId{ playerId },
	_lastActive{ steady_clock::now() },
	_isActive{ true }
{
}
