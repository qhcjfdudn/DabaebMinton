#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	constexpr float FIXED_UPDATE_TIMESTEP		= 1 / 1.f;	// 1Hz
	constexpr float PACKET_PERIOD				= 1 / 2.f;	// 2Hz
	constexpr float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 2.f;	// 2Hz
	constexpr float REPLICATION_PERIOD			= 1 / 2.f;	// 2Hz
#else
	constexpr float FIXED_UPDATE_TIMESTEP		= 1 / 50.f;	// 50Hz
	constexpr float PACKET_PERIOD				= 1 / 20.f;	// 20Hz
	constexpr float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 50.f;	// 50Hz
	constexpr float REPLICATION_PERIOD			= 1 / 20.f; // 20Hz
#endif
	constexpr uint16_t MAX_PACKET_SIZE		= 1'500 /* MTU */ - 8 /* UDP Header */ - 20 /* IP Header */;
	constexpr float PACKET_TIME_OUT			= 0.5f;
	constexpr size_t RUDP_MAX_CHANNEL_SIZE = 3;
}
