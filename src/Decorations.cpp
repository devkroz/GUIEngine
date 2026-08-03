#include "GUIEngine/Decorations.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

Separator::Separator(Orientation dir) : Widget(), m_orientation(dir) {
    if (dir == Orientation::Horizontal) {
        m_layout.minHeight = 1;
        m_layout.grow = 1;
    } else {
        m_layout.minWidth = 1;
        m_layout.grow = 1;
    }
    m_style.set("borderColor", Color(0.3f, 0.3f, 0.35f, 1.0f));
}

Vec2 Separator::measureContent(float availableWidth, float availableHeight) {
    if (m_orientation == Orientation::Horizontal) {
        return Vec2(availableWidth, 1);
    }
    return Vec2(1, availableHeight);
}

void Separator::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color color = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));

    if (m_orientation == Orientation::Horizontal) {
        renderer.drawRect(Rect(m_geometry.x, m_geometry.y, m_geometry.width, 1), color);
    } else {
        renderer.drawRect(Rect(m_geometry.x, m_geometry.y, 1, m_geometry.height), color);
    }
}

Spacer::Spacer() : Widget() {
    m_layout.grow = 1;
}

Spacer::Spacer(float fixedWidth, float fixedHeight) : Widget(), m_fixedWidth(fixedWidth), m_fixedHeight(fixedHeight) {
    m_layout.fixedWidth = fixedWidth;
    m_layout.fixedHeight = fixedHeight;
    m_layout.minWidth = fixedWidth;
    m_layout.minHeight = fixedHeight;
}

Vec2 Spacer::measureContent(float availableWidth, float availableHeight) {
    if (m_fixedWidth > 0 || m_fixedHeight > 0) {
        return Vec2(m_fixedWidth, m_fixedHeight);
    }
    return Vec2(0, 0);
}

Image::Image(const std::string& src) : Widget(), m_src(src) {
    m_layout.minWidth = 32;
    m_layout.minHeight = 32;
}

void Image::setSource(const std::string& src) {
    m_src = src;
    m_loaded = false;
}

Vec2 Image::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    size.x = std::max(size.x, m_layout.minWidth);
    size.y = std::max(size.y, m_layout.minHeight);
    return size;
}

void Image::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bg = m_style.getColor("backgroundColor", Color(0.2f, 0.2f, 0.24f, 1.0f));
    float radius = m_style.getFloat("borderRadius", 4.0f);

    renderer.drawRect(m_geometry, bg, radius);

    if (!m_src.empty() && m_loaded) {
        renderer.drawImage(m_src, m_geometry, m_tint);
    }
}

}
