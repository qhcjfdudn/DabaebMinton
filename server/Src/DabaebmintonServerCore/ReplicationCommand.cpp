#include "ServerPCH.h"
#include "ReplicationCommand.h"

ReplicationCommand::ReplicationCommand(uint8_t inInitialDirtyState) :
	mAction(ReplicationAction::RA_Create),
	mDirtyState(inInitialDirtyState)
{
}

void ReplicationCommand::HandleCreateAckd()
{
	if (mAction == ReplicationAction::RA_Create)
	{
		mAction = ReplicationAction::RA_Update;
	}
}
void ReplicationCommand::AddDirtyState(uint8_t inState)
{
	mDirtyState |= inState;
}
void ReplicationCommand::SetDestroy()
{
	mAction = ReplicationAction::RA_Destroy;
}

bool ReplicationCommand::HasDirtyState() const
{
	return (mAction == ReplicationAction::RA_Destroy) || (mDirtyState != 0);
}

ReplicationAction	ReplicationCommand::GetAction()	const
{
	return mAction;
}
uint8_t ReplicationCommand::GetDirtyState() const
{
	return mDirtyState;
}

void ReplicationCommand::ClearDirtyState(uint8_t inStateToClear)
{
	mDirtyState &= ~inStateToClear;

	// 분석 필요
	if (mAction == ReplicationAction::RA_Destroy)
	{
		mAction = ReplicationAction::RA_Update;
	}
}
