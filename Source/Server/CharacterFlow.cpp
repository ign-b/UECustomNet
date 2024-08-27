#include "CharacterFlow.h"

#include "Main.h"
#include "BitStream.h"
#include "RpcIdentifiers.h"
#include "PacketPriorities.h"
#include "PacketReliabilities.h"
#include "PacketIdentifiers.h"
#include "Packet.h"
#include "NetServerInterface.h"

void CCharacterFlow::AddCharacter(un::EntityId const& nIndex, SCharacterInstance const& CharacterInstance) {
	auto [itCharacterInstance, bInserted] = m_CharacterInstances.try_emplace(nIndex, CharacterInstance);
	if (bInserted) {
		TCharacterFlowBase::AddEntity(nIndex, CharacterInstance.CurrentCell);
	}
}

void CCharacterFlow::RemoveCharacter(un::EntityId const& nIndex) {
	auto itCharacterInstance = m_CharacterInstances.find(nIndex);
	if (itCharacterInstance == m_CharacterInstances.end()) return;

	auto const& CharacterInstance = itCharacterInstance->second;
	TCharacterFlowBase::RemoveEntity(nIndex, CharacterInstance.CurrentCell);

	m_CharacterInstances.erase(itCharacterInstance);
}

void CCharacterFlow::UpdateCharacterPosition(un::EntityId const& nIndex, SPlacementContext const& PlacementContext, SMovementContext const& MovementContext) {
	auto const& CharacterInstanceOpt = CCharacterFlow::GetCharacterInstance(nIndex);
	if (CharacterInstanceOpt != std::nullopt) {
		auto& CharacterInstance = CharacterInstanceOpt->get();

		if (CharacterInstance.PlacementContext.IsNearlyEqual(PlacementContext, PLACEMENT_ERROR_TOLERANCE) &&
			CharacterInstance.MovementContext.IsNearlyEqual(MovementContext, MOVEMENT_ERROR_TOLERANCE)) {
			return;
		}

		CharacterInstance.PlacementContext = PlacementContext;
		CharacterInstance.MovementContext = MovementContext;
		CharacterInstance.LastUpdateTime = clk::now();

		auto const OldCell = CharacterInstance.CurrentCell;
		auto const NewCell = TCharacterFlowBase::WorldToCellCoords(
			PlacementContext.vecPosition.GetX(), PlacementContext.vecPosition.GetY());

		if (OldCell == NewCell) {
			return;
		}

		CharacterInstance.CurrentCell = NewCell;

		TCharacterFlowBase::UpdateEntityPosition(nIndex, OldCell, NewCell);
	}
}

OptionalCharacterRef CCharacterFlow::GetCharacterInstance(un::EntityId const& nCharacter) {
	auto itCharacterInstance = m_CharacterInstances.find(nCharacter);
	if (itCharacterInstance != m_CharacterInstances.end()) {
		return std::ref(itCharacterInstance->second);
	}

	return std::nullopt;
}

OptionalCharacterRef CCharacterFlow::GetCharacterInstance(CCharacterFlow* pCharacterFlow, un::EntityId const& nCharacter) {
	if (pCharacterFlow) {
		return pCharacterFlow->GetCharacterInstance(nCharacter);
	}

	return std::nullopt;
}

void CCharacterFlow::BroadcastCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter) {
	auto* pCurrentCharacterFlow = g_pServerApp->GetCharacterFlow();
	auto const& CharacterInstanceOpt = GetCharacterInstance(pCurrentCharacterFlow, nOtherCharacter);

	if (CharacterInstanceOpt != std::nullopt) {
		auto& CharacterInstance = CharacterInstanceOpt->get();

		auto const NowClockTime = clk::now();
		auto const nTimeSinceLastBroadcast = std::chrono::duration_cast<std::chrono::milliseconds>
			(NowClockTime - CharacterInstance.LastBroadcastTime).count();

		if (nTimeSinceLastBroadcast < BROADCAST_FREQUENCY_MAXIMUM_RATE) {
			return;
		}

		if (CharacterInstance.LastBroadcastTime > CharacterInstance.LastUpdateTime) {
			return;
		}

		CBitStream BitStream{};
		BitStream.Write(static_cast<un::PacketId>(PACKET_CHARACTER_ONFOOT_DATA));
		BitStream.Write(nOtherCharacter);
		BitStream.Write(CharacterInstance.PlacementContext);
		BitStream.Write(CharacterInstance.MovementContext);

		g_pServerApp->GetNetServer()->Send(&BitStream, PRIORITY_LOW, RELIABILITY_UNRELIABLE_SEQUENCED, nCharacter, false);

		CharacterInstance.LastBroadcastTime = clk::now();
	}
}

void CCharacterFlow::VisibilityAddCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter) {
	printf("[Server:Debug] Character %i is now seeing character %i\n", nCharacter, nOtherCharacter);

	auto* pCurrentCharacterFlow = g_pServerApp->GetCharacterFlow();
	auto const& CharacterInstanceOpt = GetCharacterInstance(pCurrentCharacterFlow, nOtherCharacter);

	if (CharacterInstanceOpt != std::nullopt) {
		auto& CharacterInstance = CharacterInstanceOpt->get();
		auto const& PlacementContext = CharacterInstance.PlacementContext;

		CBitStream BitStream{};
		BitStream.Write(nOtherCharacter);
		BitStream.Write(PlacementContext);

		g_pServerApp->GetNetServer()->RPC(RPC_CHARACTER_WORLD_ADD, &BitStream, PRIORITY_MEDIUM, RELIABILITY_RELIABLE_ORDERED, nCharacter, false);

		CharacterInstance.LastBroadcastTime = {};
	}
}

void CCharacterFlow::VisibilityRemoveCallback(un::EntityId const& nCharacter, un::EntityId const& nOtherCharacter) {
	printf("[Server:Debug] Character %i don't see character %i anymore\n", nCharacter, nOtherCharacter);

	CBitStream BitStream{};
	BitStream.Write(nOtherCharacter);

	g_pServerApp->GetNetServer()->RPC(RPC_CHARACTER_WORLD_REMOVE, &BitStream, PRIORITY_MEDIUM, RELIABILITY_RELIABLE_ORDERED, nCharacter, false);
}
