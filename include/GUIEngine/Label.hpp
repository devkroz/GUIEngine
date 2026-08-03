#pragma once

#include "Widget.hpp"
#include "Style.hpp"

namespace GUIEngine {

class Label : public Widget {
public:
    Label(const std::string& text = "");
    ~Label() override = default;

    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }

    void setFontSize(float size);
    float getFontSize() const { return m_fontSize; }

    void setTextColor(const Color& color) { m_style.set("textColor", color); }
    void setFontFamily(const std::string& family) { m_fontFamily = family; }

    enum class Alignment { Left, Center, Right };
    void setAlign(Alignment align) { m_align = align; }

    void setWrap(bool wrap) { m_wrap = wrap; }
    void setMultiline(bool multiline) { m_multiline = multiline; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;

private:
    std::string m_text;
    float m_fontSize = 0;
    std::string m_fontFamily;
    Alignment m_align = Alignment::Left;
    bool m_wrap = false;
    bool m_multiline = false;
};

}
