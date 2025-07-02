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
        float uv[2];
        struct { float u, v; };
    };

    struct VectorProxy {
        float* ptr;
        VectorProxy(float* p) : ptr(p) {}
        VectorProxy& operator=(const Vec3& value) {
            ptr[0] = value.x;
            ptr[1] = value.y;
            ptr[2] = value.z;
            return *this;
        }
        VectorProxy& operator=(const float& value) {
            ptr[0] = value;
            ptr[1] = value;
            ptr[2] = value;
            return *this;
        }
        operator Vec3() const {
            return { ptr[0], ptr[1], ptr[2] };
        }
    };

    VectorProxy xyz{ position };
    VectorProxy rgb{ color };
    VectorProxy nxyz{ normal };
};