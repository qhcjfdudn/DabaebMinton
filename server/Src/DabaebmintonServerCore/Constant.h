#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	constexpr float FIXED_UPDATE_TIMESTEP		= 1 / 1.f;	// 1Hz
	constexpr float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 60.f;	// 60Hz
	constexpr float REPLICATION_PERIOD			= 1 / 40.f;	// 40Hz
#else
	constexpr float FIXED_UPDATE_TIMESTEP		= 1 / 50.f;	// 50Hz
	constexpr float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 60.f;	// 60Hz
	constexpr float REPLICATION_PERIOD			= 1 / 40.f; // 40Hz
#endif
	constexpr uint16_t MAX_PACKET_SIZE		= 1'500 /* MTU */ - 20 /* IP Header */ - 8 /* UDP Header */;
	constexpr float PACKET_TIME_OUT			= 0.5f;
}
