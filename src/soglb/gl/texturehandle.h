#pragma once

#include "api/gl.hpp" // IWYU pragma: export
#include "glassert.h"
#include "sampler.h"
#include "texture.h"

#include <gslu.h>
#include <type_traits>
#include <utility>

namespace gl
{
// NOLINTNEXTLINE(bugprone-reserved-identifier)
template<typename _Texture>
class TextureHandle final
{
public:
  using Texture = _Texture;
  static_assert(std::is_base_of_v<::gl::Texture, Texture>);

  explicit TextureHandle(gslu::nn_shared<Texture> texture, gslu::nn_unique<Sampler>&& sampler)
      : m_texture{std::move(texture)}
      , m_sampler{std::move(sampler)}
      , m_handle{GL_ASSERT_FN(api::getTextureSamplerHandle(m_texture->getHandle(), m_sampler->getHandle()))}
  {
    gsl_Ensures(m_handle != 0);
    GL_ASSERT(api::makeTextureHandleResident(m_handle));
  }

  ~TextureHandle()
  {
//#define NVIDIA_NSIGHT_HACK
#ifdef NVIDIA_NSIGHT_HACK
    api::makeTextureHandleNonResident(m_handle);
    api::getError();
#else
    GL_ASSERT(api::makeTextureHandleNonResident(m_handle));
#endif
  }

  [[nodiscard]] auto getHandle() const
  {
    return m_handle;
  }

  [[nodiscard]] const auto& getTexture() const
  {
    return m_texture;
  }

private:
  const gslu::nn_shared<Texture> m_texture;
  const gslu::nn_unique<Sampler> m_sampler;
  const uint64_t m_handle;
};
} // namespace gl
