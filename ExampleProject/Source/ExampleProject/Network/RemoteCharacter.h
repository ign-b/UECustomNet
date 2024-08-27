#pragma once

#include "UECustomNet/NetTypes.h"

class ACNCharacterRemote;

class CRemotePlayer {
protected:
	ACNCharacterRemote* WorldCharacter;

public:
	CRemotePlayer()
		: WorldCharacter(nullptr) {}
	
	explicit CRemotePlayer(SPlacementContext const& PlacementContext);

	~CRemotePlayer();

public:
	ACNCharacterRemote* GetWorldCharacter() const { return WorldCharacter; }
};
