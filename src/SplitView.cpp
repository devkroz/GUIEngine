#include "GUIEngine/SplitView.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

SplitView::SplitView() : Panel() {
    m_layout.padding = Padding(0);
    m_orientation = LayoutDirection::Row;
    m_split = 0.5f;
    m_dividerSize = 6;
}

void SplitView::setSplit(float percentage) {
    m_split = std::clamp(percentage, m_minSplit, m_maxSplit);
    if (m_onSplitChange) m_onSplitChange(m_split);
}

Rect SplitView::getFirstRect() const {
    if (m_orientation == LayoutDirection::Row) {
        float firstWidth = m_geometry.width * m_split - m_dividerSize * 0.5f;
        return Rect(m_geometry.x, m_geometry.y, firstWidth, m_geometry.height);
    } else {
        float firstHeight = m_geometry.height * m_split - m_dividerSize * 0.5f;
        return Rect(m_geometry.x, m_geometry.y, m_geometry.width, firstHeight);
    }
}

Rect SplitView::getSecondRect() const {
    if (m_orientation == LayoutDirection::Row) {
        float secondX = m_geometry.x + m_geometry.width * m_split + m_dividerSize * 0.5f;
        float secondWidth = m_geometry.width * (1.0f - m_split) - m_dividerSize * 0.5f;
        return Rect(secondX, m_geometry.y, secondWidth, m_geometry.height);
    } else {
        float secondY = m_geometry.y + m_geometry.height * m_split + m_dividerSize * 0.5f;
        float secondHeight = m_geometry.height * (1.0f - m_split) - m_dividerSize * 0.5f;
        return Rect(m_geometry.x, secondY, m_geometry.width, secondHeight);
    }
}

Rect SplitView::getDividerRect() const {
    if (m_orientation == LayoutDirection::Row) {
        float divX = m_geometry.x + m_geometry.width * m_split - m_dividerSize * 0.5f;
        return Rect(divX, m_geometry.y, m_dividerSize, m_geometry.height);
    } else {
        float divY = m_geometry.y + m_geometry.height * m_split - m_dividerSize * 0.5f;
        return Rect(m_geometry.x, divY, m_geometry.width, m_dividerSize);
    }
}

Vec2 SplitView::measureContent(float availableWidth, float availableHeight) {
    return Vec2(availableWidth, availableHeight);
}

void SplitView::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color dividerColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    Color dividerHover = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));

    if (m_childRefs.size() >= 1) {
        Rect firstRect = getFirstRect();
        m_childRefs[0]->layout(firstRect);
        renderer.pushScissor(firstRect);
        m_childRefs[0]->render(renderer);
        renderer.popScissor();
    }

    if (m_childRefs.size() >= 2) {
        Rect secondRect = getSecondRect();
        m_childRefs[1]->layout(secondRect);
        renderer.pushScissor(secondRect);
        m_childRefs[1]->render(renderer);
        renderer.popScissor();
    }

    Rect divRect = getDividerRect();
    Color dc = (m_state == WidgetState::Hovered || m_dragging) ? dividerHover : dividerColor;
    renderer.drawRect(divRect, dc);
}

void SplitView::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    Rect divRect = getDividerRect();

    switch (event.type) {
        case EventType::MouseButtonPress:
            if (event.mouseButton == 1 && divRect.contains(event.mouseX, event.mouseY)) {
                m_dragging = true;
            }
            break;
        case EventType::MouseButtonRelease:
            if (event.mouseButton == 1) m_dragging = false;
            break;
        case EventType::MouseMove:
            if (m_dragging) {
                if (m_orientation == LayoutDirection::Row) {
                    float ratio = (event.mouseX - m_geometry.x) / m_geometry.width;
                    setSplit(ratio);
                } else {
                    float ratio = (event.mouseY - m_geometry.y) / m_geometry.height;
                    setSplit(ratio);
                }
            }
            if (divRect.contains(event.mouseX, event.mouseY) || m_dragging) {
                setState(WidgetState::Hovered);
            } else {
                setState(WidgetState::Normal);
            }
            break;
        default:
            break;
    }
}

}
