#include "GUIEngine/ProgressBar.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

ProgressBar::ProgressBar() : Widget() {
    m_layout.minHeight = 8;
    m_layout.minWidth = 100;
    m_layout.grow = 1;
}

void ProgressBar::setProgress(float percent) {
    m_progress = std::clamp(percent, 0.0f, 100.0f);
}

Vec2 ProgressBar::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    size.x = std::max(size.x, m_layout.minWidth);
    size.y = std::max(size.y, m_layout.minHeight);
    return size;
}

void ProgressBar::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color trackColor = m_style.getColor("buttonColor", theme.get().getColor("buttonColor", Color(0.22f, 0.22f, 0.26f)));
    Color fillColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    float radius = m_style.getFloat("borderRadius", 4.0f);

    renderer.drawRect(m_geometry, trackColor, radius);

    if (m_indeterminate) {
        float barWidth = m_geometry.width * 0.3f;
        float barX = m_geometry.x + (m_geometry.width - barWidth) * m_animOffset;
        Rect fillRect(barX, m_geometry.y, barWidth, m_geometry.height);
        renderer.pushScissor(m_geometry);
        renderer.drawRect(fillRect, fillColor, radius);
        renderer.popScissor();
    } else {
        float fillW = m_geometry.width * (m_progress / 100.0f);
        if (fillW > 0) {
            Rect fillRect(m_geometry.x, m_geometry.y, fillW, m_geometry.height);
            renderer.pushScissor(m_geometry);
            renderer.drawRect(fillRect, fillColor, radius);
            renderer.popScissor();
        }
    }
}

void ProgressBar::update(float deltaTime) {
    if (m_indeterminate) {
        m_animOffset += deltaTime * 0.5f;
        if (m_animOffset > 1.0f) m_animOffset = -0.3f;
    }
}

}
