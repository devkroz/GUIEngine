#pragma once

#include "Widget.hpp"
#include "Label.hpp"

namespace GUIEngine {

class Button : public Widget {
public:
    Button(const std::string& text = "");
    ~Button() override = default;

    void setText(const std::string& text);
    const std::string& getText() const { return m_label ? m_label->getText() : m_text; }

    void setFontSize(float size) { m_fontSize = size; if (m_label) m_label->setFontSize(size); }

    void setBackgroundColor(const Color& color) { m_style.set("buttonColor", color); }
    void setTextColor(const Color& color) { m_style.set("textColor", color); }
    void setBorderRadius(float radius) { m_style.set("borderRadius", radius); }

    void setPrimary(bool primary);
    bool isPrimary() const { return m_primary; }

    void setIcon(const std::string& icon) { m_icon = icon; }
    void setPadding(const Padding& padding) { m_padding = padding; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

protected:
    virtual Color resolveColor(const std::string& key1, const std::string& key2, const Color& def);

protected:
    std::string m_text;
    Label* m_label = nullptr;
    bool m_primary = false;
    float m_fontSize = 0;
    std::string m_icon;
    Padding m_padding = Padding(8, 16);

private:
};

}
