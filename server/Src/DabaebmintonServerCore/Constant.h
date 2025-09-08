#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	const float FIXED_UPDATE_TIMESTEP		= 1.f;		// 1Hz
	const float PACKET_PERIOD				= 1 / 20.f; // 2Hz
	const float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 5.f;	// 5Hz
	const float REPLICATION_PERIOD			= 1 / 20.f; // 20Hz
#else
	const float FIXED_UPDATE_TIMESTEP		= 0.02f;	// 50Hz
	const float PACKET_PERIOD				= 0.1f;		// 10Hz
	const float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 60.f;	// 60Hz
	const float REPLICATION_PERIOD = 1 / 20.f; // 20Hz
#endif
	const int MAX_PACKET_SIZE				= 1'500 /* MTU */ - 8 /* UDP Header */ - 20 /* IP Header */;
}
