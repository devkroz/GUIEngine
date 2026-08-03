#pragma once

#include "Panel.hpp"
#include "Label.hpp"
#include <vector>

namespace GUIEngine {

struct Tab {
    std::string id;
    std::string title;
    std::unique_ptr<Widget> content;
    bool closable = false;
};

class TabView : public Widget {
public:
    TabView();
    ~TabView() override = default;

    Tab* addTab(const std::string& id, const std::string& title, bool closable = false);
    void removeTab(const std::string& id);
    void selectTab(const std::string& id);
    Tab* getTab(const std::string& id);
    void closeTab(const std::string& id);

    int getTabCount() const { return static_cast<int>(m_tabs.size()); }
    int getActiveIndex() const { return m_activeIndex; }
    void setTabPosition(LayoutDirection pos) { m_tabPosition = pos; }

    void setOnTabChange(std::function<void(const std::string&)> cb) { m_onTabChange = std::move(cb); }
    void setOnTabClose(std::function<void(const std::string&)> cb) { m_onTabClose = std::move(cb); }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
    void handleEvent(const Event& event) override;
    void layout(const Rect& bounds) override;

    bool isContainer() const override { return true; }

private:
    std::vector<std::unique_ptr<Tab>> m_tabs;
    int m_activeIndex = 0;
    float m_tabHeight = 32;
    LayoutDirection m_tabPosition = LayoutDirection::Row;
    int m_hoveredTab = -1;
    int m_hoveredCloseBtn = -1;
    float m_scrollOffset = 0;

    std::function<void(const std::string&)> m_onTabChange;
    std::function<void(const std::string&)> m_onTabClose;

    Rect getTabRect(int index) const;
    Rect getContentRect() const;
    Rect getCloseBtnRect(int index) const;
    int getTabAt(float x, float y) const;
    int getCloseBtnAt(float x, float y) const;
};

}
