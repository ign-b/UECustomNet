#pragma once

#include "Math/Quaternion.h"
#include "Math/Vector3.h"

struct SPlacementContext {
    CVector3f vecPosition;
    CQuaternionf quatRotation;

    SPlacementContext() : vecPosition(),
                          quatRotation() {
    }

    explicit SPlacementContext(
        CVector3f const &vecInPosition,
        CQuaternionf const &quatInRotation
    ) : vecPosition(vecInPosition),
        quatRotation(quatInRotation) {
    }

    SPlacementContext &operator=(SPlacementContext const &InPlacementContext) {
        vecPosition = InPlacementContext.vecPosition;
        quatRotation = InPlacementContext.quatRotation;
        return *this;
    }

    bool IsNearlyEqual(SPlacementContext const &InPlacementContext, float ErrorTolerance = 1e-6f) const {
        return vecPosition.IsNearlyEqual(InPlacementContext.vecPosition, ErrorTolerance) &&
               quatRotation.IsNearlyEqual(InPlacementContext.quatRotation, ErrorTolerance);
    }
};

struct SMovementContext {
    CVector3f vecVelocity;
    float fGroundSpeed;
    bool bShouldMove;
    bool bIsFalling;

    SMovementContext()
        : vecVelocity()
        , fGroundSpeed(0.0f)
        , bShouldMove(false)
        , bIsFalling(false) {
    }

    SMovementContext(CVector3f const &vecInVelocity, bool fInGroundSpeed, bool bInShouldMove, bool bInIsFalling)
        : vecVelocity(vecInVelocity)
        , fGroundSpeed(fInGroundSpeed)
        , bShouldMove(bInShouldMove)
        , bIsFalling(bInIsFalling) {
    }

    SMovementContext &operator=(SMovementContext const &InMovementContext) {
        vecVelocity = InMovementContext.vecVelocity;
        fGroundSpeed = InMovementContext.fGroundSpeed;
        bShouldMove = InMovementContext.bShouldMove;
        bIsFalling = InMovementContext.bIsFalling;
        return *this;
    }

    bool IsNearlyEqual(SMovementContext const &InMovementContext, float ErrorTolerance = 1e-6f) const {
        return vecVelocity.IsNearlyEqual(InMovementContext.vecVelocity, ErrorTolerance) &&
               std::abs(fGroundSpeed - InMovementContext.fGroundSpeed) <= ErrorTolerance &&
               bShouldMove == InMovementContext.bShouldMove &&
               bIsFalling == InMovementContext.bIsFalling;
    }
};
