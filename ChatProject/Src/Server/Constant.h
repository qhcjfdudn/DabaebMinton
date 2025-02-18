#pragma once

namespace Constant
{
#ifdef STAGING_ENV
	const float FIXED_TIMESTEP = 5.0f;
#else
	const float FIXED_TIMESTEP = 0.02f;
#endif
}
