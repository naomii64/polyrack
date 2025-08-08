#pragma once
#include "../Includes/DataTypes.h"
#include <vector>

//forward declarations
struct Hitbox;


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
    Mat4 matrix;
};
class POBJ_Cable : public PhysicsObject{
    public:
        //overrides
        void physicsTick(float delta) override;
        void onDraw() override;
        
        void createCable(int pointCount,Vec3 start={0.0f,0.0f,0.0f},Vec3 end={0.0f,0.0f,0.0f});
        
        //Creates a hitbox for the corresponding point and returns the hitbox ID
        int createPointHitbox(CablePoint& point);

        // cable settings
        float desiredDistance;
        //data
        std::vector<CablePoint> points;    
        int hitboxAID;
        int hitboxBID;

        static inline constexpr int VARIANT_COUNT=6;
        static int variantCounter;
    private:
        bool hasBeenCreated=false;
        int cableVariant;
        Mat3 cableTextureMatrix;
        //separate physics into steps for readability
        void applyGravityAndMomentum(float delta);
        void applyConstraints();
        void rotatePoints();
        void calculateMatrices();
        void moveHitboxes();
};