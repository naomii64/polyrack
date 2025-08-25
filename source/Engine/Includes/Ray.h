#pragma once
#include "DataTypes.h"

struct Ray {
    Vec3f origin;
    Vec3f direction;

    Ray() = default;
};
struct RayHit {
    bool hit = false;
    float distance = 0.0f;
};
struct AABB {
    Vec3f min; // Minimum corner (usually the bottom-left-back)
    Vec3f max; // Maximum corner (usually the top-right-front)

    AABB() = default;

    AABB(const Vec3f& min, const Vec3f& max)
        : min(min), max(max) {}

    // Check if a point is inside the AABB
    bool contains(const Vec3f& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }
};

inline Ray screenToWorldRay(Vec2f screenPos, Mat4f viewMatrix, Mat4f projectionMatrix, Vec2f viewportSize) {
    // Convert screen position to normalized device coordinates (-1 to 1)
    float x = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / viewportSize.y;

    Vec4f clipCoords = Vec4f(x, y, -1.0f, 1.0f);

    // Convert to view space
    Mat4f inverseProjection = projectionMatrix.inverse();
    Vec4f viewCoords = inverseProjection * clipCoords;
    viewCoords = Vec4f(viewCoords.x, viewCoords.y, -1.0f, 0.0f);

    // Convert to world space
    Mat4f inverseView = viewMatrix.inverse();
    Vec4f worldCoords = inverseView * viewCoords;

    Vec3f rayDir = Vec3f(worldCoords.x, worldCoords.y, worldCoords.z).normalized();
    Vec3f rayOrigin = inverseView.getTranslation(); // camera position

    return Ray{ rayOrigin, rayDir };
}
inline RayHit rayIntersectsAABB(const Ray& ray, const AABB& box) {
    RayHit result;
    float tMin = -INFINITY;
    float tMax = INFINITY;
    const float epsilon = 1e-6f;

    for (int i = 0; i < 3; ++i) {
        float origin = ray.origin[i];
        float dir = ray.direction[i];
        float minB = box.min[i];
        float maxB = box.max[i];

        if (std::abs(dir) < epsilon) {
            // Ray is parallel to slab. No hit if origin not within slab
            if (origin < minB || origin > maxB)
                return result;
        } else {
            float invD = 1.0f / dir;
            float t1 = (minB - origin) * invD;
            float t2 = (maxB - origin) * invD;
            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) return result;
        }
    }

    if (tMin < 0.0f) return result; // intersection is behind the origin

    result.hit = true;
    result.distance = tMin;
    return result;
}