#version 330 core

layout(location = 0) in vec4 aPosition; 
layout(location = 1) in vec4 aNormal;
layout(location = 2) in vec4 aColor;     
layout(location = 3) in vec2 aTexCoord;

layout(location = 4) in uint aGroupID;

//matrices for the different parts
//0th matrix will always be identity
uniform mat4 uMatrices[17]=mat4[17](
    //0
    mat4(1.0),
    //1-17
    mat4(1.0),mat4(1.0),mat4(1.0),mat4(1.0),
    mat4(1.0),mat4(1.0),mat4(1.0),mat4(1.0),
    mat4(1.0),mat4(1.0),mat4(1.0),mat4(1.0),
    mat4(1.0),mat4(1.0),mat4(1.0),mat4(1.0)
);


//matrices for the single model
uniform mat4 uProjectionMatrix;       
uniform mat4 uModelMatrix;    
uniform mat4 uModelNormal;            
uniform mat4 uViewMatrix;

uniform mat3 uTexCoordMatrix=mat3(
    1.0,0.0,0.0,
    0.0,1.0,0.0,
    0.0,0.0,1.0
);

out vec4 vColour;
out vec4 vWorldNormal;
out vec2 vTexCoord;            
            
void main()
{
    mat4 groupMatrix = uMatrices[aGroupID];

    gl_Position = uProjectionMatrix*uViewMatrix*uModelMatrix*groupMatrix*aPosition; 

    vWorldNormal = uModelNormal*aNormal;
    vColour = aColor;
    vTexCoord = vec2(uTexCoordMatrix*vec3(aTexCoord.xy,1.0)).xy;

    //gl_Position=round(gl_Position*32.0)/32.0;
}