#include "GUIEngine/Panel.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

Panel::Panel() : Widget() {
    m_style.set("panelColor", Color(0.16f, 0.16f, 0.19f, 1.0f));
    m_style.set("borderColor", Color(0.3f, 0.3f, 0.35f, 1.0f));
    m_style.set("borderRadius", 8.0f);
    m_layout.padding = Padding(12);
}

Vec2 Panel::measureContent(float availableWidth, float availableHeight) {
    return Widget::measureContent(availableWidth, availableHeight);
}

void Panel::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("panelColor", theme.get().getColor("panelColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    float radius = m_style.getFloat("borderRadius", 8.0f);

    if (m_shadow) {
        renderer.drawShadow(m_geometry, 20, Color(0, 0, 0, 0.3f));
    }

    renderer.drawRect(m_geometry, bgColor, radius);
    renderer.drawRectOutline(m_geometry, borderColor, 1.0f, radius);

    renderer.pushScissor(m_geometry);

    for (Widget* child : m_childRefs) {
        if (child->isVisible()) {
            child->render(renderer);
        }
    }

    renderer.popScissor();
}

}
