#include "GUIEngine/TextField.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include <cmath>

namespace GUIEngine {

TextField::TextField(const std::string& text) : Widget(), m_text(text) {
    m_focusable = true;
    m_layout.minHeight = 36;
    m_layout.minWidth = 120;
    m_cursorIndex = static_cast<int>(text.size());
    m_style.set("inputColor", Color(0.16f, 0.16f, 0.19f, 1.0f));
    m_style.set("borderColor", Color(0.3f, 0.3f, 0.35f, 1.0f));
}

void TextField::setText(const std::string& text) {
    m_text = text;
    if (m_cursorIndex > static_cast<int>(m_text.size())) {
        m_cursorIndex = static_cast<int>(m_text.size());
    }
    if (m_onChange) m_onChange(m_text);
}

Vec2 TextField::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);
    size.x = std::max(size.x, m_layout.minWidth);
    size.y = std::max(size.y, m_layout.minHeight);
    return size;
}

void TextField::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("inputColor", theme.get().getColor("inputColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    Color textColor = m_style.getColor("textColor", theme.get().getColor("textColor", Color(0.88f, 0.88f, 0.92f)));
    Color textColorMuted = m_style.getColor("textColorMuted", theme.get().getColor("textColorMuted", Color(0.55f, 0.55f, 0.6f)));
    float radius = m_style.getFloat("borderRadius", theme.get().getFloat("borderRadiusSmall", 3.0f));

    if (m_focused) {
        Color focusBorder = m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f)));
        borderColor = focusBorder;
    }

    renderer.drawRect(m_geometry, bgColor, radius);
    renderer.drawRectOutline(m_geometry, borderColor, 1.5f, radius);

    renderText(renderer, textColor);

    if (m_focused) {
        renderCursor(renderer);
    }
}

void TextField::renderText(Renderer& renderer, const Color& textColor) {
    float textY = m_geometry.y + (m_geometry.height - 20) * 0.5f;
    float textX = m_geometry.x + 10 + m_textOffset;

    if (m_text.empty() && !m_placeholder.empty()) {
        renderer.drawText(m_placeholder, m_geometry.x + 10, textY,
                          Color(0.55f, 0.55f, 0.6f, 1.0f));
        return;
    }

    std::string displayText = m_text;
    if (m_password) {
        displayText = std::string(m_text.size(), '*');
    }

    renderer.drawText(displayText, textX, textY, textColor);
}

void TextField::renderCursor(Renderer& renderer) {
    m_cursorBlink += 0.016f;
    if (m_cursorBlink < 0.5f) return;
    if (m_cursorBlink > 1.0f) m_cursorBlink = 0;

    float textY = m_geometry.y + (m_geometry.height - 20) * 0.5f;
    float cursorX = m_geometry.x + 10 + m_textOffset;

    std::string displayText = m_text;
    if (m_password) {
        displayText = std::string(m_text.size(), '*');
    }

    std::string beforeCursor = displayText.substr(0, m_cursorIndex);
    float textWidth = beforeCursor.size() * 8.0f;
    cursorX += textWidth;

    renderer.drawRect(Rect(cursorX, textY, 1.5f, 20), Color(1.0f, 1.0f, 1.0f, 0.9f));
}

void TextField::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    switch (event.type) {
        case EventType::MouseButtonPress:
            if (event.mouseButton == 1) {
                focus();
                m_cursorBlink = 0;
            }
            break;
        case EventType::TextInput:
            if (!m_readOnly && !event.text.empty()) {
                for (char c : event.text) {
                    if (m_maxLength < 0 || static_cast<int>(m_text.size()) < m_maxLength) {
                        m_text.insert(m_text.begin() + m_cursorIndex, c);
                        m_cursorIndex++;
                    }
                }
                if (m_onChange) m_onChange(m_text);
            }
            break;
        case EventType::KeyPress:
            if (event.key == 8 || event.key == 127) {
                if (event.key == 8 && m_cursorIndex > 0) {
                    m_text.erase(m_text.begin() + m_cursorIndex - 1);
                    m_cursorIndex--;
                } else if (event.key == 127 && m_cursorIndex < static_cast<int>(m_text.size())) {
                    m_text.erase(m_text.begin() + m_cursorIndex);
                }
                if (m_onChange) m_onChange(m_text);
            } else if (event.key == 13) {
                if (m_onSubmit) m_onSubmit(m_text);
            } else if (event.key == 27) {
                unfocus();
            } else if (event.key == 1073741903) {
                if (m_cursorIndex < static_cast<int>(m_text.size())) m_cursorIndex++;
            } else if (event.key == 1073741904) {
                if (m_cursorIndex > 0) m_cursorIndex--;
            } else if (event.key == 1073741901) {
                m_cursorIndex = static_cast<int>(m_text.size());
            } else if (event.key == 1073741898) {
                m_cursorIndex = 0;
            }
            m_cursorBlink = 0;
            break;
        default:
            break;
    }
}

}
