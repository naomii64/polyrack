#version 330 core
            
in vec4 vColour;
in vec2 vTexCoord;    
in vec4 vWorldNormal;

//uniforms
uniform vec4 uTextureRects[64];
uniform sampler2D myTexture;
uniform int uTextureID;
            
uniform vec4 uTintColor=vec4(1.0,1.0,1.0,1.0);
            
layout(location = 0) out vec4 FragColor;

vec2 cropUVSToTexture(vec2 inputUV, int textureID)
{
    vec4 rect = uTextureRects[textureID]; // x, y, width, height
    vec2 wrappedUV = fract(inputUV);      // ensures 0.0 ≤ UV < 1.0
    return rect.xy + wrappedUV * rect.zw;
}

void main()
{
    vec4 mappedNormal = (vWorldNormal+vec4(1.0))/vec4(2.0);
                
    //final colors that do get combined
    vec4 texColor = texture(myTexture,  cropUVSToTexture(vTexCoord,uTextureID));
    vec4 shadowColor = vec4(0.0);
    shadowColor+=mappedNormal.y*3.0;
    shadowColor+=mappedNormal.x;
    shadowColor+=mappedNormal.z*2.0;
    shadowColor/=4.0;
    shadowColor*=shadowColor;

    shadowColor.a=1.0;
    FragColor = /*shadowColor**/texColor*uTintColor;

    //FragColor=round(FragColor*8.0)/8.0;
}