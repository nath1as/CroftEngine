#pragma once

#include <cstdint>
#include <gl/pixel.h>
#include <gl/soglb_fwd.h>
#include <gsl/gsl-lite.hpp>
#include <map>
#include <memory>
#include <tuple>

namespace render::scene
{
class CSM;
class Material;
class Renderer;
class ShaderCache;

class MaterialManager final
{
public:
  explicit MaterialManager(gsl::not_null<std::shared_ptr<ShaderCache>> shaderCache,
                           gsl::not_null<std::shared_ptr<Renderer>> renderer);

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getSprite(bool billboard);

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getCSMDepthOnly(bool skeletal);
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getDepthOnly(bool skeletal);

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getGeometry(bool inWater, bool skeletal, bool roomShadowing);

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getWaterSurface();

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getLightning();

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getWorldComposition(bool inWater, bool dof);

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getUi();

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getFXAA();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getCRT();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getVelvia();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getFilmGrain();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getLensDistortion();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getHBAOFx();

  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>>
    getFlat(bool withAlpha, bool invertY = false, bool withAspectRatio = false);
  [[nodiscard]] const std::shared_ptr<Material>& getBackdrop();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getHBAO();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>> getVSMSquare();
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>>
    getFastGaussBlur(uint8_t extent, uint8_t blurDir, uint8_t blurDim);
  [[nodiscard]] gsl::not_null<std::shared_ptr<Material>>
    getFastBoxBlur(uint8_t extent, uint8_t blurDir, uint8_t blurDim);

  void setGeometryTextures(std::shared_ptr<gl::TextureHandle<gl::Texture2DArray<gl::SRGBA8>>> geometryTextures);
  void setFiltering(bool bilinear, float anisotropyLevel);

  void setCSM(const gsl::not_null<std::shared_ptr<CSM>>& csm)
  {
    m_csm = csm;
  }

private:
  const gsl::not_null<std::shared_ptr<ShaderCache>> m_shaderCache;
  std::shared_ptr<gl::TextureHandle<gl::Texture2D<gl::RGB8>>> m_noiseTexture;

  std::shared_ptr<Material> m_fxaa{nullptr};
  std::shared_ptr<Material> m_crt{nullptr};
  std::shared_ptr<Material> m_velvia{nullptr};
  std::shared_ptr<Material> m_filmGrain{nullptr};
  std::shared_ptr<Material> m_lensDistortion{nullptr};
  std::shared_ptr<Material> m_hbaoFx{nullptr};

  std::map<bool, gsl::not_null<std::shared_ptr<Material>>> m_sprite{};
  std::map<bool, gsl::not_null<std::shared_ptr<Material>>> m_csmDepthOnly{};
  std::map<bool, gsl::not_null<std::shared_ptr<Material>>> m_depthOnly{};
  std::map<std::tuple<bool, bool, bool>, gsl::not_null<std::shared_ptr<Material>>> m_geometry{};
  std::shared_ptr<Material> m_waterSurface{nullptr};
  std::shared_ptr<Material> m_lightning{nullptr};
  std::map<std::tuple<bool, bool>, gsl::not_null<std::shared_ptr<Material>>> m_composition{};
  std::shared_ptr<Material> m_ui{nullptr};
  std::map<std::tuple<bool, bool, bool>, gsl::not_null<std::shared_ptr<Material>>> m_flat{};
  std::map<std::tuple<uint8_t, uint8_t, uint8_t>, gsl::not_null<std::shared_ptr<Material>>> m_fastGaussBlur{};
  std::map<std::tuple<uint8_t, uint8_t, uint8_t>, gsl::not_null<std::shared_ptr<Material>>> m_fastBoxBlur{};
  std::shared_ptr<Material> m_backdrop{nullptr};
  std::shared_ptr<Material> m_hbao{nullptr};
  std::shared_ptr<Material> m_linearDepth{nullptr};
  std::shared_ptr<Material> m_vsmSquare{nullptr};

  std::shared_ptr<CSM> m_csm;
  const gsl::not_null<std::shared_ptr<Renderer>> m_renderer;
  std::shared_ptr<gl::TextureHandle<gl::Texture2DArray<gl::SRGBA8>>> m_geometryTextures;
};
} // namespace render::scene
