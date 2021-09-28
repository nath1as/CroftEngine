#pragma once

#include "engine/engineconfig.h"
#include "ui/widgets/widget.h"

#include <functional>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

namespace engine
{
class Presenter;
}

namespace ui
{
class Ui;
}

namespace hid
{
enum class Action;
}

namespace ui::widgets
{
class GridBox;
class GroupBox;
} // namespace ui::widgets

namespace menu
{
class ControlsWidget final : public ui::widgets::Widget
{
public:
  explicit ControlsWidget(const engine::NamedInputMappingConfig& mappingConfig);

  [[nodiscard]] glm::ivec2 getPosition() const override;

  [[nodiscard]] glm::ivec2 getSize() const override;

  void setPosition(const glm::ivec2& position) override;

  void setSize(const glm::ivec2& size) override;

  void update(bool hasFocus) override;

  void fitToContent() override;

  void draw(ui::Ui& ui, const engine::Presenter& presenter) const override;

  [[nodiscard]] const gsl::not_null<std::shared_ptr<ui::widgets::GridBox>>& getCurrentGridBox() const;

  void nextRow();

  void prevRow();

  void nextColumn();

  void prevColumn();

  void updateBindings(
    const engine::NamedInputMappingConfig& mappingConfig,
    const std::function<std::shared_ptr<ui::widgets::Widget>(const engine::InputMappingConfig&, hid::Action)>& factory);

  [[nodiscard]] hid::Action getCurrentAction() const;

private:
  gsl::not_null<std::shared_ptr<ui::widgets::GridBox>> m_content;
  gsl::not_null<std::shared_ptr<ui::widgets::GroupBox>> m_container;
  std::vector<gsl::not_null<std::shared_ptr<ui::widgets::GridBox>>> m_controlGroups;
};
} // namespace menu
