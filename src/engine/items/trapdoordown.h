#pragma once

#include "itemnode.h"


namespace engine
{
    namespace items
    {
        class TrapDoorDown final : public ModelItemNode
        {
        public:
            TrapDoorDown(const gsl::not_null<level::Level*>& level,
                         const std::string& name,
                         const gsl::not_null<const loader::Room*>& room,
                         const core::Angle& angle,
                         const core::TRCoordinates& position,
                         uint16_t activationState,
                         int16_t darkness,
                         const loader::SkeletalModelType& animatedModel)
                : ModelItemNode(level, name, room, angle, position, activationState, true, SaveHitpoints | SaveFlags, darkness, animatedModel)
            {
            }


            void update() override
            {
                if( m_state.updateActivationTimeout() )
                {
                    if( getCurrentState() == 0 )
                        m_state.goal_anim_state = 1;
                }
                else if( getCurrentState() == 1 )
                {
                    m_state.goal_anim_state = 0;
                }
            }


            void onInteract(LaraNode& /*lara*/) override
            {
            }


            void patchFloor(const core::TRCoordinates& pos, int& y) override
            {
                if( getCurrentState() != 0 || !possiblyOnTrapdoor(pos) || pos.Y > m_state.position.position.Y
                    || y <= m_state.position.position.Y )
                    return;

                y = m_state.position.position.Y;
            }


            void patchCeiling(const core::TRCoordinates& pos, int& y) override
            {
                if( getCurrentState() != 0 || !possiblyOnTrapdoor(pos) || pos.Y <= m_state.position.position.Y
                    || y > m_state.position.position.Y )
                    return;

                y = m_state.position.position.Y + loader::QuarterSectorSize;
            }


        private:
            bool possiblyOnTrapdoor(const core::TRCoordinates& pos) const
            {
                auto trapdoorSectorX = m_state.position.position.X / loader::SectorSize;
                auto trapdoorSectorZ = m_state.position.position.Z / loader::SectorSize;
                auto posSectorX = pos.X / loader::SectorSize;
                auto posSectorZ = pos.Z / loader::SectorSize;
                auto trapdoorAxis = core::axisFromAngle(m_state.rotation.Y, 1_au);
                BOOST_ASSERT(trapdoorAxis.is_initialized());

                if( *trapdoorAxis == core::Axis::PosZ && trapdoorSectorX == posSectorX && (trapdoorSectorZ + 1 == posSectorZ || trapdoorSectorZ == posSectorZ) )
                    return true;
                if( *trapdoorAxis == core::Axis::NegZ && trapdoorSectorX == posSectorX && (trapdoorSectorZ - 1 == posSectorZ || trapdoorSectorZ == posSectorZ) )
                    return true;
                if( *trapdoorAxis == core::Axis::PosX && trapdoorSectorZ == posSectorZ && (trapdoorSectorX + 1 == posSectorX || trapdoorSectorX == posSectorX) )
                    return true;
                if( *trapdoorAxis == core::Axis::NegX && trapdoorSectorZ == posSectorZ && (trapdoorSectorX - 1 == posSectorX || trapdoorSectorX == posSectorX) )
                    return true;

                return false;
            }
        };
    }
}
