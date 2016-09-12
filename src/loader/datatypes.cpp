#include "datatypes.h"

#include "level/level.h"
#include "render/textureanimator.h"
#include "util/vmath.h"

#include <boost/lexical_cast.hpp>
#include <boost/range/adaptors.hpp>


namespace loader
{
    namespace
    {
#pragma pack(push,1)
        struct RenderVertex
        {
            gameplay::Vector2 texcoord0;
            gameplay::Vector3 position;
            gameplay::Vector4 color;


            static const gameplay::VertexFormat& getFormat()
            {
                static const gameplay::VertexFormat::Element elems[3] = {
                    {gameplay::VertexFormat::TEXCOORD0, 2},
                    {gameplay::VertexFormat::POSITION, 3},
                    {gameplay::VertexFormat::COLOR, 4}
                };
                static const gameplay::VertexFormat fmt{elems, 3};

                return fmt;
            }
        };
#pragma pack(pop)

        struct MeshPart
        {
            using IndexBuffer = std::vector<uint16_t>;

            IndexBuffer indices;
            std::shared_ptr<gameplay::Material> material;
        };


        struct RenderModel
        {
            std::vector<MeshPart> m_parts;


            std::shared_ptr<gameplay::Model> toModel(const gsl::not_null<std::shared_ptr<gameplay::Mesh>>& mesh)
            {
                for( const MeshPart& localPart : m_parts )
                {
                    gameplay::MeshPart* part = mesh->addPart(gameplay::Mesh::PrimitiveType::TRIANGLES, gameplay::Mesh::IndexFormat::INDEX16, localPart.indices.size(), true);
                    part->setIndexData(localPart.indices.data(), 0, localPart.indices.size());
                }

                auto model = std::make_shared<gameplay::Model>(mesh);

                for( size_t i = 0; i < m_parts.size(); ++i )
                {
                    model->setMaterial(m_parts[i].material, i);
                }

                return model;
            }
        };
    }


    std::shared_ptr<gameplay::Node> Room::createSceneNode(int dumpIdx,
                                                          const level::Level& level,
                                                          const std::map<TextureLayoutProxy::TextureKey, std::shared_ptr<gameplay::Material>>& materials,
                                                          const std::vector<std::shared_ptr<gameplay::Texture>>& textures,
                                                          const std::vector<std::shared_ptr<gameplay::Model>>& staticMeshes,
                                                          render::TextureAnimator& animator)
    {
        RenderModel renderModel;
        std::map<TextureLayoutProxy::TextureKey, size_t> texBuffers;
        std::vector<RenderVertex> vbuf;
        auto mesh = gameplay::Mesh::createMesh(RenderVertex::getFormat(), vbuf.size(), true);

        for( const QuadFace& quad : rectangles )
        {
            const TextureLayoutProxy& proxy = level.m_textureProxies.at(quad.proxyId);

            if(texBuffers.find(proxy.textureKey) == texBuffers.end())
            {
                texBuffers[proxy.textureKey] = renderModel.m_parts.size();
                renderModel.m_parts.emplace_back();
                auto it = materials.find(proxy.textureKey);
                Expects(it != materials.end());
                renderModel.m_parts.back().material = it->second;
            }
            const auto partId = texBuffers[proxy.textureKey];

            const auto firstVertex = vbuf.size();
            for(int i = 0; i < 4; ++i)
            {
                RenderVertex iv;
                iv.position = vertices[quad.vertices[i]].position.toRenderSystem();
                iv.color = vertices[quad.vertices[i]].color;
                iv.texcoord0.x = proxy.uvCoordinates[i].xpixel / 255.0f;
                iv.texcoord0.y = proxy.uvCoordinates[i].ypixel / 255.0f;
                vbuf.push_back(iv);
            }

            animator.registerVertex(quad.proxyId, { mesh, partId }, 0, firstVertex + 0);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 0);
            animator.registerVertex(quad.proxyId, { mesh, partId }, 1, firstVertex + 1);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 1);
            animator.registerVertex(quad.proxyId, { mesh, partId }, 2, firstVertex + 2);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 2);
            animator.registerVertex(quad.proxyId, { mesh, partId }, 0, firstVertex + 0);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 0);
            animator.registerVertex(quad.proxyId, { mesh, partId }, 2, firstVertex + 2);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 2);
            animator.registerVertex(quad.proxyId, { mesh, partId }, 3, firstVertex + 3);
            renderModel.m_parts[partId].indices.emplace_back(firstVertex + 3);
        }
        for( const Triangle& tri : triangles )
        {
            const TextureLayoutProxy& proxy = level.m_textureProxies.at(tri.proxyId);

            if(texBuffers.find(proxy.textureKey) == texBuffers.end())
            {
                texBuffers[proxy.textureKey] = renderModel.m_parts.size();
                renderModel.m_parts.emplace_back();
                auto it = materials.find(proxy.textureKey);
                Expects(it != materials.end());
                renderModel.m_parts.back().material = it->second;
            }
            const auto partId = texBuffers[proxy.textureKey];

            const auto firstVertex = vbuf.size();
            for(int i = 0; i < 4; ++i)
            {
                RenderVertex iv;
                iv.position = vertices[tri.vertices[i]].position.toRenderSystem();
                iv.color = vertices[tri.vertices[i]].color;
                iv.texcoord0.x = proxy.uvCoordinates[i].xpixel / 255.0f;
                iv.texcoord0.y = proxy.uvCoordinates[i].ypixel / 255.0f;
                renderModel.m_parts[partId].indices.emplace_back(vbuf.size());
                vbuf.push_back(iv);
            }

            animator.registerVertex(tri.proxyId, { mesh, partId }, 0, firstVertex + 0);
            animator.registerVertex(tri.proxyId, { mesh, partId }, 1, firstVertex + 1);
            animator.registerVertex(tri.proxyId, { mesh, partId }, 2, firstVertex + 2);
        }

        mesh->rebuild(reinterpret_cast<float*>(vbuf.data()), vbuf.size());
        auto resModel = renderModel.toModel(mesh);
        node = std::make_shared<gameplay::Node>("");
        node->setDrawable(resModel);

        for( Light& light : lights )
        {
            const auto f = std::abs(light.specularIntensity) / 8191.0f;
            BOOST_ASSERT(f >= 0 && f <= 1);

            switch( light.getLightType() )
            {
                case LightType::Shadow:
                    BOOST_LOG_TRIVIAL(debug) << "Light: Shadow";
                    light.node = gameplay::Light::createPoint(light.color.r / 255.0f * f, light.color.g / 255.0f * f, light.color.b / 255.0f * f, light.specularFade);
                    break;
                case LightType::Null:
                case LightType::Point:
                    BOOST_LOG_TRIVIAL(debug) << "Light: Null/Point";
                    light.node = gameplay::Light::createPoint(light.color.r / 255.0f * f, light.color.g / 255.0f * f, light.color.b / 255.0f * f, light.specularFade);
                    break;
                case LightType::Spotlight:
                    BOOST_LOG_TRIVIAL(debug) << "Light: Spot";
                    light.node = gameplay::Light::createSpot(light.color.r / 255.0f * f, light.color.g / 255.0f * f, light.color.b / 255.0f * f, light.specularFade, light.r_inner, light.r_outer);
                    break;
                case LightType::Sun:
                    BOOST_LOG_TRIVIAL(debug) << "Light: Sun";
                    light.node = gameplay::Light::createDirectional(light.color.r / 255.0f * f, light.color.g / 255.0f * f, light.color.b / 255.0f * f);
                    break;
            }

            BOOST_LOG_TRIVIAL(debug) << "  - Position: " << light.position.X << "/" << light.position.Y << "/" << light.position.Z;
            BOOST_LOG_TRIVIAL(debug) << "  - Length: " << light.length;
            BOOST_LOG_TRIVIAL(debug) << "  - Color: " << light.color.a / 255.0f << "/" << light.color.r / 255.0f << "/" << light.color.g / 255.0f << "/" << light.color.b / 255.0f;
            BOOST_LOG_TRIVIAL(debug) << "  - Specular Fade: " << light.specularFade;
            BOOST_LOG_TRIVIAL(debug) << "  - Specular Intensity: " << light.specularIntensity;
            BOOST_LOG_TRIVIAL(debug) << "  - Inner: " << light.r_inner;
            BOOST_LOG_TRIVIAL(debug) << "  - Outer: " << light.r_outer;
            BOOST_LOG_TRIVIAL(debug) << "  - Intensity: " << light.intensity;
        }

        for( const RoomStaticMesh& sm : this->staticMeshes )
        {
            auto idx = level.findStaticMeshIndexById(sm.meshId);
            BOOST_ASSERT(idx >= 0);
            BOOST_ASSERT(static_cast<size_t>(idx) < staticMeshes.size());
            auto subNode = std::make_shared<gameplay::Node>("");
            subNode->setDrawable(staticMeshes[idx]);
            subNode->setRotation({0,1,0}, util::auToRad(sm.rotation));
            subNode->setTranslation((sm.position - position).toRenderSystem());
            node->addChild(subNode);
        }
        node->setTranslation(position.toRenderSystem());

        node->setId(("Room:" + boost::lexical_cast<std::string>(dumpIdx)).c_str());

        for( const Sprite& sprite : sprites )
        {
            BOOST_ASSERT(sprite.vertex < vertices.size());
            BOOST_ASSERT(sprite.texture < level.m_spriteTextures.size());

            const SpriteTexture& tex = level.m_spriteTextures[sprite.texture];

            auto spriteNode = gameplay::Sprite::create(textures[tex.texture], tex.right_side - tex.left_side + 1, tex.bottom_side - tex.top_side + 1, tex.buildSourceRectangle());
            spriteNode->setBlendMode(gameplay::Sprite::BLEND_ADDITIVE);

            auto n = std::make_shared<gameplay::Node>("");
            n->setDrawable(spriteNode);
            n->setTranslation((vertices[sprite.vertex].position - core::TRCoordinates{0, tex.bottom_side / 2, 0}).toRenderSystem());

            node->addChild(n);
        }

        // resultNode->addShadowVolumeSceneNode();

        return node;
    }


    std::shared_ptr<gameplay::Texture> DWordTexture::toTexture() const
    {
        auto img = gameplay::Image::create(256, 256, &pixels[0][0]);
        auto tex = gameplay::Texture::create(img, false);
        return tex;
    }


    gameplay::BoundingBox StaticMesh::getCollisionBox(const core::TRCoordinates& pos, core::Angle angle) const
    {
        auto result = collision_box;

        const auto axis = core::axisFromAngle(angle, 45_deg);
        switch( *axis )
        {
            case core::Axis::PosZ:
                // nothing to do
                break;
            case core::Axis::PosX:
                std::swap(result.min.x, result.min.z);
                result.min.z *= -1;
                std::swap(result.max.x, result.max.z);
                result.max.z *= -1;
                break;
            case core::Axis::NegZ:
                result.min.x *= -1;
                result.min.z *= -1;
                result.max.x *= -1;
                result.max.z *= -1;
                break;
            case core::Axis::NegX:
                std::swap(result.min.x, result.min.z);
                result.min.x *= -1;
                std::swap(result.max.x, result.max.z);
                result.max.x *= -1;
                break;
        }

        result.min += gameplay::Vector3(pos.X, pos.Y, pos.Z);
        result.max += gameplay::Vector3(pos.X, pos.Y, pos.Z);
        return result;
    }


    void Room::patchHeightsForBlock(const engine::ItemController& ctrl, int height)
    {
        core::RoomBoundPosition pos = ctrl.getRoomBoundPosition();
        //! @todo Ugly const_cast
        auto groundSector = const_cast<loader::Sector*>(ctrl.getLevel().findFloorSectorWithClampedPosition(pos).get());
        pos.position.Y += height - loader::SectorSize;
        const auto topSector = ctrl.getLevel().findFloorSectorWithClampedPosition(pos);

        const auto q = height / loader::QuarterSectorSize;
        if( groundSector->floorHeight == -127 )
        {
            groundSector->floorHeight = topSector->ceilingHeight + q;
        }
        else
        {
            groundSector->floorHeight += q;
            if( groundSector->floorHeight == topSector->ceilingHeight )
                groundSector->floorHeight = -127;
        }

        if( groundSector->boxIndex == 0xffff )
            return;

        //! @todo Ugly const_cast
        loader::Box& box = const_cast<loader::Box&>(ctrl.getLevel().m_boxes[groundSector->boxIndex]);
        if( (box.overlap_index & 0x8000) == 0 )
            return;

        if( height >= 0 )
            box.overlap_index &= ~0x4000;
        else
            box.overlap_index |= 0x4000;
    }
}
