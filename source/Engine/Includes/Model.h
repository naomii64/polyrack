#pragma once
//juce imports
#include "juce_opengl/juce_opengl.h"
//engine imports
#include "DataTypes.h"
#include "Vertex.h"
#include <vector>

class Model {
public:
    // OpenGL buffer and vertex count
    GLuint vbo = 0;
    GLsizei vertexCount = 0;

    //group stuff
    GLuint groupVBO = 0;
    bool hasGroupBuffer = false;

    Model(){}
    ~Model() {
        //auto& gl = scene.openGLComponent.getContext().extensions;
        //if (vbo != 0) gl.glDeleteBuffers(1, &vbo);
    }
    // Create geometry from a vertex buffer only (no index buffer)
    void createGeometry(juce::OpenGLContext& openGLContext, const std::vector<Vertex>& vertexBuffer, const std::vector<uint8_t>& groupBuffer = {}) {
        auto& gl = openGLContext.extensions;
        
        if (vbo == 0) gl.glGenBuffers(1, &vbo);

        gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, vbo);
        gl.glBufferData(juce::gl::GL_ARRAY_BUFFER, sizeof(Vertex) * vertexBuffer.size(), vertexBuffer.data(), juce::gl::GL_STATIC_DRAW);

        vertexCount = static_cast<GLsizei>(vertexBuffer.size());
        

        // If group buffer is provided
        if (!groupBuffer.empty()) {

            if (groupVBO == 0) gl.glGenBuffers(1, &groupVBO);
            gl.glBindBuffer(juce::gl::GL_ARRAY_BUFFER, groupVBO);
            gl.glBufferData(juce::gl::GL_ARRAY_BUFFER, groupBuffer.size(),
                            groupBuffer.data(), juce::gl::GL_STATIC_DRAW);
            hasGroupBuffer = true;
        } else {
            hasGroupBuffer = false;
        }
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