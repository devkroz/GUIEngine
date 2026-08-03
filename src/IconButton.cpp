#include "GUIEngine/IconButton.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

IconButton::IconButton(const std::string& icon) : Button(icon) {
    m_layout.minWidth = 36;
    m_layout.minHeight = 36;
    m_layout.fixedWidth = 36;
    m_layout.fixedHeight = 36;
    m_padding = Padding(0);
    if (m_label) m_label->setText(icon);
}

Vec2 IconButton::measureContent(float availableWidth, float availableHeight) {
    return Vec2(36, 36);
}

void IconButton::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = resolveColor("buttonColor", "", theme.get().getColor("buttonColor", Color(0.22f, 0.22f, 0.26f)));
    float radius = m_style.getFloat("borderRadius", theme.get().getFloat("borderRadius", 3.0f));

    if (m_state == WidgetState::Hovered) {
        bgColor = Color(bgColor.r + 0.05f, bgColor.g + 0.05f, bgColor.b + 0.05f, bgColor.a);
    } else if (m_state == WidgetState::Pressed) {
        bgColor = Color(bgColor.r * 0.85f, bgColor.g * 0.85f, bgColor.b * 0.85f, bgColor.a);
    }

    renderer.drawRect(m_geometry, bgColor, radius);

    if (m_label) {
        m_label->setPosition(m_geometry.x, m_geometry.y);
        m_label->setSize(m_geometry.width, m_geometry.height);
        m_label->setAlign(Label::Alignment::Center);
        m_label->render(renderer);
    }
}

}
