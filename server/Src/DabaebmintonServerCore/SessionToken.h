#pragma once

using SessionId_t = uint64_t;

class SessionToken {
public:
	SessionToken(SessionId_t tokenId = 0, PlayerId_t playerId = 0);
	SessionToken(SessionId_t tokenId, PlayerId_t playerId, steady_clock::time_point lastActive);

	SessionId_t GetTokenId() const { return _tokenId; }
	PlayerId_t GetPlayerId() const { return _playerId; }
	steady_clock::time_point GetLastActive() const { return _lastActive; }
	void SetActive(bool isActive) { _isActive = isActive; }

private:
	SessionId_t _tokenId;						// 클라이언트 인증용 토큰
	PlayerId_t _playerId;					// 이 세션의 소유자
	steady_clock::time_point _lastActive;	// 마지막 활동 시간
	//std::string secretKey;				// 패킷 HMAC/암호화용
	bool _isActive;
};
