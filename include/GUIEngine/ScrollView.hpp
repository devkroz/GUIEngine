#pragma once

#include "Panel.hpp"
#include "Event.hpp"

namespace GUIEngine {

class ScrollView : public Panel {
public:
    ScrollView();
    ~ScrollView() override = default;

    void setContentSize(float width, float height);
    void setScrollSpeed(float speed) { m_scrollSpeed = speed; }
    void setShowScrollbars(bool show) { m_showScrollbars = show; }
    void setAutoHideScrollbars(bool autoHide) { m_autoHideScrollbars = autoHide; }

    void setStickToBottom(bool stick) { m_stickToBottom = stick; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;
    void update(float deltaTime) override;

private:
    float m_contentWidth = 0;
    float m_contentHeight = 0;
    float m_scrollX = 0;
    float m_scrollY = 0;
    float m_maxScrollX = 0;
    float m_maxScrollY = 0;
    float m_scrollSpeed = 40;
    bool m_showScrollbars = true;
    bool m_autoHideScrollbars = true;
    bool m_stickToBottom = false;
    float m_scrollbarAlpha = 0;
    float m_dragStartScroll = 0;
    float m_dragStartPos = 0;
    bool m_draggingV = false;
    bool m_draggingH = false;
    float m_overscrollY = 0;

    void clampScroll();
    Rect getVerticalScrollbar() const;
    Rect getHorizontalScrollbar() const;
    bool needsVerticalScrollbar() const;
    bool needsHorizontalScrollbar() const;
};

}
