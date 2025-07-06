#pragma once
#include "DataTypes.h"

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray() = default;
};
struct RayHit {
    bool hit = false;
    float distance = 0.0f;
};
struct AABB {
    Vec3 min; // Minimum corner (usually the bottom-left-back)
    Vec3 max; // Maximum corner (usually the top-right-front)

    AABB() = default;

    AABB(const Vec3& min, const Vec3& max)
        : min(min), max(max) {}

    // Check if a point is inside the AABB
    bool contains(const Vec3& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }
};

inline Ray screenToWorldRay(Vec2 screenPos, Mat4 viewMatrix, Mat4 projectionMatrix, Vec2 viewportSize) {
    // Convert screen position to normalized device coordinates (-1 to 1)
    float x = (2.0f * screenPos.x) / viewportSize.x - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / viewportSize.y;

    Vec4 clipCoords = Vec4(x, y, -1.0f, 1.0f);

    // Convert to view space
    Mat4 inverseProjection = projectionMatrix.inverse();
    Vec4 viewCoords = inverseProjection * clipCoords;
    viewCoords = Vec4(viewCoords.x, viewCoords.y, -1.0f, 0.0f);

    // Convert to world space
    Mat4 inverseView = viewMatrix.inverse();
    Vec4 worldCoords = inverseView * viewCoords;

    Vec3 rayDir = Vec3(worldCoords.x, worldCoords.y, worldCoords.z).normalized();
    Vec3 rayOrigin = inverseView.getTranslation(); // camera position

    return Ray{ rayOrigin, rayDir };
}
inline RayHit rayIntersectsAABB(const Ray& ray, const AABB& box) {
    RayHit result;

    float tMin = (box.min.x - ray.origin.x) / ray.direction.x;
    float tMax = (box.max.x - ray.origin.x) / ray.direction.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (box.min.y - ray.origin.y) / ray.direction.y;
    float tyMax = (box.max.y - ray.origin.y) / ray.direction.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax)) return result;

    tMin = std::max(tMin, tyMin);

    float tzMin = (box.min.z - ray.origin.z) / ray.direction.z;
    float tzMax = (box.max.z - ray.origin.z) / ray.direction.z;
    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) return result;

    tMin = std::max(tMin, tzMin);

    if (tMin < 0.0f) return result; // Hit is behind the ray origin

    result.hit = true;
    result.distance = tMin;
    return result;
}