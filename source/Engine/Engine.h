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
        static void calculateDeltaTime();

        //rendering
        static Renderer* renderer;
        static void draw();

        //physics
        //runs every frame and tries to tick physics 60 times a second
        static inline constexpr float physicsDelta=1.0f/60.0f;
        static double physicsAccumulator;
        static void runPhysics();
        static void physicsTick();
        static double previousSeconds;
        //Engine deltatime
        static double deltaTime;
        static inline const Vec3 GRAVITY={0.0f,-98.0f,0.0f};
};