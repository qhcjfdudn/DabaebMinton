#include "ServerPCH.h"
#include "BadmintonNet.h"

#include "Engine.h"
#include "OutputMemoryBitStream.h"

BadmintonNet::BadmintonNet(PxVec2 location) : StaticGameObject(location)
{
	auto& engineInstance = Engine::GetInstance();
	_rigidbody = engineInstance.CreateBox2DStatic(location, 0.5f, 2.5f);
}

unsigned int BadmintonNet::GetClassId()
{
	return 'BNET';
}

void BadmintonNet::Write(OutputMemoryBitStream& inStream)
{
	StaticGameObject::Write(inStream);
	inStream.Write(_location);
	inStream.Write(_velocity);
}
