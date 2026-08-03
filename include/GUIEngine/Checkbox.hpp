#pragma once

#include "Widget.hpp"
#include "Label.hpp"

namespace GUIEngine {

class Checkbox : public Widget {
public:
    Checkbox(const std::string& label = "");
    ~Checkbox() override = default;

    void setLabel(const std::string& text);
    const std::string& getLabel() const { return m_label ? m_label->getText() : m_labelText; }

    void setChecked(bool checked) { m_checked = checked; }
    bool isChecked() const { return m_checked; }

    void setOnChangeCallback(std::function<void(bool)> cb) { m_onChange = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

private:
    std::string m_labelText;
    Label* m_label = nullptr;
    bool m_checked = false;
    std::function<void(bool)> m_onChange;
    float m_boxSize = 18;

    void onClick();
};

}
