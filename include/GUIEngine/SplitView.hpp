#pragma once

#include "Panel.hpp"
#include <functional>

namespace GUIEngine {

class SplitView : public Panel {
public:
    SplitView();
    ~SplitView() override = default;

    void setAutomatic(bool automatic) { m_automatic = automatic; }
    void setOrientation(LayoutDirection dir) { m_orientation = dir; }
    void setSplit(float percentage);
    float getSplit() const { return m_split; }

    void setMinSplit(float minSplit) { m_minSplit = minSplit; }
    void setMaxSplit(float maxSplit) { m_maxSplit = maxSplit; }
    void setDividerSize(float size) { m_dividerSize = size; }

    void setOnSplitChange(std::function<void(float)> cb) { m_onSplitChange = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;

private:
    LayoutDirection m_orientation = LayoutDirection::Row;
    float m_split = 0.5f;
    float m_minSplit = 0.1f;
    float m_maxSplit = 0.9f;
    float m_dividerSize = 6;
    bool m_automatic = false;
    bool m_dragging = false;

    std::function<void(float)> m_onSplitChange;

    Rect getDividerRect() const;
    Rect getFirstRect() const;
    Rect getSecondRect() const;
};

}
