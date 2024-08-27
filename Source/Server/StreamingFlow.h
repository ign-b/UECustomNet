#pragma once

#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <functional>

struct SCellCoords {
    short nX, nY;

    SCellCoords() : nX(0),
                    nY(0) {
    }

    SCellCoords(short sInX, short sInY) : nX(sInX),
                                          nY(sInY) {
    }

    SCellCoords &operator=(SCellCoords const &InCell) {
        nX = InCell.nX;
        nY = InCell.nY;
        return *this;
    }

    bool operator==(SCellCoords const &InCell) const {
        return nX == InCell.nX && nY == InCell.nY;
    }
};

template<typename T>
void hash_combine(std::size_t &seed, T const &v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<>
struct std::hash<SCellCoords> {
    std::size_t operator()(SCellCoords const &data) const noexcept {
        std::size_t seed = 0;
        hash_combine(seed, data.nX);
        hash_combine(seed, data.nY);
        return seed;
    }
};

template<typename TIndex, float nGridCellSize>
class CStreamingFlow {
protected:
    using TVisibilityAddCallback = std::function<void(TIndex const &, TIndex const &)>;
    using TVisibilityRemoveCallback = std::function<void(TIndex const &, TIndex const &)>;
    using TBroadcastCallback = std::function<void(TIndex const &, TIndex const &)>;

    using TEntitySet = std::unordered_set<TIndex>;
    using TGridMap = std::unordered_map<SCellCoords, TEntitySet>;

    using TGridCellHandler = std::function<void(SCellCoords const &, TEntitySet const &)>;

protected:
    TVisibilityAddCallback m_cbVisibilityAdd;
    TVisibilityRemoveCallback m_cbVisibilityRemove;
    TBroadcastCallback m_cbBroadcast;

    TGridMap m_GridEntitiesMap;

public:
    CStreamingFlow(
        TVisibilityAddCallback const &cbInVisibilityAdd,
        TVisibilityRemoveCallback const &cbInVisibilityRemove,
        TBroadcastCallback const &cbInBroadcast
    ) : m_cbVisibilityAdd(cbInVisibilityAdd),
        m_cbVisibilityRemove(cbInVisibilityRemove),
        m_cbBroadcast(cbInBroadcast),
        m_GridEntitiesMap() {
    }

protected:
    [[nodiscard]] SCellCoords WorldToCellCoords(float const fX, float const fY) const {
        return {static_cast<short>(fX / nGridCellSize), static_cast<short>(fY / nGridCellSize)};
    }

    [[nodiscard]] bool IsInSuperCell(SCellCoords const &Cell, SCellCoords const &OtherCell) const {
        return std::abs(Cell.nX - OtherCell.nX) <= 1 && std::abs(Cell.nY - OtherCell.nY) <= 1;
    }

    void ForEachSuperCell(SCellCoords const &Cell, TGridCellHandler const &cbExec) const {
        short constexpr SUPERCELL_RANGE = 1;

        for (short nX = -SUPERCELL_RANGE; nX <= SUPERCELL_RANGE; ++nX) {
            for (short nY = -SUPERCELL_RANGE; nY <= SUPERCELL_RANGE; ++nY) {
                SCellCoords NeighborCell{static_cast<short>(Cell.nX + nX), static_cast<short>(Cell.nY + nY)};

                auto itCellPlayers = m_GridEntitiesMap.find(NeighborCell);
                if (itCellPlayers == m_GridEntitiesMap.end()) { continue; }

                cbExec(NeighborCell, itCellPlayers->second);
            }
        }
    }

public:
    void AddEntity(TIndex const &nIndex, SCellCoords const &Cell) {
        auto &EntitiesSet = m_GridEntitiesMap[Cell];
        if (EntitiesSet.find(nIndex) != EntitiesSet.end()) { return; }

        EntitiesSet.emplace(nIndex);

        ForEachSuperCell(Cell, [&](SCellCoords const &, TEntitySet const &NeighborEntities) {
            for (auto const &nOtherEntityIndex: NeighborEntities) {
                if (nIndex == nOtherEntityIndex) { continue; }
                m_cbVisibilityAdd(nIndex, nOtherEntityIndex);
                m_cbVisibilityAdd(nOtherEntityIndex, nIndex);
            }
        });
    }

    void RemoveEntity(TIndex const &nIndex, SCellCoords const &Cell) {
        auto itEntitiesSet = m_GridEntitiesMap.find(Cell);
        if (itEntitiesSet == m_GridEntitiesMap.end()) { return; }

        auto &EntitiesSet = itEntitiesSet->second;
        if (EntitiesSet.find(nIndex) == EntitiesSet.end()) { return; }

        EntitiesSet.erase(nIndex);

        if (EntitiesSet.empty()) {
            m_GridEntitiesMap.erase(itEntitiesSet);
        }

        ForEachSuperCell(Cell, [&](SCellCoords const &, TEntitySet const &NeighborEntities) {
            for (auto const &nOtherEntityIndex: NeighborEntities) {
                if (nIndex == nOtherEntityIndex) { continue; }
                m_cbVisibilityRemove(nIndex, nOtherEntityIndex);
                m_cbVisibilityRemove(nOtherEntityIndex, nIndex);
            }
        });
    }

    void UpdateEntityPosition(TIndex const &nIndex, SCellCoords const &OldCell, SCellCoords const &NewCell) {
        if (OldCell == NewCell) return;

        auto itOldCell = m_GridEntitiesMap.find(OldCell);
        if (itOldCell == m_GridEntitiesMap.end()) { return; }

        auto &OldEntitiesSet = itOldCell->second;
        auto itEntityIndex = OldEntitiesSet.find(nIndex);
        if (itEntityIndex == OldEntitiesSet.end()) { return; }

        auto &NewEntitiesSet = m_GridEntitiesMap[NewCell];
        NewEntitiesSet.emplace(*itEntityIndex);
        OldEntitiesSet.erase(itEntityIndex);

        if (OldEntitiesSet.empty()) {
            m_GridEntitiesMap.erase(itOldCell);
        }

        ForEachSuperCell(OldCell, [&](SCellCoords const &NeighborCell, TEntitySet const &NeighborEntities) {
            for (auto &nOtherEntityIndex: NeighborEntities) {
                if (nOtherEntityIndex == nIndex) { continue; }

                if (!IsInSuperCell(NewCell, NeighborCell) &&
                    IsInSuperCell(OldCell, NeighborCell)) {
                    m_cbVisibilityRemove(nOtherEntityIndex, nIndex);
                    m_cbVisibilityRemove(nIndex, nOtherEntityIndex);
                }
            }
        });

        ForEachSuperCell(NewCell, [&](SCellCoords const &NeighborCell, TEntitySet const &NeighborEntities) {
            for (auto &nOtherEntityIndex: NeighborEntities) {
                if (nOtherEntityIndex == nIndex) { continue; }

                if (IsInSuperCell(NewCell, NeighborCell) &&
                    !IsInSuperCell(OldCell, NeighborCell)) {
                    m_cbVisibilityAdd(nOtherEntityIndex, nIndex);
                    m_cbVisibilityAdd(nIndex, nOtherEntityIndex);
                }
            }
        });
    }

    void ProcessEntitiesInGrid() {
        for (auto &[Cell, EntitiesSet]: m_GridEntitiesMap) {
            for (auto &nEntityIndex: EntitiesSet) {
                ForEachSuperCell(Cell, [&](SCellCoords const &, TEntitySet const &NeighborEntities) {
                    for (auto &nOtherEntityIndex: NeighborEntities) {
                        if (nOtherEntityIndex == nEntityIndex) { continue; }
                        m_cbBroadcast(nEntityIndex, nOtherEntityIndex);
                    }
                });
            }
        }
    }
};
