#pragma once

#include "Button.hpp"

namespace GUIEngine {

class IconButton : public Button {
public:
    IconButton(const std::string& icon = "");
    ~IconButton() override = default;

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;
};

}
