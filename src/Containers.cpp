#include "GUIEngine/Containers.hpp"
#include "GUIEngine/Renderer.hpp"

namespace GUIEngine {

Container::Container() : Widget() {
    m_layout.padding = Padding(0);
}

Vec2 Container::measureContent(float availableWidth, float availableHeight) {
    return Widget::measureContent(availableWidth, availableHeight);
}

void Container::render(Renderer& renderer) {
    for (Widget* child : m_childRefs) {
        if (child->isVisible()) {
            child->render(renderer);
        }
    }
}

HBox::HBox() : Container() {
    m_layout.direction = LayoutDirection::Row;
    m_layout.itemAlignment = Alignment::Center;
}

VBox::VBox() : Container() {
    m_layout.direction = LayoutDirection::Column;
    m_layout.itemAlignment = Alignment::Stretch;
}

StackPanel::StackPanel() : Container() {
    m_layout.direction = LayoutDirection::Column;
    m_layout.spacing = 8;
    m_layout.itemAlignment = Alignment::Stretch;
}

}
