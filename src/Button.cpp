#include "GUIEngine/Button.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include "GUIEngine/Label.hpp"

namespace GUIEngine {

Button::Button(const std::string& text) : Widget(), m_text(text) {
    m_focusable = true;
    m_layout.minHeight = 36;
    m_layout.minWidth = 80;
    m_layout.padding = Padding(8, 16);

    m_label = add<Label>(text);
    m_label->setAlign(Label::Alignment::Center);
}

void Button::setText(const std::string& text) {
    m_text = text;
    if (m_label) m_label->setText(text);
}

void Button::setPrimary(bool primary) {
    m_primary = primary;
    if (primary) {
        m_style.set("buttonColor", Theme::defaultTheme().get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    }
}

Vec2 Button::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);

    float labelWidth = 0, labelHeight = 20;
    if (m_label && m_label->isVisible()) {
        Vec2 labelSize = m_label->measure(availableWidth, availableHeight);
        labelWidth = labelSize.x;
        labelHeight = labelSize.y;
    }

    float paddingH = m_padding.left + m_padding.right;
    float paddingV = m_padding.top + m_padding.bottom;

    size.x = std::max(size.x, labelWidth + paddingH);
    size.y = std::max(size.y, labelHeight + paddingV);

    if (m_layout.fixedWidth < 0) size.x = std::max(size.x, m_layout.minWidth);
    if (m_layout.fixedHeight < 0) size.y = std::max(size.y, m_layout.minHeight);

    return size;
}

Color Button::resolveColor(const std::string& key1, const std::string& key2, const Color& def) {
    Theme& theme = Theme::defaultTheme();
    Color color = m_style.getColor(key1, theme.get().getColor(key1, def));

    if (m_state == WidgetState::Hovered) {
        Color hover = m_style.getColor(key1 + "Hovered", theme.get().getColor(key1 + "Hovered", color));
        Color h2 = m_style.getColor(key2, theme.get().getColor(key2, color));
        if (m_style.has(key2 + "Hovered")) hover = m_style.getColor(key2 + "Hovered", hover);
        return hover;
    } else if (m_state == WidgetState::Pressed) {
        Color pressed = m_style.getColor(key1 + "Pressed", theme.get().getColor(key1 + "Pressed", color));
        if (m_style.has(key2 + "Pressed")) pressed = m_style.getColor(key2 + "Pressed", pressed);
        return pressed;
    }
    return color;
}

void Button::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = resolveColor("buttonColor", "", theme.get().getColor("buttonColor", Color(0.22f, 0.22f, 0.26f)));
    Color primaryColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    float radius = m_style.getFloat("borderRadius", theme.get().getFloat("borderRadius", 6.0f));
    float borderWidth = m_style.getFloat("borderWidth", theme.get().getFloat("borderWidth", 1.0f));

    if (m_primary) {
        Color primaryHovered = m_style.getColor("primaryColorHovered", theme.get().getColor("primaryColorHovered", Color(0.3f, 0.6f, 1.0f)));
        Color primaryPressed = m_style.getColor("primaryColorPressed", theme.get().getColor("primaryColorPressed", Color(0.15f, 0.4f, 0.85f)));

        if (m_state == WidgetState::Hovered) bgColor = primaryHovered;
        else if (m_state == WidgetState::Pressed) bgColor = primaryPressed;
        else bgColor = primaryColor;
    }

    renderer.drawRect(m_geometry, bgColor, radius);

    if (!m_primary) {
        renderer.drawRectOutline(m_geometry, borderColor, borderWidth, radius);
    }

    if (m_label) {
        m_label->setPosition(m_geometry.x + m_padding.left, m_geometry.y + m_padding.top);
        m_label->setSize(m_geometry.width - m_padding.left - m_padding.right,
                          m_geometry.height - m_padding.top - m_padding.bottom);
        m_label->render(renderer);
    }
}

void Button::handleEvent(const Event& event) {
    Widget::handleEvent(event);
}

}
