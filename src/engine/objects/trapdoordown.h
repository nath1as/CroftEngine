#pragma once

#include "core/units.h"
#include "core/vec.h"
#include "modelobject.h"
#include "serialization/serialization_fwd.h"

#include <gsl/gsl-lite.hpp>
#include <string>

namespace engine
{
struct Location;
}

namespace engine::world
{
class World;
struct Room;
struct SkeletalModelType;
} // namespace engine::world

namespace loader::file
{
struct Item;
}

namespace engine::objects
{
class TrapDoorDown final : public ModelObject
{
public:
  TrapDoorDown(const gsl::not_null<world::World*>& world, const Location& location)
      : ModelObject{world, location}
  {
  }

  TrapDoorDown(const std::string& name,
               const gsl::not_null<world::World*>& world,
               const gsl::not_null<const world::Room*>& room,
               const loader::file::Item& item,
               const gsl::not_null<const world::SkeletalModelType*>& animatedModel);

  void update() override;

  void patchFloor(const core::TRVec& pos, core::Length& y) override;

  void patchCeiling(const core::TRVec& pos, core::Length& y) override;

  void serialize(const serialization::Serializer<world::World>& ser) override;

private:
  bool possiblyOnTrapdoor(const core::TRVec& pos) const;
};
} // namespace engine::objects
