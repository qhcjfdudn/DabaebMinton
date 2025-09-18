#include "ServerPCH.h"
#include "ReplicationCommand.h"

ReplicationCommand::ReplicationCommand(uint8_t inInitialDirtyState) :
	_action(ReplicationAction::RA_Create),
	_dirtyState(inInitialDirtyState)
{
}

void ReplicationCommand::HandleCreateAckd()
{
	if (_action == ReplicationAction::RA_Create)
	{
		_action = ReplicationAction::RA_Update;
	}
}
void ReplicationCommand::AddDirtyState(uint8_t inState)
{
	_dirtyState |= inState;
}
void ReplicationCommand::SetDestroy()
{
	_action = ReplicationAction::RA_Destroy;
}

bool ReplicationCommand::HasDirtyState() const
{
	return (_action == ReplicationAction::RA_Destroy) || (_dirtyState != 0);
}

ReplicationAction	ReplicationCommand::GetAction()	const
{
	return _action;
}
uint8_t ReplicationCommand::GetDirtyState() const
{
	return _dirtyState;
}

void ReplicationCommand::ClearDirtyState(uint8_t inStateToClear)
{
	_dirtyState &= ~inStateToClear;

	// 분석 필요
	if (_action == ReplicationAction::RA_Destroy)
	{
		_action = ReplicationAction::RA_Update;
	}
}
