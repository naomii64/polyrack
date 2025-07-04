#pragma once
//juce imports
#include "juce_opengl/juce_opengl.h"
//engine imports
#include "DataTypes.h"
#include "Vertex.h"

#define DEBUG_MODE true

class Model {
public:
    // OpenGL buffer and vertex count
    GLuint vbo = 0;
    GLsizei vertexCount = 0;

    Model(){}
    ~Model() {
        //auto& gl = scene.openGLComponent.getContext().extensions;
        //if (vbo != 0) gl.glDeleteBuffers(1, &vbo);
    }
    // Create geometry from a vertex buffer only (no index buffer)
    void createGeometry(juce::OpenGLContext& openGLContext, const std::vector<Vertex>& vertexBuffer) {
        auto& gl = openGLContext.extensions;
        
        #if DEBUG_MODE
            std::cout << "generating mode of " << vertexBuffer.size() << " vertices" << std::endl;
        #endif

        if (vbo == 0) gl.glGenBuffers(1, &vbo);

        gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
        gl.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(Vertex) * vertexBuffer.size(), vertexBuffer.data(), juce::gl::GL_STATIC_DRAW);

        vertexCount = static_cast<GLsizei>(vertexBuffer.size());
        
        #if DEBUG_MODE
            std::cout << "done!" << std::endl;
        #endif
    }
    // Transform data
	Vec3 position{ 0.0f, 0.0f, 0.0f };
	Vec3 rotation{ 0.0f, 0.0f, 0.0f };
	Vec3 scale{ 1.0f, 1.0f, 1.0f };

    int textureID = 0;

    //static functions for drawing the model
    static Mat4 getTransformMatrix(Vec3 position, Vec3 rotation, Vec3 scale){
        return Mat4::translation(position) * Mat4::rotation(rotation) * Mat4::scaling(scale);
    }
    static Mat4 getModelNormalMatrix(Vec3 rotation){
        return Mat4::rotation(rotation).fastInverseRT();
    }
};