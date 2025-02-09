#pragma once

#include <gl/soglb_fwd.h>
#include <glm/vec3.hpp>
#include <gsl/gsl-lite.hpp>
#include <optional>
#include <tuple>
#include <vector>

namespace render::scene
{
class RenderContext;
class Node;

class Visitor
{
public:
  explicit Visitor(RenderContext& context, bool withScissors = true)
      : m_context{context}
      , m_withScissors{withScissors}
  {
  }

  Visitor(const Visitor&) = delete;
  Visitor(Visitor&&) = delete;
  Visitor& operator=(Visitor&&) = delete;
  Visitor& operator=(const Visitor&) = delete;

  virtual ~Visitor();

  virtual void visit(const Node& node);

  RenderContext& getContext()
  {
    return m_context;
  }

  [[nodiscard]] bool withScissors() const
  {
    return m_withScissors;
  }

  void add(const gsl::not_null<const Node*>& node);

  void render(const std::optional<glm::vec3>& camera) const;

private:
  RenderContext& m_context;
  const bool m_withScissors;
  using RenderableInfo = std::tuple<gsl::not_null<const Node*>, gl::RenderState>;
  mutable std::vector<RenderableInfo> m_nodes;
};
} // namespace render::scene
