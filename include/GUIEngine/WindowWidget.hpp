#pragma once

#include "Panel.hpp"
#include "Label.hpp"
#include "Button.hpp"

namespace GUIEngine {

class WindowWidget : public Panel {
public:
    WindowWidget(const std::string& title = "");
    ~WindowWidget() override = default;

    void setTitle(const std::string& title);
    const std::string& getTitle() const { return m_titleLabel ? m_titleLabel->getText() : m_title; }

    void setClosable(bool closable) { m_closable = closable; if (m_closeButton) m_closeButton->setVisible(closable); }
    bool isClosable() const { return m_closable; }

    void setMovable(bool movable) { m_movable = movable; }
    bool isMovable() const { return m_movable; }

    void setResizable(bool resizable) { m_resizable = resizable; }
    bool isResizable() const { return m_resizable; }

    void setShowHeader(bool show) { m_showHeader = show; updateHeader(); }
    bool getShowHeader() const { return m_showHeader; }

    Panel* getContentPanel() { return m_contentPanel; }

    void setOnClose(std::function<void()> cb) { m_onClose = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;
    void layout(const Rect& bounds) override;

private:
    std::string m_title;
    Label* m_titleLabel = nullptr;
    Button* m_closeButton = nullptr;
    Panel* m_headerPanel = nullptr;
    Panel* m_contentPanel = nullptr;

    bool m_closable = true;
    bool m_movable = true;
    bool m_resizable = false;
    bool m_showHeader = true;
    bool m_dragging = false;
    float m_dragOffsetX = 0;
    float m_dragOffsetY = 0;
    float m_headerHeight = 36;
    bool m_resizing = false;
    float m_resizeStartW = 0;
    float m_resizeStartH = 0;
    float m_resizeStartX = 0;
    float m_resizeStartY = 0;

    std::function<void()> m_onClose;

    void updateHeader();
};

}
