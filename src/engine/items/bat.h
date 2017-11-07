#pragma once

#include "aiagent.h"

#include "engine/ai/ai.h"


namespace engine
{
    namespace items
    {
        class Bat final
            : public AIAgent
        {
        public:
            Bat(const gsl::not_null<level::Level*>& level,
                const std::string& name,
                const gsl::not_null<const loader::Room*>& room,
                const loader::Item& item,
                const loader::SkeletalModelType& animatedModel)
                : AIAgent(level, name, room, item, SaveHitpoints | SaveFlags | SavePosition | NonLot | Intelligent, animatedModel, 0)
            {
            }


            void update() override;


            void onInteract(LaraNode& /*lara*/) override
            {
            }
        };
    }
}
