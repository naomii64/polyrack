#pragma once
#include <string>
#include <vector>
#include "Includes/Includes.h"

namespace TextManager{
    enum Justification{
        LEFT,
        CENTER,
        RIGHT
    };

    extern std::vector<Vertex> textMesh(std::string& string,float fontSize=0.1f,Justification justify=Justification::LEFT);
}