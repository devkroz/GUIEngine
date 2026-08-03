#pragma once

#include "Widget.hpp"
#include <string>

namespace GUIEngine {

class TextField : public Widget {
public:
    TextField(const std::string& text = "");
    ~TextField() override = default;

    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }

    void setPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; }
    void setFontSize(float size) { m_fontSize = size; }
    void setTextColor(const Color& color) { m_style.set("textColor", color); }
    void setBorderColor(const Color& color) { m_style.set("borderColor", color); }
    void setBackgroundColor(const Color& color) { m_style.set("inputColor", color); }
    void setBorderRadius(float radius) { m_style.set("borderRadius", radius); }

    void setPasswordMode(bool password) { m_password = password; }
    void setMaxLength(int max) { m_maxLength = max; }
    void setReadOnly(bool readOnly) { m_readOnly = readOnly; }

    void setOnChangeCallback(std::function<void(const std::string&)> cb) { m_onChange = std::move(cb); }
    void setOnSubmitCallback(std::function<void(const std::string&)> cb) { m_onSubmit = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

protected:
    void renderText(Renderer& renderer, const Color& textColor);
    void renderCursor(Renderer& renderer);

    int charIndexAtPosition(float x);
    float positionAtCharIndex(int index);
    int getCharIndexFromKey(Event& event);

private:
    std::string m_text;
    std::string m_placeholder;
    float m_fontSize = 0;
    bool m_password = false;
    int m_maxLength = -1;
    bool m_readOnly = false;

    int m_cursorIndex = 0;
    int m_selectionStart = -1;
    float m_cursorBlink = 0;
    float m_cursorX = 0;
    float m_textOffset = 0;

    std::function<void(const std::string&)> m_onChange;
    std::function<void(const std::string&)> m_onSubmit;
};

}
