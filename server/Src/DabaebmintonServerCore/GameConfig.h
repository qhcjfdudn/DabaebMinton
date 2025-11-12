#pragma once

#include "Player.h"

namespace GameConfig {
	static const int MAX_PLAYERS = 2;

	static const PxVec2 defaultBadmintonBottomPosition{ 0, 0.5f };
	static const PxVec2 defaultBadmintonNetPosition{ 0, 1.25f };
	
	static const PxVec2 defaultBadmintonShuttlecockPosition{ -2, 6.15f };
	static const PxVec2 defaultBadmintonShuttlecockVelocity{ 4, 3 };

	static const PxVec2 defaultPlayersPosition[MAX_PLAYERS] = {
		PxVec2{ -6, 5 }, PxVec2{ 6, 5 }
	};
	static const ECharacterID defaultPlayerCharacterIds[MAX_PLAYERS] = {
		ECharacterID::Daramgee, ECharacterID::Baebsae
	};
}
