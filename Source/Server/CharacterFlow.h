#pragma once

#include <optional>
#include <unordered_map>

#include "NetTypes.h"
#include "PlayerSocket.h"
#include "StreamingFlow.h"

class CNetServerInterface;

using clk = std::chrono::steady_clock;

struct SCharacterInstance {
	SCellCoords CurrentCell;
	SPlacementContext PlacementContext;
	SMovementContext MovementContext;
	clk::time_point LastUpdateTime;
	clk::time_point LastBroadcastTime;

	SCharacterInstance()
		: CurrentCell()
		, PlacementContext()
		, MovementContext()
		, LastUpdateTime()
		, LastBroadcastTime()
	{}

	SCharacterInstance& operator=(SCharacterInstance const& InCharacterInstance) {
		CurrentCell = InCharacterInstance.CurrentCell;
		PlacementContext = InCharacterInstance.PlacementContext;
		MovementContext = InCharacterInstance.MovementContext;
		LastUpdateTime = InCharacterInstance.LastUpdateTime;
		LastBroadcastTime = InCharacterInstance.LastBroadcastTime;
		return *this;
	}
};

using OptionalCharacterRef = std::optional<std::reference_wrapper<SCharacterInstance>>;

using TCharacterFlowBase = CStreamingFlow<un::EntityId, 1000.0f>;
class CCharacterFlow : protected TCharacterFlowBase {
protected:
	static uint32_t constexpr BROADCAST_FREQUENCY_MAXIMUM_RATE = 20; // ms
	float const PLACEMENT_ERROR_TOLERANCE = 0.01f; // units
	float const MOVEMENT_ERROR_TOLERANCE = 0.01f; // units

protected:
	std::unordered_map<un::EntityId, SCharacterInstance> m_CharacterInstances;

public:
	CCharacterFlow() : TCharacterFlowBase(VisibilityAddCallback, VisibilityRemoveCallback, BroadcastCallback) {}

	void AddCharacter(un::EntityId const& nIndex, SCharacterInstance const& CharacterInstance);
	void RemoveCharacter(un::EntityId const& nIndex);
	void UpdateCharacterPosition(un::EntityId const& nIndex, SPlacementContext const& PlacementContext, SMovementContext const& MovementContext);
	void ProcessCharactersInGrid() { TCharacterFlowBase::ProcessEntitiesInGrid(); }
	OptionalCharacterRef GetCharacterInstance(un::EntityId const& nCharacter);

protected:
	static OptionalCharacterRef GetCharacterInstance(CCharacterFlow* pCharacterFlow, un::EntityId const& nCharacter);
	static void BroadcastCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter);
	static void VisibilityAddCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter);
	static void VisibilityRemoveCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter);
};
