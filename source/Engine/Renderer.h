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
#include "../UI/UIManager.h"
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

    void drawModel(Model &model);
    void drawModelAt(
		Model& model, 
		Vec3 position = {0.0f,0.0f,0.0f}, 
		Vec3 rotation = {0.0f,0.0f,0.0f}, 
		Vec3 scale = {1.0f,1.0f,1.0f}, 
		int textureID=0,
		Vec4 tint = {1.0f,1.0f,1.0f,1.0f}
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
		Vec4 tint = {1.0f,1.0f,1.0f,1.0f}
	);
    void drawBorderRect(
		float x, 
		float y, 
		float width, 
		float height, 
		//border width in pixels
		float borderWidth, 
		float uvBorderWidth,
		int textureID=0,
		Vec4 tint = {1.0f,1.0f,1.0f,1.0f}
	);
	Ray rayFrom(Vec2 screenPos);

    Vec2 getPixelSize();

    //juces
	juce::Image& mainTextureImage=juce::Image();
	TextureManager* mainTextureAtlas=nullptr;

	juce::OpenGLContext openGLContext;



	//for easier acessing
	/*Screen size in pixels*/
	Vec2 screenSize{1.0f,1.0f};
	/*Normalized screen size (from -1.0 to 1.0 on the y)*/
	Vec2 screenSpaceSize{2.0f,2.0f};
	
	bool invertY=false;

	void setCameraPosition(Vec3 position,Vec3 rotation={0.0f,0.0f,0.0f});

private:
	// Create an OpenGLContext for this Component.
	juce::String vertexShader;
	juce::String fragmentShader;
	std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;

	juce::OpenGLFrameBuffer lowResBuffer;
	int screenResolutionY = 512;

	//	screen sizes
	//	---------------
	//	2^7=128
	//	2^8=256
	//	2^9=512

	void newOpenGLContextCreated() override;
	void renderOpenGL() override;
    void createTextureRectsFromAtlas(TextureManager &atlas);
    void openGLContextClosing() override;
	
	float aspect = 0;
	float fov = 90;

	Mat4 cameraMatrix;

    Mat4 projectionMatrix;
	Mat4 orthographicMatrix;

	GLuint fullscreenVAO = 0;
	GLuint fullscreenVBO = 0;
	GLuint fullscreenEBO = 0;
	std::unique_ptr<juce::OpenGLShaderProgram> fullscreenShaderProgram;

	juce::OpenGLTexture mainTexture;

	bool bufferReloading = false;
};