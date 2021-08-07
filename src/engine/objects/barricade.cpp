#include "barricade.h"

#include "engine/world/world.h"
#include "laraobject.h"

namespace engine::objects
{
void Barricade::update()
{
  if(m_state.updateActivationTimeout())
  {
    m_state.goal_anim_state = 1_as;
  }
  else
  {
    m_state.goal_anim_state = 0_as;
  }

  ModelObject::update();
  m_state.location.updateRoom();
  setCurrentRoom(m_state.location.room);
}

void Barricade::collide(CollisionInfo& collisionInfo)
{
  collideWithLara(collisionInfo);
}
} // namespace engine::objects
