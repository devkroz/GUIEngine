#pragma once

#include "Panel.hpp"
#include "Label.hpp"
#include <functional>

namespace GUIEngine {

class Separator : public Widget {
public:
    enum class Orientation { Horizontal, Vertical };

    Separator(Orientation dir = Orientation::Horizontal);
    ~Separator() override = default;

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;

private:
    Orientation m_orientation;
};

class Spacer : public Widget {
public:
    Spacer();
    Spacer(float fixedWidth, float fixedHeight);
    ~Spacer() override = default;

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& ) override {}

private:
    float m_fixedWidth = 0;
    float m_fixedHeight = 0;
};

class Image : public Widget {
public:
    Image(const std::string& src = "");
    ~Image() override = default;

    void setSource(const std::string& src);
    const std::string& getSource() const { return m_src; }

    void setTint(const Color& tint) { m_tint = tint; }
    void setFit(bool fit) { m_fit = fit; }

    Vec2 measureContent(float availableWidth, float availableHeight) override;
    void render(Renderer& renderer) override;

private:
    std::string m_src;
    Color m_tint = Color::White();
    bool m_fit = true;
    bool m_loaded = false;
};

}
