#include "tallblock.h"

#include "level/level.h"

namespace engine
{
namespace items
{
void TallBlock::update()
{
    if( m_state.updateActivationTimeout() )
    {
        if( getCurrentState() == 0 )
        {
            loader::Room::patchHeightsForBlock(*this, 2 * loader::SectorSize);
            m_state.goal_anim_state = 1;
        }
    }
    else
    {
        if( getCurrentState() == 1 )
        {
            loader::Room::patchHeightsForBlock(*this, 2 * loader::SectorSize);
            m_state.goal_anim_state = 0;
        }
    }

    ModelItemNode::update();
    const loader::Room* room = m_state.position.room;
    getLevel().findRealFloorSector(m_state.position.position, &room);
    setCurrentRoom(room);

    if( m_triggerState != engine::items::TriggerState::Activated )
    {
        return;
    }

    m_triggerState = engine::items::TriggerState::Enabled;
    loader::Room::patchHeightsForBlock(*this, -2 * loader::SectorSize);
    auto pos = m_state.position.position;
    pos.X = (pos.X / loader::SectorSize) * loader::SectorSize + loader::SectorSize / 2;
    pos.Z = (pos.Z / loader::SectorSize) * loader::SectorSize + loader::SectorSize / 2;
    m_state.position.position = pos;
}
}
}
