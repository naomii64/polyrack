#pragma once
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include "juce_core/juce_core.h"

//disable compiler specific warning
#pragma warning(disable : 4201)

#define DEBUG_MODE true

#if DEBUG_MODE
#include <vector>
#endif

//list everything here so they can refrence eachother
struct Vec2;
struct Vec3;
struct Mat4;


struct Vec3 {
    union {
        struct { float x, y, z; };
        float data[3];
    };

    // Constructors
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
    
    float& operator[](int index) {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vec3 index out of range");
        }
    }
    const float& operator[](int index) const {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vec3 index out of range");
        }
    }
    //vector operations
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    Vec3 operator*(const Vec3& other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    Vec3 operator/(const Vec3& other) const {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }
    //incrementers or whatever theyre called
    Vec3& operator+=(const Vec3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    //scalar operations
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }
    Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    //misc
    Vec3 operator-() const {
        return Vec3{ -x, -y, -z };
    }
    //linar algebra
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    Vec3 rotationNormal() const {
        return Vec3(
            cos(y) * sin(x),
            sin(y),
            cos(y) * cos(x)
        );
    }
    // Magnitude and normalization
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    Vec3 normalized() const {
        float len = length();
        if (len == 0.0f) return Vec3(0, 0, 0);
        return *this / len;
    }

    static Vec3 random() {
        return Vec3(
            float(rand()) / RAND_MAX,
            float(rand()) / RAND_MAX,
            float(rand()) / RAND_MAX);
    }
};
struct Vec2 {
    //constructors
    union {
        struct { float x, y; };
        float data[2];
    };
    
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    explicit Vec2(float scalar) : x(scalar), y(scalar) {}
    

    //operations
    //vector operations
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2 operator*(const Vec2& other) const {
        return Vec2(x * other.x, y * other.y);
    }
    Vec2 operator/(const Vec2& other) const {
        return Vec2(x / other.x, y / other.y);
    }

    //scalar operations
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }
    Vec2 operator/(float scalar) const {
        return Vec2(x / scalar, y / scalar);
    }
    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Vec2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    std::string toString() const {
        std::string xString=std::to_string(x);
        std::string yString=std::to_string(y);

        return "("+xString+","+yString+")";
    }
};
struct Vec4 {
    //constructors
    union {
        struct { float x, y, z, w; };
        float data[4];
    };
    
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    explicit Vec4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    
    //vector operations
    Vec4 operator+(const Vec4& other) const {
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
    }
    Vec4 operator-(const Vec4& other) const {
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
    }
    Vec4 operator*(const Vec4& other) const {
        return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
    }
    Vec4 operator/(const Vec4& other) const {
        return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
    }

    Vec2 xy() const { return Vec2(x, y); }
    Vec2 zw() const { return Vec2(z, w); }

    static Vec4 random() {
        return Vec4(
            float(rand()) / RAND_MAX,
            float(rand()) / RAND_MAX,
            float(rand()) / RAND_MAX,
            float(rand()) / RAND_MAX
        );
    }

    std::string toString() const {
        std::string xString=std::to_string(x);
        std::string yString=std::to_string(y);
        std::string zString=std::to_string(z);
        std::string wString=std::to_string(w);
        
        return "("+xString+","+yString+","+zString+","+wString+")";
    }




    //for debugging
    #if DEBUG_MODE
    static void debugVector(std::vector<Vec4>& vector){
        std::cout << "vector of "<< vector.size() <<" Vec4s:" << std::endl;
        for(Vec4& v: vector){
            std::cout << "\t" << v.toString() << std::endl;
        }
    }
    #endif
};
struct Mat4 {
    float data[16]; // column-major order

    Mat4() {
        for (int i = 0; i < 16; ++i)
            data[i] = (i % 5 == 0) ? 1.0f : 0.0f; // identity
    }
    Mat4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        data[0] = m00; data[4] = m01; data[8] = m02;  data[12] = m03;
        data[1] = m10; data[5] = m11; data[9] = m12;  data[13] = m13;
        data[2] = m20; data[6] = m21; data[10] = m22;  data[14] = m23;
        data[3] = m30; data[7] = m31; data[11] = m32;  data[15] = m33;
    }

    static constexpr float identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    float& operator()(int row, int col) { return data[col * 4 + row]; }
    const float& operator()(int row, int col) const { return data[col * 4 + row]; }

    // Multiply two matrices
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += (*this)(row, k) * other(k, col);
                }
                result(row, col) = sum;
            }
        }
        return result;
    }

    // Translation matrix
    static Mat4 translation(float x, float y, float z) {
        Mat4 result;
        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;
        return result;
    }
    static Mat4 translation(const Vec3& v) {
        return translation(v.x, v.y, v.z);
    }
    // Scaling matrix
    static Mat4 scaling(float x, float y, float z) {
        Mat4 result;
        result(0, 0) = x;
        result(1, 1) = y;
        result(2, 2) = z;
        return result;
    }
    static Mat4 scaling(const Vec3& v) {
        return scaling(v.x, v.y, v.z);
    }

    // Rotation around X axis
    static Mat4 rotationX(float radians) {
        Mat4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;
        return result;
    }

    // Rotation around Y axis
    static Mat4 rotationY(float radians) {
        Mat4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        result(0, 0) = c;
        result(0, 2) = s;
        result(2, 0) = -s;
        result(2, 2) = c;
        return result;
    }

    // Rotation around Z axis
    static Mat4 rotationZ(float radians) {
        Mat4 result;
        float c = std::cos(radians);
        float s = std::sin(radians);
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        return result;
    }

    // Combined rotation from Vec3 (pitch, yaw, roll)
    static Mat4 rotation(const Vec3& v) {
        return rotationZ(v.z) * rotationY(v.y) * rotationX(v.x);
    }
    static Mat4 rotation(float x, float y, float z) {
        return rotation(Vec3(x, y, z));
    }
    //perspective
    static Mat4 makePerspectiveMatrix(float fovYDegrees, float aspect, float nearZ, float farZ)
    {
        float fovYRadians = juce::degreesToRadians(fovYDegrees);
        float f = 1.0f / std::tan(fovYRadians / 2.0f);
        float rangeInv = 1.0f / (nearZ - farZ);

        return Mat4{
            f / aspect, 0.0f,        0.0f,                                  0.0f,
            0.0f,       f,           0.0f,                                  0.0f,
            0.0f,       0.0f,        (nearZ + farZ) * rangeInv,             (2.0f * nearZ * farZ) * rangeInv,
            0.0f,       0.0f,       -1.0f,                                  0.0f
        };
    }
    // Conversion operator to float* (non-const)
    operator float* () {
        return data;
    }
    Mat4 Mat4::fastInverseRT() const {
        Mat4 inv;

        // transpose the 3x3 rotation part
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 3; ++col)
                inv.data[col * 4 + row] = data[row * 4 + col]; // assuming column-major

        // inverse translation
        inv.data[12] = -(inv.data[0] * data[12] + inv.data[4] * data[13] + inv.data[8] * data[14]);
        inv.data[13] = -(inv.data[1] * data[12] + inv.data[5] * data[13] + inv.data[9] * data[14]);
        inv.data[14] = -(inv.data[2] * data[12] + inv.data[6] * data[13] + inv.data[10] * data[14]);

        // last row stays the same
        inv.data[3] = 0.0f;
        inv.data[7] = 0.0f;
        inv.data[11] = 0.0f;
        inv.data[15] = 1.0f;

        return inv;
    }
    Mat4 Mat4::transpose() const {
        Mat4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result(i,j) = (float(j),float(i));
        return result;
    }
    // Conversion operator to const float* (for const Mat4)
    operator const float* () const {
        return data;
    }
};