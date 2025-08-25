#pragma once
#include <string>
#include <vector>
#include "Includes/DataTypes.h"
#include "Includes/Model.h"
#include "Includes/Vertex.h"
#include "Includes/ParseOBJ.h"

namespace TextManager{
    enum Justification{
        LEFT,
        CENTER,
        RIGHT
    };

    extern std::vector<Vertex> textMesh(std::string& string,float fontSize=0.1f,Justification justify=Justification::LEFT);
}