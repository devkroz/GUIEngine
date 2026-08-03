#pragma once

#include "Widget.hpp"

namespace GUIEngine {

class Slider : public Widget {
public:
    Slider();
    ~Slider() override = default;

    void setRange(float min, float max);
    void setValue(float value);
    float getValue() const { return m_value; }

    void setStep(float step) { m_step = step; }
    void setOrientation(LayoutDirection dir) { m_orientation = dir; }

    void setOnChangeCallback(std::function<void(float)> cb) { m_onChange = std::move(cb); }
    void setOnReleaseCallback(std::function<void(float)> cb) { m_onRelease = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

private:
    float m_min = 0;
    float m_max = 100;
    float m_value = 50;
    float m_step = 0;
    LayoutDirection m_orientation = LayoutDirection::Row;
    bool m_dragging = false;

    std::function<void(float)> m_onChange;
    std::function<void(float)> m_onRelease;

    void updateValueFromPosition(float mouseX, float mouseY);
    float getTrackLength() const;
    float getThumbSize() const;
};

}
