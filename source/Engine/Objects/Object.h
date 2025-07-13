#pragma once
#include "../Includes/DataTypes.h"
#include <vector>
class Object{
    public:
        //constructors
        Object();
        ~Object();
        //transforms
        Transform transform;
        Mat4 matrix;
        //tree structure
        std::vector<Object*> children;
        Object* parent;
        //functions
        void updateMatrix();

        //drawing
        //draws the object as well as all its children
        void callDraw();
        //where the actual drawing for the object itself should be done
        virtual void onDraw();
};
class PhysicsObject : public Object{
    public:
        virtual void physicsTick(float delta);
};

//actual objects
class OBJ_Scene : public Object{};

//physics objects
struct CablePoint{
    Vec3 position;
    Vec3 previousPosition;
    bool fixed;

    //might replace later with storing the matrix
    Vec3 rotation;
};
class POBJ_Cable : public PhysicsObject{
    public:
        void physicsTick(float delta) override;
        void createCable(int pointCount,Vec3 start={0.0f,0.0f,0.0f},Vec3 end={0.0f,0.0f,0.0f});
        
        void onDraw() override;
        //cable settings
        float desiredDistance;

        std::vector<CablePoint> points;
    
        static inline constexpr int VARIANT_COUNT=6;
    private:
        bool hasBeenCreated=false;
        int cableVariant;
        Mat3 cableTextureMatrix;
};