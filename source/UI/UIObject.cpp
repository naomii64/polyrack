#pragma once
#include "UIObject.h"

UIObject::UIObject(){}
UIObject::~UIObject(){}
void UIObject::draw(Renderer& renderer){
    //the boxes size thing nowwwwwwAAAHH
    Vec4 rectangle = getBoundingRect(renderer);

    Vec2 finalPos=rectangle.xy();
    Vec2 finalWidth=rectangle.zw();

    UIStyle currentStyle;
    if(isHovered){
        currentStyle=style.hovered;
    }else{
        currentStyle=style.default;
    }

    renderer.drawBorderRect(
        finalPos.x,finalPos.y,finalWidth.x,finalWidth.y,
        15.0f,
        1.0f/3.0f,
        EngineAssets::borderTextureID,
        currentStyle.backgroundColor
    );
    
    const float textInsetAmount = 25.0f;
    const float fontSize = 50.0f;

    Vec2 insetScreenAmount={0,0};
    
    //only render the text if there is any to save time
    if(hasText){
        Vec3 textScale=Vec3(fontSize);
        Vec3 textRotation(0.0f,0.0f,0.0f);
        
        Vec2 textPosition = finalPos+Vec2(textInsetAmount)+Vec2(0.0f,fontSize);
        Vec3 textPosv3(textPosition.x,textPosition.y,0.0f);

        renderer.drawModelAt(textModel,textPosv3,textRotation,textScale,EngineAssets::fontTextureID);
    }    
}
Vec4 UIObject::getBoundingRect(Renderer& renderer){
    return Vec4{
        x.toUnits(renderer.getWidth()),
        y.toUnits(renderer.getHeight()),
        width.toUnits(renderer.getWidth()),
        height.toUnits(renderer.getHeight())
    };
}
void UIObject::setText(Renderer& renderer,std::string textString)
{
    textModel.createGeometry(renderer.openGLContext,TextManager::textMesh(textString,1.0f));
    hasText=true;
}