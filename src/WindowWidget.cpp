#include "GUIEngine/WindowWidget.hpp"
#include "GUIEngine/Renderer.hpp"
#include "GUIEngine/Style.hpp"

namespace GUIEngine {

WindowWidget::WindowWidget(const std::string& title) : Panel(), m_title(title) {
    m_layout.fixedWidth = 400;
    m_layout.fixedHeight = 300;
    m_layout.padding = Padding(0);
    m_movable = true;
    m_closable = true;
    m_shadow = true;

    m_headerPanel = add<Panel>();
    m_headerPanel->setBackgroundColor(Color(0.13f, 0.13f, 0.16f, 1.0f));
    m_headerPanel->setBorderRadius(0);
    m_headerPanel->setMargin(Margin(0));
    m_headerPanel->setPadding(Padding(0, 12));
    m_headerPanel->setLayout(LayoutParams());
    m_headerPanel->getLayout().direction = LayoutDirection::Row;
    m_headerPanel->getLayout().itemAlignment = Alignment::Center;
    m_headerPanel->getLayout().justifyContent = Justify::SpaceBetween;
    m_headerPanel->setFixedHeight(m_headerHeight);

    m_titleLabel = m_headerPanel->add<Label>(title);
    m_titleLabel->setFontSize(13);

    m_closeButton = m_headerPanel->add<IconButton>("x");
    m_closeButton->setOnClick([this]() {
        if (m_onClose) m_onClose();
        setVisible(false);
    });

    m_contentPanel = add<Panel>();
    m_contentPanel->setBackgroundColor(Color(0.16f, 0.16f, 0.19f, 1.0f));
    m_contentPanel->setBorderRadius(0);
    m_contentPanel->setMargin(Margin(0));
    m_contentPanel->setPadding(Padding(12));
    m_contentPanel->getLayout().grow = 1;
}

void WindowWidget::setTitle(const std::string& title) {
    m_title = title;
    if (m_titleLabel) m_titleLabel->setText(title);
}

void WindowWidget::updateHeader() {
    if (m_headerPanel) m_headerPanel->setVisible(m_showHeader);
}

Vec2 WindowWidget::measureContent(float availableWidth, float availableHeight) {
    return Widget::measureContent(availableWidth, availableHeight);
}

void WindowWidget::layout(const Rect& bounds) {
    Widget::layout(bounds);

    if (m_headerPanel && m_contentPanel && m_showHeader) {
        Rect headerRect(m_geometry.x, m_geometry.y, m_geometry.width, m_headerHeight);
        m_headerPanel->layout(headerRect);

        Rect contentRect(m_geometry.x, m_geometry.y + m_headerHeight,
                          m_geometry.width, m_geometry.height - m_headerHeight);
        m_contentPanel->layout(contentRect);
    } else if (m_contentPanel) {
        m_contentPanel->layout(m_geometry);
    }
}

void WindowWidget::render(Renderer& renderer) {
    Theme& theme = Theme::defaultTheme();
    Color bgColor = m_style.getColor("panelColor", theme.get().getColor("panelColor", Color(0.16f, 0.16f, 0.19f)));
    Color borderColor = m_style.getColor("borderColor", theme.get().getColor("borderColor", Color(0.3f, 0.3f, 0.35f)));
    float radius = m_style.getFloat("borderRadius", 8.0f);

    if (m_shadow) {
        renderer.drawShadow(m_geometry, 20, Color(0, 0, 0, 0.4f));
    }

    renderer.drawRect(m_geometry, bgColor, radius);

    renderer.pushScissor(m_geometry);

    Rect topRect(m_geometry.x, m_geometry.y, m_geometry.width, m_headerHeight + 4);
    renderer.pushScissor(topRect);

    Rect roundedTop(m_geometry.x, m_geometry.y, m_geometry.width, m_headerHeight);
    if (m_headerPanel && m_showHeader) {
        m_headerPanel->render(renderer);
    }
    renderer.popScissor();

    Rect contentClip(m_geometry.x, m_geometry.y + m_headerHeight,
                      m_geometry.width, m_geometry.height - m_headerHeight);
    renderer.pushScissor(contentClip);
    if (m_contentPanel) {
        m_contentPanel->render(renderer);
    }
    renderer.popScissor();

    renderer.popScissor();

    renderer.drawRectOutline(m_geometry, borderColor, 1.0f, radius);
}

void WindowWidget::handleEvent(const Event& event) {
    Widget::handleEvent(event);

    Rect headerRect(m_geometry.x, m_geometry.y, m_geometry.width, m_headerHeight);

    if (event.type == EventType::MouseButtonPress && event.mouseButton == 1) {
        if (m_movable && headerRect.contains(event.mouseX, event.mouseY)) {
            m_dragging = true;
            m_dragOffsetX = event.mouseX - m_geometry.x;
            m_dragOffsetY = event.mouseY - m_geometry.y;
        }
    }

    if (event.type == EventType::MouseButtonRelease && event.mouseButton == 1) {
        m_dragging = false;
    }

    if (event.type == EventType::MouseMove && m_dragging) {
        m_geometry.x = event.mouseX - m_dragOffsetX;
        m_geometry.y = event.mouseY - m_dragOffsetY;
    }
}

}
