#pragma once
class SessionToken {
public:
	SessionToken(uint64_t tokenId, PlayerId_t playerId);

	uint64_t GetTokenId() const { return _tokenId; }
	PlayerId_t GetPlayerId() const { return _playerId; }
	void SetActive(bool isActive) { _isActive = isActive; }

private:
	uint64_t _tokenId;						// 클라이언트 인증용 토큰
	PlayerId_t _playerId;					// 이 세션의 소유자
	steady_clock::time_point _lastActive;	// 마지막 활동 시간
	//std::string secretKey;				// 패킷 HMAC/암호화용
	bool _isActive;
};
