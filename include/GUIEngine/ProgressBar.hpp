#pragma once

#include "Widget.hpp"

namespace GUIEngine {

class ProgressBar : public Widget {
public:
    ProgressBar();
    ~ProgressBar() override = default;

    void setProgress(float percent);
    float getProgress() const { return m_progress; }

    void setIndeterminate(bool indeterminate) { m_indeterminate = indeterminate; }
    bool isIndeterminate() const { return m_indeterminate; }

    void setColor(const Color& color) { m_style.set("primaryColor", color); }
    void setTrackColor(const Color& color) { m_style.set("buttonColor", color); }
    void setBorderRadius(float radius) { m_style.set("borderRadius", radius); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void update(float deltaTime) override;

private:
    float m_progress = 0;
    bool m_indeterminate = false;
    float m_animOffset = 0;
};

}
