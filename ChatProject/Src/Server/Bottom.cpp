#include "ServerPCH.h"
#include "Bottom.h"

#include "OutputMemoryBitStream.h"

Bottom::Bottom(PxVec2 location) : StaticGameObject(location)
{}

unsigned int Bottom::GetClassId()
{
    return 'BTTM';
}

void Bottom::Write(OutputMemoryBitStream& inStream)
{
	StaticGameObject::Write(inStream);
}
