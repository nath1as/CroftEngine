#pragma once

#include "itemnode.h"

namespace engine
{
namespace items
{
class StubItem final : public ModelItemNode
{
public:
    StubItem(const gsl::not_null<level::Level*>& level,
             const std::string& name,
             const gsl::not_null<const loader::Room*>& room,
             const loader::Item& item,
             const loader::SkeletalModelType& animatedModel)
            : ModelItemNode( level, name, room, item, false, 0, animatedModel )
    {
    }
};


class ScriptedItem final : public ModelItemNode
{
public:
    ScriptedItem(const gsl::not_null<level::Level*>& level,
                 const std::string& name,
                 const gsl::not_null<const loader::Room*>& room,
                 const loader::Item& item,
                 const loader::SkeletalModelType& animatedModel,
                 const sol::table& objectInfo)
            : ModelItemNode( level, name, room, item, false, objectInfo["flags"], animatedModel )
            , m_objectInfo( objectInfo )
    {
        auto initialise = objectInfo["initialise"];
        if( initialise )
            initialise.call( m_state );
    }

private:
    sol::table m_objectInfo;
};
}
}
