#include "GUIEngine/Checkbox.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

Checkbox::Checkbox(const std::string& label) : Widget(), m_labelText(label) {
    m_focusable = true;
    m_layout.minHeight = 28;
    m_layout.minWidth = 100;

    m_label = add<Label>(label);
    m_label->setAlign(Label::Alignment::Left);
}

void Checkbox::setLabel(const std::string& text) {
    m_labelText = text;
    if (m_label) m_label->setText(text);
}

Vec2 Checkbox::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);

    float labelWidth = 0;
    if (m_label && m_label->isVisible()) {
        Vec2 labelSize = m_label->measure(availableWidth, availableHeight);
        labelWidth = labelSize.x;
    }

    size.x = std::max(size.x, m_boxSize + 8 + labelWidth);
    size.y = std::max(size.y, static_cast<float>(m_boxSize));
    return size;
}

void Checkbox::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("inputColor", theme.get().getColor("inputColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    Color checkColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    float radius = 3.0f;

    if (m_state == WidgetState::Hovered) {
        borderColor = Color(borderColor.r + 0.1f, borderColor.g + 0.1f, borderColor.b + 0.1f, borderColor.a);
    }

    float boxY = m_geometry.y + (m_geometry.height - m_boxSize) * 0.5f;

    Rect boxRect(m_geometry.x, boxY, m_boxSize, m_boxSize);
    renderer.drawRect(boxRect, bgColor, radius);
    renderer.drawRectOutline(boxRect, borderColor, 1.5f, radius);

    if (m_checked) {
        renderer.drawRect(Rect(boxRect.x + 3, boxRect.y + 3, boxRect.width - 6, boxRect.height - 6),
                           checkColor, radius);

        float cx = boxRect.x + m_boxSize * 0.5f;
        float cy = boxRect.y + m_boxSize * 0.5f;
        renderer.drawLine(cx - 4, cy, cx - 1, cy + 3, Color::White(), 2);
        renderer.drawLine(cx - 1, cy + 3, cx + 4, cy - 3, Color::White(), 2);
    }

    if (m_label) {
        m_label->setPosition(m_geometry.x + m_boxSize + 8, m_geometry.y);
        m_label->setSize(m_geometry.width - m_boxSize - 8, m_geometry.height);
        m_label->render(renderer);
    }
}

void Checkbox::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    if (event.type == EventType::MouseButtonPress && event.mouseButton == 1) {
        m_pressed = true;
    }

    if (event.type == EventType::MouseButtonRelease && event.mouseButton == 1) {
        if (m_pressed) {
            m_pressed = false;
            if (hitTest(event.mouseX, event.mouseY)) {
                onClick();
            }
        }
    }
}

void Checkbox::onClick() {
    m_checked = !m_checked;
    if (m_onChange) m_onChange(m_checked);
}

}
