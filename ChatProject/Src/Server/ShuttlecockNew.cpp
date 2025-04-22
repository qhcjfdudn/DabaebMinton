#include "ServerPCH.h"
#include "ShuttlecockNew.h"

#include "OutputMemoryBitStream.h"
#include "EngineNew.h"

ShuttlecockNew::ShuttlecockNew(PxVec2 location, PxVec2 velocity) :
	GameObjectNew(location, velocity)
{
}

float ShuttlecockNew::GetRadius()
{
	return _radius;
}

bool ShuttlecockNew::FixedUpdate()
{
	SetCurrentTransform();
	cout << "Shuttlecock FixedUpdate" << endl;
	cout << "Position: " << _location.x << ", " << _location.y << endl;

	return true;
}

unsigned int ShuttlecockNew::GetClassId()
{
	return 'STCK';
}

void ShuttlecockNew::Write(OutputMemoryBitStream& inStream)
{
	GameObjectNew::Write(inStream);
}
