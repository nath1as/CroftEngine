#pragma once

#include "core/magic.h"
#include "loader/animation.h"

#include <gsl/gsl>

namespace level
{
class Level;
}

namespace loader
{
struct SkeletalModelType;
struct Animation;
}

namespace engine
{
namespace items
{
struct ItemState;
}
class SkeletalModelNode
        : public gameplay::Node
{
public:
    explicit SkeletalModelNode(const std::string& id,
                               const gsl::not_null<const level::Level*>& lvl,
                               const loader::SkeletalModelType& mdl);

    void updatePose(engine::items::ItemState& state);

    void
    setAnimIdGlobal(engine::items::ItemState& state, gsl::not_null<const loader::Animation*> animation, uint16_t frame);

    int calculateFloorSpeed(const engine::items::ItemState& state, int frameOffset = 0) const;

    int getAccelleration(const engine::items::ItemState& state) const;

    loader::BoundingBox getBoundingBox(const engine::items::ItemState& state) const;

    void resetPose()
    {
        m_bonePatches.clear();
        m_bonePatches.resize( getChildren().size(), glm::mat4{1.0f} );
    }

    void patchBone(size_t idx, const glm::mat4& m)
    {
        if( m_bonePatches.empty() )
        {
            resetPose();
        }

        BOOST_ASSERT( m_bonePatches.size() == getChildren().size() );
        BOOST_ASSERT( idx < m_bonePatches.size() );

        m_bonePatches[idx] = m;
    }

    bool advanceFrame(engine::items::ItemState& state);


    struct InterpolationInfo
    {
        const loader::AnimFrame* firstFrame = nullptr;
        const loader::AnimFrame* secondFrame = nullptr;
        float bias = 0;

        const loader::AnimFrame* getNearestFrame() const
        {
            if( bias <= 0.5f )
            {
                return firstFrame;
            }
            else
            {
                BOOST_ASSERT( secondFrame != nullptr );
                return secondFrame;
            }
        }
    };


    InterpolationInfo getInterpolationInfo(const engine::items::ItemState& state) const;

    void updatePose(const InterpolationInfo& interpolationInfo)
    {
        if( interpolationInfo.bias == 0 || interpolationInfo.secondFrame == nullptr )
            updatePoseKeyframe( interpolationInfo );
        else
            updatePoseInterpolated( interpolationInfo );
    }


    struct Sphere
    {
        const glm::mat4 m;
        const int radius;

        Sphere(const glm::mat4& m, int radius)
                : m{m}
                , radius{radius}
        {
        }

        glm::vec3 getPosition() const
        {
            return glm::vec3( m[3] );
        }
    };


    std::vector<Sphere> getBoneCollisionSpheres(const engine::items::ItemState& state,
                                                const loader::AnimFrame& frame,
                                                const glm::mat4* baseTransform);

protected:
    bool handleStateTransitions(engine::items::ItemState& state);

private:
    const gsl::not_null<const level::Level*> m_level;
    const loader::SkeletalModelType& m_model;
    std::vector<glm::mat4> m_bonePatches;

    void updatePoseKeyframe(const InterpolationInfo& framepair);

    void updatePoseInterpolated(const InterpolationInfo& framepair);
};
}
