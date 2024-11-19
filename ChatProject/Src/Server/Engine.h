#pragma once
class Engine
{
public:
	static Engine& GetInstance();
	bool isRunning;

private:
	Engine();
	~Engine() {}
};

