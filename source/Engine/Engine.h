#pragma once
#include "Objects/Object.h"
#include <vector>
#include "Renderer.h"

class Engine{
    public:
        static void init();
        //the main scene object thats drawn
        static OBJ_Scene* scene;
        //stores every object
        static std::vector<std::unique_ptr<Object>> objects;
        //stores refrences to objects that use physics, as to avoid physics ticking every single object
        static std::vector<PhysicsObject*> physicsObjects;
        static std::vector<OBJ_Comp_Socket*> sockets;

        static size_t nextTickedPhysicsObject;

        static void calculateDeltaTime();

        //rendering
        static Renderer* renderer;
        static void draw();
        /*
            Returns the position in 3D space where the given point on the screen intersects the given z value
        */
        static Vec3f screenPosToZPlane(Vec2f screenPos,float zValue);

        //physics
        //runs every frame and tries to tick physics 60 times a second
        static inline constexpr int physicsTicksPerSecond=30;
        static inline constexpr float physicsDelta=1.0f/float(physicsTicksPerSecond);
        static double physicsAccumulator;
        static void runPhysics();
        static double previousSeconds;
        //Engine deltatime
        static double deltaTime; //in seconds
        static inline const Vec3f GRAVITY={0.0f,-98.0f,0.0f};
        static Vec2f mousePosition;

        template<typename T>
        static T& createObject();
};

template <typename T>
inline T& Engine::createObject()
{
    return static_cast<T&>(*objects.emplace_back(std::make_unique<T>()));
}
