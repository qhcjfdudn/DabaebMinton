#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	const float FIXED_UPDATE_TIMESTEP		= 1.f;		// 1Hz
	const float PACKET_PERIOD				= 0.5f;		// 2Hz
	const float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 60.f;	// 60Hz
#else
	const float FIXED_UPDATE_TIMESTEP		= 0.02f;	// 50Hz
	const float PACKET_PERIOD				= 0.1f;		// 10Hz
	const float PHYSX_FIXED_UPDATE_TIMESTEP = 1 / 60.f;	// 60Hz
#endif

	const int MAX_PACKET_SIZE				= 65'535 /* UDP total length */ - 8 /* UDP Header */ - 20 /* IP Header */;
}
