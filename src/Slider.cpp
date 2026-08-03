#include "GUIEngine/Slider.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

Slider::Slider() : Widget() {
    m_focusable = true;
    m_layout.minHeight = 28;
    m_layout.minWidth = 100;
}

void Slider::setRange(float min, float max) {
    m_min = std::min(min, max);
    m_max = std::max(min, max);
    m_value = std::clamp(m_value, m_min, m_max);
}

void Slider::setValue(float value) {
    m_value = std::clamp(value, m_min, m_max);
    if (m_onChange) m_onChange(m_value);
}

Vec2 Slider::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    if (m_orientation == LayoutDirection::Row) {
        size.x = std::max(size.x, m_layout.minWidth);
        size.y = std::max(size.y, 28.0f);
    } else {
        size.x = std::max(size.x, 28.0f);
        size.y = std::max(size.y, m_layout.minHeight);
    }
    return size;
}

float Slider::getTrackLength() const {
    if (m_orientation == LayoutDirection::Row) {
        return m_geometry.width - getThumbSize();
    }
    return m_geometry.height - getThumbSize();
}

float Slider::getThumbSize() const {
    return 16;
}

void Slider::updateValueFromPosition(float mouseX, float mouseY) {
    float pos = (m_orientation == LayoutDirection::Row) ? mouseX : mouseY;
    float trackStart = (m_orientation == LayoutDirection::Row) ?
        m_geometry.x + getThumbSize() * 0.5f :
        m_geometry.y + getThumbSize() * 0.5f;
    float trackLen = getTrackLength();
    float ratio = (pos - trackStart) / trackLen;
    ratio = std::clamp(ratio, 0.0f, 1.0f);
    float newValue = m_min + ratio * (m_max - m_min);
    if (m_step > 0) {
        newValue = std::round(newValue / m_step) * m_step;
    }
    newValue = std::clamp(newValue, m_min, m_max);
    if (newValue != m_value) {
        m_value = newValue;
        if (m_onChange) m_onChange(m_value);
    }
}

void Slider::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color trackColor = m_style.getColor("buttonColor", theme.get().getColor("buttonColor", Color(0.22f, 0.22f, 0.26f)));
    Color fillColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    Color thumbColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));

    if (m_state == WidgetState::Hovered || m_dragging) {
        thumbColor = Color(thumbColor.r + 0.1f, thumbColor.g + 0.1f, thumbColor.b + 0.1f, thumbColor.a);
    }

    float ratio = (m_value - m_min) / (m_max - m_min);
    float thumbSize = getThumbSize();
    float trackLength = getTrackLength();

    if (m_orientation == LayoutDirection::Row) {
        float trackY = m_geometry.y + m_geometry.height * 0.5f;
        float trackX = m_geometry.x + thumbSize * 0.5f;
        float trackEnd = trackX + trackLength;

        renderer.drawRect(Rect(trackX, trackY - 3, trackLength, 6), trackColor, 3);
        renderer.drawRect(Rect(trackX, trackY - 3, trackLength * ratio, 6), fillColor, 3);

        float thumbX = trackX + trackLength * ratio - thumbSize * 0.5f;
        renderer.drawRect(Rect(thumbX, trackY - thumbSize * 0.5f, thumbSize, thumbSize), thumbColor, thumbSize * 0.5f);
    } else {
        float trackX = m_geometry.x + m_geometry.width * 0.5f;
        float trackY = m_geometry.y + thumbSize * 0.5f;
        float trackEnd = trackY + trackLength;

        renderer.drawRect(Rect(trackX - 3, trackY, 6, trackLength), trackColor, 3);
        renderer.drawRect(Rect(trackX - 3, trackY, 6, trackLength * (1.0f - ratio)), fillColor, 3);

        float thumbY = trackY + trackLength * (1.0f - ratio) - thumbSize * 0.5f;
        renderer.drawRect(Rect(trackX - thumbSize * 0.5f, thumbY, thumbSize, thumbSize), thumbColor, thumbSize * 0.5f);
    }
}

void Slider::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    switch (event.type) {
        case EventType::MouseButtonPress:
            if (event.mouseButton == 1 && hitTest(event.mouseX, event.mouseY)) {
                m_dragging = true;
                updateValueFromPosition(event.mouseX, event.mouseY);
            }
            break;
        case EventType::MouseButtonRelease:
            if (event.mouseButton == 1 && m_dragging) {
                m_dragging = false;
                if (m_onRelease) m_onRelease(m_value);
            }
            break;
        case EventType::MouseMove:
            if (m_dragging) {
                updateValueFromPosition(event.mouseX, event.mouseY);
            }
            break;
        default:
            break;
    }
}

}
