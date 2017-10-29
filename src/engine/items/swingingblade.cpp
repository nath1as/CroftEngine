#include "swingingblade.h"

#include "engine/heightinfo.h"
#include "level/level.h"

namespace engine
{
namespace items
{
void SwingingBlade::update()
{
    if( m_state.updateActivationTimeout() )
    {
        if( getCurrentState() == 0 )
        {
            m_state.goal_anim_state = 2;
        }
    }
    else if( getCurrentState() == 2 )
    {
        m_state.goal_anim_state = 0;
    }

    const loader::Room* room = m_state.position.room;
    auto sector = getLevel().findRealFloorSector(m_state.position.position, &room);
    setCurrentRoom(room);
    const int h = HeightInfo::fromFloor(sector, m_state.position.position, getLevel().m_cameraController)
        .distance;
    m_state.floor = h;

    ModelItemNode::update();
}
}
}
