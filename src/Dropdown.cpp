#include "GUIEngine/Dropdown.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

Dropdown::Dropdown() : Widget() {
    m_focusable = true;
    m_layout.minHeight = 36;
    m_layout.minWidth = 120;
    m_style.set("inputColor", Color(0.16f, 0.16f, 0.19f, 1.0f));
    m_style.set("borderColor", Color(0.3f, 0.3f, 0.35f, 1.0f));
}

void Dropdown::setItems(const std::vector<Choice>& items) {
    m_items = items;
    if (m_selectedIndex < 0 || m_selectedIndex >= (int)m_items.size()) {
        if (!m_items.empty()) {
            m_selectedIndex = 0;
            m_selectedValue = m_items[0].value;
        }
    }
}

void Dropdown::addItem(const Choice& item) {
    m_items.push_back(item);
    if (m_selectedIndex < 0) {
        m_selectedIndex = 0;
        m_selectedValue = item.value;
    }
}

void Dropdown::addItem(const std::string& value, const std::string& label) {
    addItem({value, label});
}

void Dropdown::removeItem(const std::string& value) {
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [&](const Choice& c) { return c.value == value; }),
        m_items.end()
    );
    setSelectedIndex(std::min(m_selectedIndex, (int)m_items.size() - 1));
}

void Dropdown::clearItems() {
    m_items.clear();
    m_selectedValue = "";
    m_selectedIndex = -1;
}

void Dropdown::setSelected(const std::string& value) {
    for (int i = 0; i < (int)m_items.size(); ++i) {
        if (m_items[i].value == value) {
            setSelectedIndex(i);
            return;
        }
    }
}

void Dropdown::setSelectedIndex(int index) {
    if (index >= 0 && index < (int)m_items.size()) {
        m_selectedIndex = index;
        m_selectedValue = m_items[index].value;
        if (m_onChange) m_onChange(m_selectedValue);
    } else if (index == -1) {
        m_selectedIndex = -1;
        m_selectedValue = "";
    }
}

int Dropdown::getSelectedIndex() const {
    return m_selectedIndex;
}

Vec2 Dropdown::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    size.x = std::max(size.x, m_layout.minWidth);
    size.y = std::max(size.y, m_layout.minHeight);

    float maxLabelWidth = 0;
    for (const auto& item : m_items) {
        maxLabelWidth = std::max(maxLabelWidth, static_cast<float>(item.label.size() * 8));
    }
    size.x = std::max(size.x, maxLabelWidth + 40);
    m_maxWidth = size.x;
    return size;
}

Rect Dropdown::getPopupRect() const {
    float itemHeight = 32;
    float popupHeight = std::min(static_cast<float>(m_items.size()) * itemHeight, 200.0f);
    return Rect(m_geometry.x, m_geometry.y + m_geometry.height, m_geometry.width, popupHeight);
}

void Dropdown::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("inputColor", theme.get().getColor("inputColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    Color textColor = m_style.getColor("textColor", theme.get().getColor("textColor", Color(0.88f, 0.88f, 0.92f)));
    Color textColorMuted = m_style.getColor("textColorMuted", theme.get().getColor("textColorMuted", Color(0.55f, 0.55f, 0.6f)));
    float radius = m_style.getFloat("borderRadius", 4.0f);

    if (m_state == WidgetState::Hovered || m_open) {
        borderColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
    }

    renderer.drawRect(m_geometry, bgColor, radius);
    renderer.drawRectOutline(m_geometry, borderColor, 1.5f, radius);

    std::string displayText = "Select...";
    if (m_selectedIndex >= 0 && m_selectedIndex < (int)m_items.size()) {
        displayText = m_items[m_selectedIndex].label;
    }

    renderer.drawText(displayText, m_geometry.x + 10, m_geometry.y + (m_geometry.height - 20) * 0.5f,
                       m_selectedIndex >= 0 ? textColor : textColorMuted);

    float arrowY = m_geometry.y + m_geometry.height * 0.5f;
    float arrowX = m_geometry.x + m_geometry.width - 16;
    Color arrowColor = borderColor;
    renderer.drawLine(arrowX - 4, arrowY - 2, arrowX, arrowY + 2, arrowColor, 1.5);
    renderer.drawLine(arrowX, arrowY + 2, arrowX + 4, arrowY - 2, arrowColor, 1.5);

    if (m_open) {
        Rect popupRect = getPopupRect();
        renderer.pushScissor(Rect(popupRect.x, popupRect.y, popupRect.width,
                                    std::min(popupRect.height + 4, 200.0f + 4)));

        renderer.drawShadow(popupRect, 12, Color(0, 0, 0, 0.3f));
        renderer.drawRect(popupRect, bgColor, radius);
        renderer.drawRectOutline(popupRect, borderColor, 1.0f, radius);

        float itemHeight = 32;
        for (int i = 0; i < (int)m_items.size(); ++i) {
            Rect itemRect(popupRect.x, popupRect.y + i * itemHeight, popupRect.width, itemHeight);

            if (i == m_hoveredIndex) {
                renderer.drawRect(itemRect, Color(0.25f, 0.25f, 0.3f, 1.0f));
            }
            if (i == m_selectedIndex) {
                renderer.drawRect(Rect(itemRect.x, itemRect.y + itemHeight * 0.3f, 3, itemHeight * 0.4f),
                                    m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f))));
            }

            renderer.drawText(m_items[i].label, itemRect.x + 12, itemRect.y + (itemHeight - 20) * 0.5f,
                               i == m_selectedIndex ?
                               m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f))) :
                               textColor);
        }

        renderer.popScissor();
    }
}

int Dropdown::getHoveredIndex(float mouseY) const {
    if (!m_open) return -1;
    Rect popup = getPopupRect();
    float itemHeight = 32;
    int index = static_cast<int>((mouseY - popup.y) / itemHeight);
    if (index >= 0 && index < (int)m_items.size()) return index;
    return -1;
}

void Dropdown::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    switch (event.type) {
        case EventType::MouseButtonPress:
            if (event.mouseButton == 1) {
                if (m_open) {
                    Rect popup = getPopupRect();
                    if (popup.contains(event.mouseX, event.mouseY)) {
                        int idx = getHoveredIndex(event.mouseY);
                        if (idx >= 0) {
                            setSelectedIndex(idx);
                            m_open = false;
                            return;
                        }
                    }
                    m_open = false;
                } else if (hitTest(event.mouseX, event.mouseY)) {
                    m_open = true;
                    m_hoveredIndex = m_selectedIndex;
                }
            }
            break;
        case EventType::MouseMove:
            if (m_open) {
                m_hoveredIndex = getHoveredIndex(event.mouseY);
            }
            break;
        case EventType::MouseLeave:
            m_hoveredIndex = -1;
            break;
        default:
            break;
    }
}

RadioButton::RadioButton(const std::string& label) : Widget(), m_labelText(label) {
    m_focusable = true;
    m_layout.minHeight = 28;
    m_layout.minWidth = 100;

    m_innerLabel = add<Label>(label);
    m_innerLabel->setAlign(Label::Alignment::Left);
}

void RadioButton::setLabel(const std::string& text) {
    m_labelText = text;
    if (m_innerLabel) m_innerLabel->setText(text);
}

Vec2 RadioButton::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);

    float labelWidth = 0;
    if (m_innerLabel && m_innerLabel->isVisible()) {
        Vec2 labelSize = m_innerLabel->measure(availableWidth, availableHeight);
        labelWidth = labelSize.x;
    }

    size.x = std::max(size.x, m_circleSize + 8 + labelWidth);
    size.y = std::max(size.y, static_cast<float>(m_circleSize));
    return size;
}

void RadioButton::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("inputColor", theme.get().getColor("inputColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    Color selectColor = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));

    if (m_state == WidgetState::Hovered) {
        borderColor = Color(borderColor.r + 0.1f, borderColor.g + 0.1f, borderColor.b + 0.1f, borderColor.a);
    }

    float circleY = m_geometry.y + (m_geometry.height - m_circleSize) * 0.5f;
    float circleX = m_geometry.x + m_circleSize * 0.5f;

    renderer.drawRectOutline(Rect(circleX - m_circleSize * 0.5f, circleY, m_circleSize, m_circleSize),
                              borderColor, 1.5f, m_circleSize * 0.5f);

    if (m_selected) {
        float innerSize = m_circleSize * 0.5f;
        renderer.drawRect(
            Rect(circleX - innerSize * 0.5f, circleY + (m_circleSize - innerSize) * 0.5f, innerSize, innerSize),
            selectColor, innerSize * 0.5f);
    }

    if (m_innerLabel) {
        m_innerLabel->setPosition(m_geometry.x + m_circleSize + 8, m_geometry.y);
        m_innerLabel->setSize(m_geometry.width - m_circleSize - 8, m_geometry.height);
        m_innerLabel->render(renderer);
    }
}

void RadioButton::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    if (event.type == EventType::MouseButtonPress && event.mouseButton == 1) {
        m_pressed = true;
    }

    if (event.type == EventType::MouseButtonRelease && event.mouseButton == 1) {
        if (m_pressed) {
            m_pressed = false;
            if (hitTest(event.mouseX, event.mouseY) && !m_selected) {
                m_selected = true;
                if (m_onChange) m_onChange(true);
            }
        }
    }
}

}
