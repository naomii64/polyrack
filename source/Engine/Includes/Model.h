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
    juce::OpenGLContext* context;

    Model(){}
    ~Model() {
        if(!context) return;
        auto& gl =  context->extensions;
        if (vbo != 0) gl.glDeleteBuffers(1, &vbo);
    }

    void createGeometry(juce::OpenGLContext& openGLContext, std::vector<Vertex> vertexBuffer, const std::vector<uint8_t>& groupBuffer = {}) {
        auto& gl = openGLContext.extensions;
        context=&openGLContext;

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

    int textureID = 0;
};