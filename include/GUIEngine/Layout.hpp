#pragma once

#include "Types.hpp"

namespace GUIEngine {

enum class LayoutDirection { Row, Column };
enum class Alignment { Start, Center, End, Stretch };
enum class Justify { Start, Center, End, SpaceBetween, SpaceEvenly, SpaceAround };
enum class PositionType { Relative, Absolute };

struct Margin {
    float top = 0, right = 0, bottom = 0, left = 0;
    Margin() = default;
    Margin(float all) : top(all), right(all), bottom(all), left(all) {}
    Margin(float v, float h) : top(v), right(h), bottom(v), left(h) {}
    Margin(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}
};

struct Padding {
    float top = 0, right = 0, bottom = 0, left = 0;
    Padding() = default;
    Padding(float all) : top(all), right(all), bottom(all), left(all) {}
    Padding(float v, float h) : top(v), right(h), bottom(v), left(h) {}
    Padding(float t, float r, float b, float l) : top(t), right(r), bottom(b), left(l) {}
};

struct LayoutParams {
    LayoutDirection direction = LayoutDirection::Column;
    Alignment itemAlignment = Alignment::Stretch;
    Justify justifyContent = Justify::Start;
    Justify alignItems = Justify::Start;
    float spacing = 0;
    float grow = 0;
    float shrink = 1;
    float fixedWidth = -1;
    float fixedHeight = -1;
    float minWidth = 0;
    float minHeight = 0;
    float maxWidth = 0;
    float maxHeight = 0;
    PositionType position = PositionType::Relative;
    Margin margin;
    Padding padding;
};

}
