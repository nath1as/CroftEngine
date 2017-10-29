#pragma once

#include "abstractstatehandler.h"
#include "engine/collisioninfo.h"
#include "engine/inputstate.h"
#include "level/level.h"

namespace engine
{
namespace lara
{
class StateHandler_21 final
    : public AbstractStateHandler
{
public:
    explicit StateHandler_21(LaraNode& lara)
        : AbstractStateHandler(lara, LaraStateId::StepRight)
    {
    }

    void handleInput(CollisionInfo& /*collisionInfo*/) override
    {
        if( getLara().m_state.health <= 0 )
        {
            setTargetState(LaraStateId::Stop);
            return;
        }

        if( getLevel().m_inputHandler->getInputState().stepMovement != AxisMovement::Right )
        {
            setTargetState(LaraStateId::Stop);
        }

        if( getLevel().m_inputHandler->getInputState().xMovement == AxisMovement::Left )
        {
            subYRotationSpeed(2.25_deg, -4_deg);
        }
        else if( getLevel().m_inputHandler->getInputState().xMovement == AxisMovement::Right )
        {
            addYRotationSpeed(2.25_deg, 4_deg);
        }
    }

    void postprocessFrame(CollisionInfo& collisionInfo) override
    {
        getLara().m_state.fallspeed = 0;
        getLara().m_state.falling = false;
        collisionInfo.badPositiveDistance = 128;
        collisionInfo.badNegativeDistance = -128;
        collisionInfo.badCeilingDistance = 0;
        collisionInfo.facingAngle = getLara().m_state.rotation.Y + 90_deg;
        setMovementAngle(collisionInfo.facingAngle);
        collisionInfo.policyFlags |= CollisionInfo::SlopesAreWalls | CollisionInfo::SlopesArePits;
        collisionInfo.initHeightInfo(getLara().m_state.position.position, getLevel(), core::ScalpHeight);

        if( stopIfCeilingBlocked(collisionInfo) )
        {
            return;
        }

        if( checkWallCollision(collisionInfo) )
        {
            setAnimIdGlobal(loader::AnimationId::STAY_SOLID, 185);
            setTargetState(LaraStateId::Stop);
        }

        if( !tryStartSlide(collisionInfo) )
        {
            placeOnFloor(collisionInfo);
        }
    }
};
}
}
