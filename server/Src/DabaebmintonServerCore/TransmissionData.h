#pragma once

class DeliveryNotificationManager;

class TransmissionData
{
public:
	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};

using TransmissionDataPtr = shared_ptr<TransmissionData>;
