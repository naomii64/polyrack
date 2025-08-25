#pragma once
//juce includes
#include "juce_core/juce_core.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_opengl/juce_opengl.h"
//engine files
#include "Includes/Model.h"
#include "Includes/DataTypes.h"
#include "TextureManager.h"
//ui
#include "Includes/Ray.h"

#include <cmath>
#include <vector>

class Renderer : public juce::Component, public juce::OpenGLRenderer
{
public:
	Renderer();
	~Renderer() override;
	void paint(juce::Graphics&) override;
	
	//projection
	void updateProjectionMatrix();
    //isVisible was taken....
	bool canBeSeen();
    void disablePerspective();

    void drawModelAt(
		Model& model, 
		Vec3f position = {0.0f,0.0f,0.0f}, 
		Vec3f rotation = {0.0f,0.0f,0.0f}, 
		Vec3f scale = {1.0f,1.0f,1.0f}, 
		int textureID=0,
		Vec4f tint = {1.0f,1.0f,1.0f,1.0f}
	);
	void drawModelWithMatrix(
		Model &model, 
		Mat4f &matrix,
		Mat4f &normalMatrix,
		int textureID=0
	);
	void resizeBuffer(unsigned int newWidth,unsigned int newHeight);
	void createTextureFromImage(const juce::Image& image);
	//drawing functions for ui
	void drawRect(
		float x,
		float y,
		float width,
		float height,
		int textureID=0,
		Vec4f tint = {1.0f,1.0f,1.0f,1.0f}
	);
    void drawBorderRect(
		float x, 
		float y, 
		float width, 
		float height, 
		float borderWidth, 
		int textureID=0,
		Vec4f tint = {1.0f,1.0f,1.0f,1.0f}
	);

	void setUVMatrix(Mat3f matrix);

	Ray rayFrom(Vec2f screenPos);

    Vec2f getPixelSize();

    //juces
	juce::Image mainTextureImage=juce::Image();
	TextureManager* mainTextureAtlas=nullptr;

	juce::OpenGLContext openGLContext;



	//for easier acessing
	/*Screen size in pixels*/
	Vec2f screenSize{1.0f,1.0f};
	/*Normalized screen size (from -1.0 to 1.0 on the y)*/
	Vec2f screenSpaceSize{2.0f,2.0f};
	
	bool invertY=false;

	void setCameraPosition(Vec3f position,Vec3f rotation={0.0f,0.0f,0.0f});
	void uploadMatrixList(std::vector<Mat4f>& matrices);

	// //for typing and debugging
    // bool keyPressed(const juce::KeyPress& key) override {
    //     auto ch = key.getTextCharacter();

    //     if (juce::CharacterFunctions::isPrintable (ch)) {
    //         std::cout<<ch<<"\n";
    //     }
        
    //     return true;
    // }
private:
	// Create an OpenGLContext for this Component.
	juce::String vertexShader;
	juce::String fragmentShader;
	std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;

	juce::OpenGLFrameBuffer lowResBuffer;
	int screenResolutionY = 512;

	void newOpenGLContextCreated() override;
	void renderOpenGL() override;
    void createTextureRectsFromAtlas(TextureManager &atlas);
    void openGLContextClosing() override;
	
	float aspect = 0;
	float fov = 90;

	Mat4f cameraMatrix;

    Mat4f projectionMatrix;
	Mat4f orthographicMatrix;

	GLuint fullscreenVAO = 0;
	GLuint fullscreenVBO = 0;
	GLuint fullscreenEBO = 0;
	std::unique_ptr<juce::OpenGLShaderProgram> fullscreenShaderProgram;

	juce::OpenGLTexture mainTexture;

	bool bufferReloading = false;
};