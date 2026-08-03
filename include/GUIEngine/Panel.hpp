#pragma once

#include "Widget.hpp"
#include <vector>

namespace GUIEngine {

class Panel : public Widget {
public:
    Panel();
    ~Panel() override = default;

    void setBackgroundColor(const Color& color) { m_style.set("panelColor", color); }
    void setBorderColor(const Color& color) { m_style.set("borderColor", color); }
    void setBorderRadius(float radius) { m_style.set("borderRadius", radius); }
    void setShadow(bool shadow) { m_shadow = shadow; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;

    bool isContainer() const override { return true; }

protected:
    bool m_shadow = false;
};

}
