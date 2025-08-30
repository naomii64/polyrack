#pragma once
#include "DataTypes.h"
//disable compiler specific warning
#pragma warning(disable : 4201)

struct Vertex {
    union {
        float position[3];
        struct { float x, y, z; };
    };
    union {
        float normal[3];
        struct { float nx, ny, nz; };
    };
    union {
        float color[4];
        struct { float r, g, b, a; };
    };
    union {
        float uvs[2];
        struct { float u, v; };
    };

    
    //quick set functions
    inline Vec3f xyz(){ return Vec3f(x,y,z); }
    inline void xyz(Vec3f vector){ x=vector.x;y=vector.y;z=vector.z; }
    inline void xyz(float scalar) { x=scalar;y=scalar;z=scalar; } 
    inline void xyz(float newX,float newY, float newZ) { x=newX; y=newY; z=newZ; } 

    inline Vec3f nxyz(){ return Vec3f(nx,ny,nz); }
    inline void nxyz(Vec3f vector){ nx=vector.x; ny=vector.y; nz=vector.z; }
    inline void nxyz(float scalar) { nx=scalar; ny=scalar; nz=scalar; } 
    inline void nxyz(float newX,float newY, float newZ) { nx=newX; ny=newY; nz=newZ; } 

    inline Vec4f rgba() { return Vec4f(r,g,b,a); }
    inline void rgba(Vec4f vector) {r=vector.r;g=vector.g;b=vector.b;a=vector.a;}
    inline void rgba(float scalar) {r=scalar;g=scalar;b=scalar;a=scalar;}
    inline void rgba(float newR,float newG,float newB,float newA) {r=newR;g=newG;b=newB;a=newA;}
    
    inline Vec2f uv() { return Vec2f(u,v); }
    inline void uv(Vec2f vector){ u=vector.x;v=vector.y; }
    inline void uv(float scalar){ u=scalar;v=scalar; }
    inline void uv(float newU, float newV){ u=newU;v=newV; }
    
};