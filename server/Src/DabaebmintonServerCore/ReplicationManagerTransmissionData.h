#pragma once

#include "ReplicationAction.h"
#include "TransmissionData.h"

class ReplicationManager;

class ReplicationManagerTransmissionData : public TransmissionData
{
public:

	ReplicationManagerTransmissionData(ReplicationManager* inReplicationManagerServer) :
		_replicationManagerServer(inReplicationManagerServer)
	{
	}

	class ReplicationTransmission
	{
	public:
		ReplicationTransmission(NetworkId_t inNetworkId, ReplicationAction inAction, uint8_t inState) :
			_networkId(inNetworkId),
			_action(inAction),
			_state(inState)
		{
		}

		int					GetNetworkId()	const { return _networkId; }
		ReplicationAction	GetAction()		const { return _action; }
		uint8_t			GetState()		const { return _state; }

	private:
		int					_networkId;
		ReplicationAction	_action;
		uint8_t			_state;
	};

	void AddTransmission(NetworkId_t inNetworkId, ReplicationAction inAction, uint8_t inState);

	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;

private:
	void HandleCreateDeliveryFailure(int inNetworkId) const;
	void HandleUpdateStateDeliveryFailure(int inNetworkId, uint8_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const;
	void HandleDestroyDeliveryFailure(int inNetworkId) const;
	void HandleCreateDeliverySuccess(int inNetworkId) const;
	void HandleDestroyDeliverySuccess(int inNetworkId) const;

	ReplicationManager* _replicationManagerServer;

	vector<ReplicationTransmission>	mTransmissions;

};

using ReplicationManagerTransmissionDataPtr = shared_ptr<ReplicationManagerTransmissionData>;
