#include "GUIEngine/TabView.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"
#include "GUIEngine/Label.hpp"
#include <algorithm>
#include <cmath>

namespace GUIEngine {

TabView::TabView() : Widget() {
    m_layout.padding = Padding(0);
    m_focusable = false;
}

Tab* TabView::addTab(const std::string& id, const std::string& title, bool closable) {
    auto tab = std::make_unique<Tab>();
    tab->id = id;
    tab->title = title;
    tab->closable = closable;
    Tab* ptr = tab.get();
    m_tabs.push_back(std::move(tab));
    if (m_activeIndex < 0) m_activeIndex = 0;
    return ptr;
}

void TabView::removeTab(const std::string& id) {
    int idx = -1;
    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        if (m_tabs[i]->id == id) { idx = i; break; }
    }
    if (idx < 0) return;
    m_tabs.erase(m_tabs.begin() + idx);
    if (m_activeIndex >= (int)m_tabs.size()) m_activeIndex = (int)m_tabs.size() - 1;
    if (m_activeIndex < 0) m_activeIndex = 0;
}

void TabView::selectTab(const std::string& id) {
    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        if (m_tabs[i]->id == id) {
            if (i != m_activeIndex) {
                m_activeIndex = i;
                if (m_onTabChange) m_onTabChange(id);
            }
            return;
        }
    }
}

Tab* TabView::getTab(const std::string& id) {
    for (auto& tab : m_tabs) {
        if (tab->id == id) return tab.get();
    }
    return nullptr;
}

void TabView::closeTab(const std::string& id) {
    if (m_onTabClose) m_onTabClose(id);
    removeTab(id);
}

Rect TabView::getTabRect(int index) const {
    float tabWidth = 0;
    int visibleTabs = 0;
    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        visibleTabs++;
    }
    tabWidth = m_geometry.width / std::max(1, visibleTabs);

    if (m_tabPosition == LayoutDirection::Row) {
        return Rect(m_geometry.x + index * tabWidth, m_geometry.y, tabWidth, m_tabHeight);
    } else {
        return Rect(m_geometry.x, m_geometry.y + index * m_tabHeight, 120, m_tabHeight);
    }
}

Rect TabView::getContentRect() const {
    if (m_tabPosition == LayoutDirection::Row) {
        return Rect(m_geometry.x, m_geometry.y + m_tabHeight, m_geometry.width, m_geometry.height - m_tabHeight);
    } else {
        return Rect(m_geometry.x + 120, m_geometry.y, m_geometry.width - 120, m_geometry.height);
    }
}

Rect TabView::getCloseBtnRect(int index) const {
    Rect tabRect = getTabRect(index);
    return Rect(tabRect.x + tabRect.width - 20, tabRect.y + (tabRect.height - 16) * 0.5f, 16, 16);
}

int TabView::getTabAt(float x, float y) const {
    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        if (getTabRect(i).contains(x, y)) return i;
    }
    return -1;
}

int TabView::getCloseBtnAt(float x, float y) const {
    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        if (!m_tabs[i]->closable) continue;
        if (getCloseBtnRect(i).contains(x, y)) return i;
    }
    return -1;
}

Vec2 TabView::measureContent(float availableWidth, float availableHeight) {
    return Vec2(std::max(availableWidth, 200.0f), std::max(availableHeight, 150.0f));
}

void TabView::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("backgroundColor", Color(0.14f, 0.14f, 0.17f, 1.0f));
    Color tabColor = m_style.getColor("buttonColor", theme.get().getColor("buttonColor", Color(0.22f, 0.22f, 0.26f)));
    Color activeColor = m_style.getColor("panelColor", theme.get().getColor("panelColor", Color(0.16f, 0.16f, 0.19f)));
    Color textColor = m_style.getColor("textColor", theme.get().getColor("textColor", Color(0.88f, 0.88f, 0.92f)));
    Color textColorMuted = m_style.getColor("textColorMuted", theme.get().getColor("textColorMuted", Color(0.55f, 0.55f, 0.6f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));

    renderer.drawRect(Rect(m_geometry.x, m_geometry.y, 120, m_geometry.height), bgColor);

    for (int i = 0; i < (int)m_tabs.size(); ++i) {
        Rect tabRect = getTabRect(i);
        bool isActive = (i == m_activeIndex);
        bool isHovered = (i == m_hoveredTab);

        Color tabBg = isActive ? activeColor : (isHovered ? Color(tabColor.r + 0.03f, tabColor.g + 0.03f, tabColor.b + 0.03f, 1.0f) : tabColor);
        renderer.drawRect(tabRect, tabBg);

        if (isActive) {
            renderer.drawRect(Rect(tabRect.x, tabRect.y + tabRect.height - 2, tabRect.width, 2),
                                m_style.getColor("primaryColor", theme.get().getColor("primaryColor", Color(0.2f, 0.5f, 0.95f))));
        }

        Color tc = isActive ? textColor : textColorMuted;
        renderer.drawText(m_tabs[i]->title, tabRect.x + 12, tabRect.y + (tabRect.height - 20) * 0.5f, tc);

        if (m_tabs[i]->closable) {
            Rect closeRect = getCloseBtnRect(i);
            Color closeColor = (i == m_hoveredCloseBtn) ? Color(0.85f, 0.2f, 0.2f, 1.0f) : textColorMuted;
            float cx = closeRect.x + closeRect.width * 0.5f;
            float cy = closeRect.y + closeRect.height * 0.5f;
            renderer.drawLine(cx - 4, cy - 4, cx + 4, cy + 4, closeColor, 1.5);
            renderer.drawLine(cx - 4, cy + 4, cx + 4, cy - 4, closeColor, 1.5);
        }
    }

    if (!m_tabs.empty() && m_activeIndex >= 0 && m_activeIndex < (int)m_tabs.size()) {
        Rect contentRect = getContentRect();
        renderer.pushScissor(contentRect);
        renderer.drawRect(contentRect, activeColor);

        if (m_tabs[m_activeIndex]->content) {
            Vec2 origPos = m_tabs[m_activeIndex]->content->getPosition();
            m_tabs[m_activeIndex]->content->setPosition(contentRect.x, contentRect.y);
            m_tabs[m_activeIndex]->content->setSize(contentRect.width, contentRect.height);
            m_tabs[m_activeIndex]->content->render(renderer);
            m_tabs[m_activeIndex]->content->setPosition(origPos.x, origPos.y);
        }
        renderer.popScissor();
    }
}

void TabView::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    switch (event.type) {
        case EventType::MouseButtonPress:
            if (event.mouseButton == 1) {
                int closeIdx = getCloseBtnAt(event.mouseX, event.mouseY);
                if (closeIdx >= 0) {
                    closeTab(m_tabs[closeIdx]->id);
                    break;
                }
                int tabIdx = getTabAt(event.mouseX, event.mouseY);
                if (tabIdx >= 0 && tabIdx != m_activeIndex) {
                    m_activeIndex = tabIdx;
                    if (m_onTabChange) m_onTabChange(m_tabs[tabIdx]->id);
                }
            }
            break;
        case EventType::MouseMove: {
            m_hoveredTab = getTabAt(event.mouseX, event.mouseY);
            m_hoveredCloseBtn = getCloseBtnAt(event.mouseX, event.mouseY);
            break;
        }
        case EventType::MouseLeave:
            m_hoveredTab = -1;
            m_hoveredCloseBtn = -1;
            break;
        default:
            break;
    }
}

void TabView::layout(const Rect& bounds) {
    Widget::layout(bounds);

    Rect contentRect = getContentRect();
    if (m_activeIndex >= 0 && m_activeIndex < (int)m_tabs.size() && m_tabs[m_activeIndex]->content) {
        m_tabs[m_activeIndex]->content->layout(contentRect);
    }
}

}
