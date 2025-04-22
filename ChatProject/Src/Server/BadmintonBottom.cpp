#include "ServerPCH.h"
#include "BadmintonBottom.h"

#include "Engine.h"
#include "OutputMemoryBitStream.h"

BadmintonBottom::BadmintonBottom(PxVec2 location) : StaticGameObject(location)
{
	auto& engineInstance = Engine::GetInstance();
	_rigidbody = engineInstance.CreatePlain(0.f, 1.f, 0.f, 0.f);
}

unsigned int BadmintonBottom::GetClassId()
{
    return 'BTTM';
}

void BadmintonBottom::Write(OutputMemoryBitStream& inStream)
{
	StaticGameObject::Write(inStream);
}
