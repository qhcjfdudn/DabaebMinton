#pragma once
class PacketQueue
{
public:
	static PacketQueue& GetReceiveStaticInstance();
	static PacketQueue& GetSendStaticInstance();

	void Push(const string& str);
	string Front();
	bool Empty();

private:
	PacketQueue() {}
	~PacketQueue() {}

	// NetworkManager와 World의 서로 다른 thread에서 접근할테니
	// lock/unlock 고민 필요
	std::queue<string> _bufferQueue;
};

// stream 객체를 리턴한다면?
// 일단 string 데이터의 queue로 만들고 추후 stream 타입으로 개선