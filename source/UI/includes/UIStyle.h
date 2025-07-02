#pragma once
#include "../../Engine/Includes/DataTypes.h"

struct UIStyle{
    Vec4 backgroundColor=Vec4(1.0f,1.0f,1.0f,1.0f);
};
struct UIObjectStyle{
    UIStyle default;
    UIStyle hovered;
};