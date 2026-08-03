#include "GUIEngine/ScrollView.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

ScrollView::ScrollView() : Panel() {
    m_layout.padding = Padding(0);
    m_showScrollbars = true;
    m_autoHideScrollbars = true;
}

void ScrollView::setContentSize(float width, float height) {
    m_contentWidth = width;
    m_contentHeight = height;
    clampScroll();
}

Vec2 ScrollView::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    return size;
}

void ScrollView::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("panelColor", theme.get().getColor("panelColor", Color(0.16f, 0.16f, 0.19f)));
    float radius = m_style.getFloat("borderRadius", 0.0f);

    if (!isClipped(m_geometry))
        renderer.drawRect(m_geometry, bgColor, radius);

    renderer.pushScissor(m_geometry);

    Rect clipRect(m_geometry.x, m_geometry.y, m_geometry.width, m_geometry.height);

    for (Widget* child : m_childRefs) {
        if (!child->isVisible()) continue;

        Rect childRect = child->getGeometry();
        Rect scrolledRect(childRect.x - m_scrollX, childRect.y - m_scrollY, childRect.width, childRect.height);

        if (!scrolledRect.intersects(clipRect)) continue;

        Vec2 origPos = child->getPosition();
        child->setPosition(origPos.x - m_scrollX, origPos.y - m_scrollY);
        child->render(renderer);
        child->setPosition(origPos.x, origPos.y);
    }

    renderer.popScissor();

    if (m_showScrollbars && (!m_autoHideScrollbars || m_scrollbarAlpha > 0.01f)) {
        Color scrollbarColor(0.5f, 0.5f, 0.55f, m_scrollbarAlpha * 0.7f);
        Color scrollbarHover(0.6f, 0.6f, 0.65f, m_scrollbarAlpha);

        if (needsVerticalScrollbar()) {
            Rect vsb = getVerticalScrollbar();
            renderer.drawRect(vsb, scrollbarColor, 3);
        }

        if (needsHorizontalScrollbar()) {
            Rect hsb = getHorizontalScrollbar();
            renderer.drawRect(hsb, scrollbarColor, 3);
        }
    }
}

void ScrollView::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    switch (event.type) {
        case EventType::MouseScroll: {
            if (hitTest(event.mouseX, event.mouseY)) {
                m_scrollbarAlpha = 1.0f;
                m_scrollY -= event.scrollY * m_scrollSpeed;
                clampScroll();
            }
            break;
        }
        case EventType::MouseButtonPress: {
            if (event.mouseButton == 1) {
                if (needsVerticalScrollbar()) {
                    Rect vsb = getVerticalScrollbar();
                    if (vsb.contains(event.mouseX, event.mouseY)) {
                        m_draggingV = true;
                        m_dragStartScroll = m_scrollY;
                        m_dragStartPos = event.mouseY;
                        break;
                    }
                }
                if (needsHorizontalScrollbar()) {
                    Rect hsb = getHorizontalScrollbar();
                    if (hsb.contains(event.mouseX, event.mouseY)) {
                        m_draggingH = true;
                        m_dragStartScroll = m_scrollX;
                        m_dragStartPos = event.mouseX;
                        break;
                    }
                }
                if (hitTest(event.mouseX, event.mouseY)) {
                }
            }
            break;
        }
        case EventType::MouseButtonRelease: {
            if (event.mouseButton == 1) {
                m_draggingV = false;
                m_draggingH = false;
            }
            break;
        }
        case EventType::MouseMove: {
            if (hitTest(event.mouseX, event.mouseY) || m_draggingV || m_draggingH) {
                m_scrollbarAlpha = 1.0f;
            }
            if (m_draggingV) {
                float delta = event.mouseY - m_dragStartPos;
                float trackLength = m_geometry.height;
                float contentRatio = m_contentHeight > 0 ? trackLength / m_contentHeight : 1;
                m_scrollY = m_dragStartScroll - delta / contentRatio;
                clampScroll();
            }
            if (m_draggingH) {
                float delta = event.mouseX - m_dragStartPos;
                float trackLength = m_geometry.width;
                float contentRatio = m_contentWidth > 0 ? trackLength / m_contentWidth : 1;
                m_scrollX = m_dragStartScroll - delta / contentRatio;
                clampScroll();
            }
            break;
        }
        default:
            break;
    }
}

void ScrollView::update(float deltaTime) {
    m_scrollbarAlpha = std::max(0.0f, m_scrollbarAlpha - deltaTime * 2.0f);
}

void ScrollView::clampScroll() {
    m_maxScrollY = std::max(0.0f, m_contentHeight - m_geometry.height);
    m_maxScrollX = std::max(0.0f, m_contentWidth - m_geometry.width);
    m_scrollY = std::clamp(m_scrollY, 0.0f, m_maxScrollY);
    m_scrollX = std::clamp(m_scrollX, 0.0f, m_maxScrollX);
}

Rect ScrollView::getVerticalScrollbar() const {
    float trackHeight = m_geometry.height;
    float thumbRatio = m_contentHeight > 0 ? m_geometry.height / m_contentHeight : 1;
    thumbRatio = std::min(thumbRatio, 1.0f);
    float thumbHeight = trackHeight * thumbRatio;
    float thumbY = m_geometry.y + (trackHeight - thumbHeight) * (m_scrollY / std::max(1.0f, m_maxScrollY));
    return Rect(m_geometry.x + m_geometry.width - 8, thumbY, 6, thumbHeight);
}

Rect ScrollView::getHorizontalScrollbar() const {
    float trackWidth = m_geometry.width;
    float thumbRatio = m_contentWidth > 0 ? m_geometry.width / m_contentWidth : 1;
    thumbRatio = std::min(thumbRatio, 1.0f);
    float thumbWidth = trackWidth * thumbRatio;
    float thumbX = m_geometry.x + (trackWidth - thumbWidth) * (m_scrollX / std::max(1.0f, m_maxScrollX));
    return Rect(thumbX, m_geometry.y + m_geometry.height - 8, thumbWidth, 6);
}

bool ScrollView::needsVerticalScrollbar() const {
    return m_contentHeight > m_geometry.height;
}

bool ScrollView::needsHorizontalScrollbar() const {
    return m_contentWidth > m_geometry.width;
}

}
