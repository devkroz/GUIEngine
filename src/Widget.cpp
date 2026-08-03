#include "GUIEngine/Widget.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

Widget::Widget(const std::string& id) : m_id(id) {}

Widget::~Widget() = default;

void Widget::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (!enabled) {
        m_state = WidgetState::Disabled;
    } else if (m_state == WidgetState::Disabled) {
        m_state = WidgetState::Normal;
    }
}

void Widget::setState(WidgetState state) {
    m_state = state;
}

Vec2 Widget::measureContent(float availableWidth, float availableHeight) {
    float w = m_layout.fixedWidth;
    float h = m_layout.fixedHeight;

    float marginW = m_layout.margin.left + m_layout.margin.right;
    float marginH = m_layout.margin.top + m_layout.margin.bottom;

    Vec2 contentSize;

    float availW = availableWidth - marginW;
    float availH = availableHeight - marginH;

    float maxW = m_layout.maxWidth > 0 ? std::min(m_layout.maxWidth, availW) : availW;
    float maxH = m_layout.maxHeight > 0 ? std::min(m_layout.maxHeight, availH) : availH;

    for (Widget* child : m_childRefs) {
        if (!child->isVisible()) continue;
        Vec2 childSize = child->measure(maxW, maxH);

        if (child->m_layout.direction == LayoutDirection::Row) {
            contentSize.x += childSize.x;
            contentSize.y = std::max(contentSize.y, childSize.y);
        } else {
            contentSize.x = std::max(contentSize.x, childSize.x);
            contentSize.y += childSize.y;
        }
    }

    if (w < 0) w = std::max(contentSize.x, m_layout.minWidth);
    else w = std::max(w, m_layout.minWidth);

    if (h < 0) h = std::max(contentSize.y, m_layout.minHeight);
    else h = std::max(h, m_layout.minHeight);

    if (m_layout.maxWidth > 0) w = std::min(w, m_layout.maxWidth);
    if (m_layout.maxHeight > 0) h = std::min(h, m_layout.maxHeight);

    w += marginW;
    h += marginH;

    return Vec2(w, h);
}

Vec2 Widget::measure(float availableWidth, float availableHeight) {
    if (!m_visible) return Vec2(0, 0);
    Vec2 size = measureContent(availableWidth, availableHeight);
    m_geometry.width = size.x;
    m_geometry.height = size.y;
    return size;
}

void Widget::layout(const Rect& bounds) {
    if (!m_visible) return;

    Rect contentRect;
    contentRect.x = bounds.x + m_layout.margin.left + m_layout.padding.left;
    contentRect.y = bounds.y + m_layout.margin.top + m_layout.padding.top;
    contentRect.width = bounds.width - m_layout.margin.left - m_layout.margin.right
                        - m_layout.padding.left - m_layout.padding.right;
    contentRect.height = bounds.height - m_layout.margin.top - m_layout.margin.bottom
                         - m_layout.padding.top - m_layout.padding.bottom;

    if (contentRect.width < 0) contentRect.width = 0;
    if (contentRect.height < 0) contentRect.height = 0;

    m_geometry.x = bounds.x + m_layout.margin.left;
    m_geometry.y = bounds.y + m_layout.margin.top;
    m_geometry.width = bounds.width - m_layout.margin.left - m_layout.margin.right;
    m_geometry.height = bounds.height - m_layout.margin.top - m_layout.margin.bottom;

    if (m_geometry.width < 0) m_geometry.width = 0;
    if (m_geometry.height < 0) m_geometry.height = 0;

    if (m_childRefs.empty()) return;

    float fixedWidth = 0, fixedHeight = 0;
    float totalGrow = 0;
    int flexCount = 0;

    for (Widget* child : m_childRefs) {
        if (!child->isVisible()) continue;
        Vec2 childSize = child->measure(contentRect.width, contentRect.height);
        if (child->m_layout.fixedWidth >= 0) fixedWidth += childSize.x;
        if (child->m_layout.fixedHeight >= 0) fixedHeight += childSize.y;
        totalGrow += child->m_layout.grow;
        if (child->m_layout.grow > 0) flexCount++;
    }

    float mainAxis = (m_layout.direction == LayoutDirection::Row) ?
        contentRect.width : contentRect.height;
    float crossAxis = (m_layout.direction == LayoutDirection::Row) ?
        contentRect.height : contentRect.width;

    float frameTotal = (m_layout.direction == LayoutDirection::Row) ?
        fixedWidth : fixedHeight;
    float freeSpace = mainAxis - frameTotal - m_layout.spacing * (m_childRefs.size() - 1);
    if (freeSpace < 0) freeSpace = 0;

    float offset = 0;
    float spacingExtra = 0;

    if (m_layout.justifyContent == Justify::Center) {
        offset = freeSpace * 0.5f;
    } else if (m_layout.justifyContent == Justify::End) {
        offset = freeSpace;
    } else if (m_layout.justifyContent == Justify::SpaceBetween && m_childRefs.size() > 1) {
        spacingExtra = freeSpace / (m_childRefs.size() - 1);
    } else if (m_layout.justifyContent == Justify::SpaceEvenly && m_childRefs.size() > 0) {
        spacingExtra = freeSpace / (m_childRefs.size() + 1);
        offset = spacingExtra;
    } else if (m_layout.justifyContent == Justify::SpaceAround && m_childRefs.size() > 0) {
        spacingExtra = freeSpace / m_childRefs.size();
        offset = spacingExtra * 0.5f;
    }

    float currentMain = contentRect.x;
    if (m_layout.direction == LayoutDirection::Row) currentMain = contentRect.x + offset;
    else currentMain = contentRect.y + offset;

    for (Widget* child : m_childRefs) {
        if (!child->isVisible()) continue;

        Vec2 childSize = child->measure(contentRect.width, contentRect.height);

        float childMain = (m_layout.direction == LayoutDirection::Row) ?
            childSize.x : childSize.y;
        float childCross = (m_layout.direction == LayoutDirection::Row) ?
            childSize.y : childSize.x;

        if (child->m_layout.grow > 0 && totalGrow > 0 && freeSpace > 0) {
            float extra = freeSpace * (child->m_layout.grow / totalGrow);
            childMain += extra;
            if (m_layout.direction == LayoutDirection::Row) childSize.x += extra;
            else childSize.y += extra;
        }

        float crossPos = contentRect.y;
        if (m_layout.direction == LayoutDirection::Row) {
            crossPos = contentRect.y;
        } else {
            crossPos = contentRect.x;
        }

        if (m_layout.itemAlignment == Alignment::Center) {
            crossPos += (crossAxis - childCross) * 0.5f;
        } else if (m_layout.itemAlignment == Alignment::End) {
            crossPos += crossAxis - childCross;
        } else if (m_layout.itemAlignment == Alignment::Stretch) {
            childCross = crossAxis;
            if (m_layout.direction == LayoutDirection::Row) childSize.y = crossAxis;
            else childSize.x = crossAxis;
        }

        Rect childRect;
        if (m_layout.direction == LayoutDirection::Row) {
            childRect.x = currentMain;
            childRect.y = crossPos;
            childRect.width = childSize.x;
            childRect.height = childSize.y;
            currentMain += childSize.x + m_layout.spacing + spacingExtra;
        } else {
            childRect.y = currentMain;
            childRect.x = crossPos;
            childRect.width = childSize.x;
            childRect.height = childSize.y;
            currentMain += childSize.y + m_layout.spacing + spacingExtra;
        }

        child->layout(childRect);
    }
}

void Widget::addChild(std::unique_ptr<Widget> child) {
    child->setParent(this);
    m_childRefs.push_back(child.get());
    m_children.push_back(std::move(child));
}

void Widget::removeChild(Widget* child) {
    m_childRefs.erase(
        std::remove(m_childRefs.begin(), m_childRefs.end(), child),
        m_childRefs.end()
    );
    m_children.erase(
        std::remove_if(m_children.begin(), m_children.end(),
            [child](const std::unique_ptr<Widget>& c) { return c.get() == child; }),
        m_children.end()
    );
}

void Widget::removeAllChildren() {
    m_childRefs.clear();
    m_children.clear();
}

const std::vector<Widget*>& Widget::getChildren() const {
    return m_childRefs;
}

std::vector<Widget*>& Widget::getChildren() {
    return m_childRefs;
}

void Widget::handleEvent(const Event& event) {
    if (!m_visible || !m_enabled) return;

    switch (event.type) {
        case EventType::MouseButtonPress: {
            if (event.mouseButton == 1) {
                m_pressed = true;
                setState(WidgetState::Pressed);
                if (m_onPressed) m_onPressed();
            }
            break;
        }
        case EventType::MouseButtonRelease: {
            if (event.mouseButton == 1) {
                if (m_pressed) {
                    m_pressed = false;
                    if (event.mouseX >= m_geometry.x &&
                        event.mouseX <= m_geometry.x + m_geometry.width &&
                        event.mouseY >= m_geometry.y &&
                        event.mouseY <= m_geometry.y + m_geometry.height) {
                        if (m_onClick) m_onClick();
                    }
                }
                setState(m_hovered ? WidgetState::Hovered : WidgetState::Normal);
                if (m_onReleased) m_onReleased();
            }
            break;
        }
        case EventType::MouseEnter: {
            m_hovered = true;
            if (!m_pressed) setState(WidgetState::Hovered);
            if (m_onHoverEnter) m_onHoverEnter();
            break;
        }
        case EventType::MouseLeave: {
            m_hovered = false;
            if (!m_pressed) setState(m_enabled ? WidgetState::Normal : WidgetState::Disabled);
            if (m_onHoverLeave) m_onHoverLeave();
            break;
        }
        case EventType::KeyPress: {
            if (m_onKey) {
                m_onKey(event);
            }
            break;
        }
        case EventType::FocusIn: {
            m_focused = true;
            setState(WidgetState::Focused);
            if (m_onFocusChange) m_onFocusChange(true);
            break;
        }
        case EventType::FocusOut: {
            m_focused = false;
            setState(m_hovered ? WidgetState::Hovered : WidgetState::Normal);
            if (m_onFocusChange) m_onFocusChange(false);
            break;
        }
        case EventType::Resize: {
            if (m_onResize) m_onResize(event.width, event.height);
            break;
        }
        default:
            break;
    }

    if (onEvent(event)) return;
}

void Widget::focus() {
    m_focused = true;
    setState(WidgetState::Focused);
    if (m_onFocusChange) m_onFocusChange(true);
}

void Widget::unfocus() {
    m_focused = false;
    setState(m_hovered ? WidgetState::Hovered : WidgetState::Normal);
    if (m_onFocusChange) m_onFocusChange(false);
}

void Widget::notifyParentLayoutChanged() {
    if (m_parent) {
    }
}

}
