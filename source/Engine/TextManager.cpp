#pragma once
#include "TextManager.h"

namespace TextManager{
    //5 eigths
    constexpr float defaultWidth = 5.0f / 8.0f;
    constexpr std::array<float, 96> generateCharWidths() {
        std::array<float, 96> widths = {};

        for (int i = 0; i < 96; ++i) {
            widths[i] = defaultWidth;
        }

        //constants
        const float fract_3_8 = 3.0f / 8.0f;
        const float fract_7_8 = 7.0f / 8.0f;
        
        widths['|' - 32] = 2.0f / 8.0f;

        widths['!' - 32] =  fract_3_8;
        widths['\'' - 32] = fract_3_8;
        widths[',' - 32] =  fract_3_8;
        widths['.' - 32] =  fract_3_8;
        widths[':' - 32] =  fract_3_8;
        widths[';' - 32] =  fract_3_8;
        widths['`' - 32] =  fract_3_8;
        widths['i' - 32] =  fract_3_8;
        widths['l' - 32] =  0.5f;

        widths['"' - 32] = 4.0f / 8.0f;
        widths['#' - 32] = 6.0f / 8.0f;
        
        widths['m' - 32] = fract_7_8;
        widths['M' - 32] = fract_7_8;
        widths['w' - 32] = fract_7_8;
        widths['W' - 32] = fract_7_8;

        widths[' ' - 32] = 0.5f;
        
        return widths;
    }
    constexpr std::array<float, 96> charWidths = generateCharWidths();

    //justification enumerators
    std::vector<Vertex> textMesh(std::string& text,float fontSize,Justification justify) {
        std::vector<Vertex> mesh;

        //constants
        const int glyphsPerRow = 16;
        const int rows = 6;
        const int asciiStart = 32;
        //uv width and height for 1 character (doesnt account for unique char shapes)
        const float uvHeight=1.0f/float(rows);
        const float uvWidth=1.0f/float(glyphsPerRow);


        //vars
        float currentX = 0.0f;
        float finalTextWidth=0.0f;
        
        //calculate how wide the final text will be
        //doesnt need to do this for justify left
        if(justify!=Justification::LEFT){
            
            for (char c : text) {
                    
                    int ascii = static_cast<int>(c);
                    //skip invalid (non typable) characters
                    if (ascii < 32 || ascii >= 128) continue;
                    
                    //calculate the UV x and y
                    int glyphIndex = ascii - asciiStart;
                    float charWidthPercentage=charWidths[glyphIndex];
                    float currentCharWidth=charWidthPercentage*fontSize;

                    finalTextWidth+=currentCharWidth;
            }
        }


        float justifyOffset=0.0f;
        switch(justify){
            case Justification::CENTER:
            justifyOffset=finalTextWidth*0.5f;
            break;
            case Justification::RIGHT:
            justifyOffset=finalTextWidth;
            break;
        }

        for (char c : text) {
                int ascii = static_cast<int>(c);

                //skip invalid (non typable) characters
                if (ascii < 32 || ascii >= 128) continue;
                
                //calculate the UV x and y
                int glyphIndex = ascii - asciiStart;
                float charWidthPercentage=charWidths[glyphIndex];
                float currentCharWidth=charWidthPercentage*fontSize;

                int col = glyphIndex % glyphsPerRow;
                int row = 5-(glyphIndex / glyphsPerRow);


                const Vec2 posA={currentX-justifyOffset,0.0f};
                const Vec2 posB={currentX+currentCharWidth-justifyOffset,fontSize};
                
                const Vec2 uvA={col*uvWidth,row*uvHeight};
                const Vec2 uvB={uvA.x+(uvWidth*charWidthPercentage),uvA.y+uvHeight};
                
                //normal is all 1s for now
                #define NORMAL {1.0f,1.0f,1.0f}
                #define COLOR {1.0f,1.0f,1.0f,1.0f}

                //define verts
                Vertex vertA = {
                    {posA.x,posA.y,0.0f},
                    NORMAL,
                    COLOR,
                    {uvA.x,uvA.y},
                };
                Vertex vertB = {
                    {posB.x,posA.y,0.0f},
                    NORMAL,
                    COLOR,
                    {uvB.x,uvA.y},
                };
                Vertex vertC = {
                    {posB.x,posB.y,0.0f},
                    NORMAL,
                    COLOR,
                    {uvB.x,uvB.y},
                };
                Vertex vertE = {
                    {posA.x,posB.y,0.0f},
                    NORMAL,
                    COLOR,
                    {uvA.x,uvB.y},
                };

                #undef NORMAL
                #undef COLOR
                
                //tri 1
                mesh.emplace_back(vertA);
                mesh.emplace_back(vertB);
                mesh.emplace_back(vertC);
                //tri 2
                mesh.emplace_back(vertA);
                mesh.emplace_back(vertC);
                mesh.emplace_back(vertE);
                

                currentX+=currentCharWidth;
        }

        return mesh;
    }
}