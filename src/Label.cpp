#include "GUIEngine/Label.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

Label::Label(const std::string& text) : Widget(), m_text(text) {
    m_focusable = false;
    m_style.set("textColor", Color(0.88f, 0.88f, 0.92f, 1.0f));
    m_style.set("fontSize", 14);
    m_layout.minHeight = 20;
}

void Label::setText(const std::string& text) {
    m_text = text;
}

void Label::setFontSize(float size) {
    m_fontSize = size;
}

Vec2 Label::measureContent(float availableWidth, float availableHeight) {
    Vec2 size = Widget::measureContent(availableWidth, availableHeight);

    if (m_text.empty()) return size;

    float fontSize = m_fontSize > 0 ? m_fontSize : 14.0f;

    float textWidth = m_wrap ? availableWidth : getTextWidth(m_text);
    float lineHeight = fontSize * 1.25f;

    if (m_multiline || m_wrap) {
        int numLines = 1;
        float lineWidth = 0;
        for (size_t i = 0; i < m_text.size(); ++i) {
            if (m_text[i] == '\n') {
                numLines++;
                lineWidth = 0;
            } else {
                lineWidth++;
                if (m_wrap && lineWidth > textWidth / (fontSize * 0.5f)) {
                    numLines++;
                    lineWidth = 0;
                }
            }
        }
        size.x = std::max(size.x, textWidth);
        size.y = std::max(size.y, static_cast<float>(numLines) * lineHeight);
    } else {
        size.x = std::max(size.x, textWidth);
        size.y = std::max(size.y, lineHeight);
    }

    return size;
}

float Label::getTextWidth(const std::string& text) {
    return text.size() * 8.0f;
}

void Label::render(Renderer& renderer) {
    if (m_text.empty()) return;

    Theme& theme = Theme::defaultTheme();
    Color textColor = m_style.getColor("textColor", theme.get().getColor("textColor", Color(0.88f, 0.88f, 0.92f)));
    float fontSize = m_fontSize > 0 ? m_fontSize : theme.get().getFloat("fontSize", 14.0f);

    if (m_align == Alignment::Center) {
        float textWidth = getTextWidth(m_text);
        renderer.drawText(m_text, m_geometry.x + (m_geometry.width - textWidth) * 0.5f,
                          m_geometry.y + (m_geometry.height - fontSize) * 0.5f, textColor);
    } else if (m_align == Alignment::Right) {
        float textWidth = getTextWidth(m_text);
        renderer.drawText(m_text, m_geometry.x + m_geometry.width - textWidth,
                          m_geometry.y + (m_geometry.height - fontSize) * 0.5f, textColor);
    } else {
        renderer.drawText(m_text, m_geometry.x,
                          m_geometry.y + (m_geometry.height - fontSize) * 0.5f, textColor);
    }
}

}
