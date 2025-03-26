#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	const float FIXED_UPDATE_TIMESTEP =			1.0f;		// 1Hz
	const float PACKET_PERIOD =					0.4f;		// 2.5Hz
	const float PHYSX_FIXED_UPDATE_TIMESTEP =	1 / 60.f;	// 60Hz
#else
	const float FIXED_UPDATE_TIMESTEP = 0.02f;	// 50Hz
	const float PACKET_PERIOD = 0.1f;	// 10Hz
#endif
}
