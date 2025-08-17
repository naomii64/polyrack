#pragma once
#include "Engine.h"

#include "Renderer.h"
#include "Scene.h"
#include "EngineAssets.h"
#include "../PluginEditor.h"

#define USE_FRAME_BUFFER false

#include "../Defines.h"
#if draw_hitboxes
#include "Hitboxes.h"
#endif

#include "../shaders/shaders.h"

#pragma warning( disable : 4100)

const char* fullscreenVertexShader = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;

    out vec2 vTexCoord;

    void main()
    {
        vTexCoord = aTexCoord;
        gl_Position = vec4(aPos.xy, 0.0, 1.0);
    }
)";
const char* fullscreenFragmentShader = R"(
    #version 330 core
    in vec2 vTexCoord;

    out vec4 FragColor;

    uniform sampler2D screenTexture;
    uniform vec2 screenSize;

   float ditherPattern(vec2 pos) {
        float bayer[16] = float[](
            0.0, 8.0, 2.0, 10.0,
            12.0, 4.0, 14.0, 6.0,
            3.0, 11.0, 1.0, 9.0,
            15.0, 7.0, 13.0, 5.0
        );

        int x = int(floor(fract(pos.x) * 4.0));
        int y = int(floor(fract(pos.y) * 4.0));

        return bayer[y * 4 + x]/16.0;
    }

    vec4 dither(vec4 color, vec2 screenCoord){
        vec4 colorA = floor(color * 16.0) / 16.0;
        vec4 colorB = ceil(color * 16.0) / 16.0;

        vec4 colorDiff = colorB - colorA;
        colorDiff = max(colorDiff, vec4(1e-6)); // avoid division by zero

        float ditherSize = 4.0;
        vec2 ditherPos = fract(screenCoord / ditherSize);

        vec4 dith = vec4(round(ditherPattern(ditherPos)*4.0)/4.0);
        vec4 colorAdifference = (color - colorA) / colorDiff;
        dith = dith + colorAdifference;

        return mix(colorA, colorB, floor(dith));
    }



    void main()
    {
        vec2 screenCoord = screenSize*vTexCoord;

        FragColor = dither(texture(screenTexture, vTexCoord),screenCoord);
    }
)";

Renderer::Renderer(){
    setOpaque(true);
    openGLContext.setRenderer(this);
    openGLContext.setContinuousRepainting(true);
    openGLContext.attachTo(*this);
}
Renderer::~Renderer(){
    openGLContext.detach();
}

//========================//
//-----DRAW FUNCTIONS-----//
//========================//

//draws a model with a given transform without modifying it. great for models that get reused
void Renderer::drawModelAt(Model& model, Vec3 position, Vec3 rotation, Vec3 scale,int textureID,Vec4 tint){
    Vec3 usedScale=scale;
    if(invertY){
        usedScale.y*=-1.0f;
    }
    Mat4 mat=Model::getTransformMatrix(position,rotation,usedScale);
    Mat4 nmat=Model::getModelNormalMatrix(rotation);
    drawModelWithMatrix(model,mat,nmat,textureID);

}
void Renderer::drawModelWithMatrix(Model &model, Mat4 &matrix,Mat4 &normalMatrix, int textureID)
{

        auto& gl = openGLContext.extensions;

        gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, model.vbo);

        gl.glEnableVertexAttribArray(0);
        gl.glEnableVertexAttribArray(1);
        gl.glEnableVertexAttribArray(2);
        gl.glEnableVertexAttribArray(3);
        
        //xyz
        gl.glVertexAttribPointer(0, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), nullptr);
        //nxyz
        gl.glVertexAttribPointer(1, 3, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 3));
        //rgba
        gl.glVertexAttribPointer(2, 4, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 6));
        //uv
        gl.glVertexAttribPointer(3, 2, juce::gl::GL_FLOAT, juce::gl::GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(float) * 10));
        
        if (model.hasGroupBuffer) {
            gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, model.groupVBO);
            gl.glEnableVertexAttribArray(4); // or any unused attribute slot
            juce::gl::glVertexAttribIPointer(4, 1, juce::gl::GL_UNSIGNED_BYTE, juce::gl::GL_FALSE, 0);
        }
        


        shaderProgram->setUniformMat4("uModelMatrix",matrix.data, 1, juce::gl::GL_FALSE);
        shaderProgram->setUniformMat4("uModelNormal", normalMatrix.data, 1, juce::gl::GL_FALSE);
        
        shaderProgram->setUniform("uTextureID",textureID);
        
        shaderProgram->setUniform("uTintColor",1.0f,1.0f,1.0f,1.0f);
        // Draw arrays instead of elements
        juce::gl::glDrawArrays(juce::gl::GL_TRIANGLES, 0, model.vertexCount);

        gl.glDisableVertexAttribArray(0);
        gl.glDisableVertexAttribArray(1);
        gl.glDisableVertexAttribArray(2);
        gl.glDisableVertexAttribArray(3);
        gl.glDisableVertexAttribArray(4);
}
void Renderer::drawModel(Model &model)
{
    drawModelAt(model,model.position,model.rotation,model.scale);
}

//ui related ones
void Renderer::drawRect(float x, float y, float width, float height, int textureID, Vec4 tint) {
    constexpr float z = -1.0f;
    constexpr float depth = 1.0f;

    const Vec3 position = {x, y, z};
    const Vec3 rotation = {0.0f, 0.0f, 0.0f}; // or a named constant like NO_ROTATION
    const Vec3 scale = {width, height, depth};

    drawModelAt(EngineAssets::mTestSquare, position, rotation, scale,textureID,tint);
}

void Renderer::drawBorderRect(float x, float y, float width, float height, float borderWidth , int textureID, Vec4 tint) {
    //still probably too much math for a simple rectange, might optimize later
    //probably make a class that stores all this and only recalculates when things change
    //i have no idea why down being +y and -y is inconsistent

    constexpr float z = -1.0f;

    const float borderWidth2 = borderWidth*2;

    const float usedHeight = std::max(height,borderWidth2);
    const float usedWidth = std::max(width,borderWidth2);
    
    const Vec3 position = {x, y, z};
    const Vec3 rotation = {0.0f, 0.0f, 0.0f};
    const Vec3 scale(1.0f);


    std::vector<Mat4> matrices;
    for(int i=0;i<17;i++)matrices.emplace_back();

    const Mat4 cornerScale = Mat4::scaling(Vec3(borderWidth));
    const Mat4 verticalBarScale = Mat4::scaling(Vec3(borderWidth,usedHeight-borderWidth2,1.0f));
    const Mat4 horizontalBarScale = Mat4::scaling(Vec3(usedWidth-borderWidth2,borderWidth,1.0f));
    

    const float rightXvalue = usedWidth-borderWidth;
    const float bottomYvalue = -usedHeight+borderWidth;

    matrices[NineSlice::TOP_LEFT+1]=cornerScale;
    matrices[NineSlice::BOTTOM_RIGHT+1]=Mat4::translation(rightXvalue,bottomYvalue,0.0f)*cornerScale;
    matrices[NineSlice::BOTTOM_LEFT+1]=Mat4::translation(0.0f,bottomYvalue,0.0f)*cornerScale;
    matrices[NineSlice::TOP_RIGHT+1]=Mat4::translation(rightXvalue,0.0f,0.0f)*cornerScale;
    
    matrices[NineSlice::TOP_MIDDLE+1]=Mat4::translation(borderWidth,0.0f,0.0f)*horizontalBarScale;
    matrices[NineSlice::BOTTOM_MIDDLE+1]=Mat4::translation(borderWidth,bottomYvalue,0.0f)*horizontalBarScale;

    matrices[NineSlice::MIDDLE_RIGHT+1]=Mat4::translation(rightXvalue,-borderWidth,0.0f)*verticalBarScale;
    matrices[NineSlice::MIDDLE_LEFT+1]=Mat4::translation(0.0f,-borderWidth,0.0f)*verticalBarScale;

    matrices[NineSlice::MIDDLE_MIDDLE+1]=Mat4::translation(borderWidth,-borderWidth,0.0f)* Mat4::scaling(usedWidth-borderWidth2,usedHeight-borderWidth2,0.0f);

    uploadMatrixList(matrices);

    drawModelAt(EngineAssets::mBorderRectModel, position, rotation, scale,textureID,tint);
}

void Renderer::setUVMatrix(Mat3 matrix)
{
    shaderProgram->setUniformMat3("uTexCoordMatrix",matrix.data,1,false);
}

Ray Renderer::rayFrom(Vec2 screenPos)
{
    return screenToWorldRay(screenPos,cameraMatrix,projectionMatrix,Vec2(float(getWidth()),float(getHeight())));
}

Vec2 Renderer::getPixelSize(){
    return Vec2(screenSpaceSize.x/getWidth(),screenSpaceSize.y/getHeight());
}

void Renderer::setCameraPosition(Vec3 position,Vec3 rotation)
{

    Mat4 translate = Mat4::translation(position*-1.0f);
    Mat4 rotate = Mat4::rotation(rotation).fastInverseRT();

    cameraMatrix=rotate*translate;
}

void Renderer::uploadMatrixList(std::vector<Mat4>& matrices)
{
    #define gl juce::gl
        
    if (matrices.empty()) {
        std::cout << "no matrices to upload!\n";
        return;
    }

    const int matrixCount = int(matrices.size());
    //const int floatLength = matrixCount * 16;
    GLfloat data[17 * 16]; //there are 17 matrices

    // Flatten Mat4 list into float array
    for (int i = 0; i < matrixCount; ++i) {
        const Mat4& m = matrices[i];
        for (int j = 0; j < 16; ++j) {
            data[i * 16 + j] = m.data[j]; // assumes your Mat4 has float data[16]
        }
    }

    shaderProgram->use();
    gl::glUniformMatrix4fv(shaderProgram->getUniformIDFromName("uMatrices"), matrixCount, gl::GL_FALSE, data);

    #undef gl
}

void Renderer::newOpenGLContextCreated()
{
    AudioPluginAudioProcessorEditor::mainProcessEditor->onRendererLoad();
    #define gl juce::gl
    //define shaders
    vertexShader = VS_GLSL;
    fragmentShader = FS_GLSL;

    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    //check for compilation errors
    if (shaderProgram->addVertexShader(vertexShader) && shaderProgram->addFragmentShader(fragmentShader) && shaderProgram->link())
    {
        shaderProgram->use();
    }
    else
    {
        jassertfalse;
    }
    //enabling stuff
    gl::glEnable(gl::GL_BLEND);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);
    gl::glEnable(gl::GL_CULL_FACE);
    gl::glCullFace(gl::GL_BACK);

    //==============================
    //load the main texture
    //==============================
    createTextureFromImage(mainTextureImage);
    createTextureRectsFromAtlas(*mainTextureAtlas);
    //==============================
    // Fullscreen shader program
    //==============================
    #if USE_FRAME_BUFFER
    fullscreenShaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));
    fullscreenShaderProgram->addVertexShader(fullscreenVertexShader);
    fullscreenShaderProgram->addFragmentShader(fullscreenFragmentShader);
    fullscreenShaderProgram->link();

    // Fullscreen quad setup
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    gl::glGenVertexArrays(1, &fullscreenVAO);
    gl::glGenBuffers(1, &fullscreenVBO);
    gl::glGenBuffers(1, &fullscreenEBO);

    gl::glBindVertexArray(fullscreenVAO);

    gl::glBindBuffer(gl::GL_ARRAY_BUFFER, fullscreenVBO);
    gl::glBufferData(gl::GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, gl::GL_STATIC_DRAW);

    gl::glBindBuffer(gl::GL_ELEMENT_ARRAY_BUFFER, fullscreenEBO);
    gl::glBufferData(gl::GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, gl::GL_STATIC_DRAW);

    gl::glEnableVertexAttribArray(0);
    gl::glVertexAttribPointer(0, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float), (void*)0);

    gl::glEnableVertexAttribArray(1);
    gl::glVertexAttribPointer(1, 2, gl::GL_FLOAT, gl::GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    gl::glBindVertexArray(0);

    lowResBuffer.initialise(openGLContext,1, 1);

    GLuint texID = lowResBuffer.getTextureID();
    gl::glBindTexture(gl::GL_TEXTURE_2D, texID);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MIN_FILTER, gl::GL_NEAREST);
    gl::glTexParameteri(gl::GL_TEXTURE_2D, gl::GL_TEXTURE_MAG_FILTER, gl::GL_NEAREST);
    gl::glBindTexture(gl::GL_TEXTURE_2D, 0);
    #endif

    //initialize engine
    Engine::renderer=this;
    Engine::init();
}
void Renderer::renderOpenGL(){
    Engine::calculateDeltaTime();
    Engine::runPhysics();

    if(!canBeSeen()){return;}
    
    shaderProgram->use();
    //check the size early on so we can skip frames to lessen the flickering
    updateProjectionMatrix();
    
    #if USE_FRAME_BUFFER
    //abandon the frame if the size was changed
    if((bufferReloading)||(!lowResBuffer.isValid())){bufferReloading = false;return;}
    lowResBuffer.makeCurrentRenderingTarget();
    gl::glViewport(0, 0, lowResBuffer.getWidth(), lowResBuffer.getHeight());
    #endif

    Scene::applyCameraSettings(*this);

    gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl::glClear(gl::GL_COLOR_BUFFER_BIT | gl::GL_DEPTH_BUFFER_BIT);
    //bind the texture

    gl::glActiveTexture(gl::GL_TEXTURE0);
    gl::glBindTexture(gl::GL_TEXTURE_2D, mainTexture.getTextureID());
    shaderProgram->setUniform("myTexture", 0);
    
    
    gl::glEnable(gl::GL_DEPTH_TEST);
    gl::glDepthFunc(gl::GL_LEQUAL);
    shaderProgram->setUniformMat4("uViewMatrix", cameraMatrix, 1, gl::GL_FALSE);
    
    //======================
    //>>>>draw calls go here

    //draw all the instances of modules
    Engine::draw();

    #if draw_hitboxes
    HitboxManager::drawAllHitBoxes(*this);
    #endif


    //prepare to draw the ui
    disablePerspective();
    shaderProgram->setUniformMat4("uViewMatrix", Mat4::identity, 1, gl::GL_FALSE);

    gl::glClear( gl::GL_DEPTH_BUFFER_BIT);

    //drawBorderRect(0,0,Scene::mousePos.x,Scene::mousePos.y,20,EngineAssets::tBorder);
    AudioPluginAudioProcessorEditor::mainProcessEditor->mainUI.callDraw();

    //TEST THING FOR SHOWING THE TEXTURE
    if(juce::KeyPress::isKeyCurrentlyDown('g')) drawModelAt(EngineAssets::mTestSquare,Vec3(0.0f),Vec3(0.0f),{screenSize.x,screenSize.y,1.0f},0);

    //>>>>end of draw cals
    //=====================

    #if USE_FRAME_BUFFER
    lowResBuffer.releaseAsRenderingTarget();
    //=======================
    //draw full screen buffer
    //=======================
    openGLContext.makeActive();
    
    gl::glDisable(gl::GL_DEPTH_TEST);
    gl::glDisable(gl::GL_BLEND);
    
    gl::glViewport(0, 0, getWidth(), getHeight());
    
    fullscreenShaderProgram->use();
    fullscreenShaderProgram->setUniform("screenTexture", 0);
    fullscreenShaderProgram->setUniform("screenSize", (float)lowResBuffer.getWidth(), (float)lowResBuffer.getHeight());

    gl::glActiveTexture(gl::GL_TEXTURE0);
    gl::glBindTexture(gl::GL_TEXTURE_2D, lowResBuffer.getTextureID());
    
    gl::glBindVertexArray(fullscreenVAO);
    gl::glDrawElements(gl::GL_TRIANGLES, 6, gl::GL_UNSIGNED_INT, 0);
    gl::glBindVertexArray(0);
    #endif
}
void Renderer::createTextureRectsFromAtlas(TextureManager& atlas){
    if(&atlas==nullptr){
        std::cout << "failed to bind texture uvs: Texture atlas is null" << std::endl;
        return;
    }
    std::vector<Vec4>& textureRects = atlas.textureCoords;
    //for now the glFloat holds a max of 64 textures for now
    const int floatLength = 64*4;
    GLfloat data[floatLength];
    //fill the data in
    // 2. Flatten the Vec4 list into a float array
    const int textureCount = int(textureRects.size());
    for(int i=0;i<textureCount;i++){
        Vec4& currentTextureRect = textureRects[i];
        int ix4=i*4;
        data[ix4]=currentTextureRect.x;
        //textures are being loaded in reverse order for some reason on the y
        data[ix4+1]=1.0f-(currentTextureRect.y+currentTextureRect.w);
        data[ix4+2]=currentTextureRect.z;
        data[ix4+3]=currentTextureRect.w;
    }


    // 3. Upload it as a uniform vec4 array
    shaderProgram->use();
    gl::glUniform4fv(shaderProgram->getUniformIDFromName("uTextureRects"),floatLength,data);
}
void Renderer::openGLContextClosing(){}
//disable the warning for unrefrenced g
#pragma warning(disable : 4100)
void Renderer::paint(juce::Graphics& g){}
bool Renderer::canBeSeen(){
    return ((getHeight()>0)&&(getWidth()>0));
}
void Renderer::updateProjectionMatrix(){
    //the screen isnt even visible
    if(!canBeSeen()){return;}
    float newAspect = float(getWidth()) / float(getHeight());
    if (aspect != newAspect) {
        aspect = newAspect;
        projectionMatrix = Mat4::makePerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);
        //used for ui, maps coords in pixels instead of 3d space
        Vec2 pixelSize=getPixelSize();
        orthographicMatrix = Mat4{
            2.0f/getWidth(),0.0f,0.0f,-1.0f,
            0.0f,-2.0f/getHeight(),0.0f,1.0f,
            0.0f,0.0f,0.0f,1.0f,
            0.0f,0.0f,0.0f,1.0f
        };

        //for easier reading
        screenSize.x = float(getWidth());
        screenSize.y = float(getHeight());

        screenSpaceSize.x = aspect*screenSpaceSize.y;

        #if USE_FRAME_BUFFER
        resizeBuffer(int(float(screenResolutionY)*aspect),screenResolutionY);
        #endif
    }
    shaderProgram->setUniformMat4("uProjectionMatrix", projectionMatrix, 1, gl::GL_FALSE);
    invertY=false;
}
void Renderer::disablePerspective(){
    shaderProgram->setUniformMat4("uProjectionMatrix", orthographicMatrix, 1, gl::GL_FALSE);
    invertY=true;
}
void Renderer::resizeBuffer(unsigned int newWidth,unsigned int newHeight){
    if (unsigned(lowResBuffer.getWidth()) == newWidth && unsigned(lowResBuffer.getHeight()) == newHeight) {return;}

    #undef gl
    // 1. Release old buffer if needed
    lowResBuffer.release();
    // 2. Initialize with new size
    lowResBuffer.initialise(openGLContext, newWidth, newHeight);

    // 3. Setup texture filtering or any other params again
    GLuint texID = lowResBuffer.getTextureID();

    juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, texID);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MIN_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MAG_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, 0);

    //try to but fail to prevent flickering
    bufferReloading = true;

}
void Renderer::createTextureFromImage(const juce::Image& image)
{
    //check if image is valid
    if (!image.isValid())
    {
        std::cout << "Invalid image!" << std::endl;
        return;
    }
    //check if the openGLContext is active
    if (!openGLContext.isActive())
    {
        std::cout << "OpenGL context not active!" << std::endl;
        return;
    }

    //==============================================
    //load the texture image and set the parameters
    //==============================================

    mainTexture.loadImage(image);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MIN_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MAG_FILTER, juce::gl::GL_NEAREST);
    juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, 0);

    //debug out the texutre id
    //std::cout << "Texture ID = " << mainTexture.getTextureID() << std::endl;
}