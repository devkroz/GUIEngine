#pragma once

#include "Widget.hpp"

namespace GUIEngine {

class Container : public Widget {
public:
    Container();
    ~Container() override = default;

    bool isContainer() const override { return true; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
};

class HBox : public Container {
public:
    HBox();
    ~HBox() override = default;
};

class VBox : public Container {
public:
    VBox();
    ~VBox() override = default;
};

class StackPanel : public Container {
public:
    StackPanel();
    ~StackPanel() override = default;

    void setDirection(LayoutDirection dir) { m_layout.direction = dir; }
    void setSpacing(float spacing) { m_layout.spacing = spacing; }
    void setAlignment(Alignment align) { m_layout.itemAlignment = align; }
    void setJustifyContent(Justify justify) { m_layout.justifyContent = justify; }
};

}
