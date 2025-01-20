#pragma once
class SendQueue
{
public:
	static SendQueue& GetInstance();

	void Push(const string& str);
	string Front();
	bool Empty();

private:
	SendQueue() {}
	~SendQueue() {}

	// NetworkManager와 World의 서로 다른 thread에서 접근할테니
	// lock/unlock 고민 필요
	std::queue<string> _bufferQueue;
};

