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
        Mat4 normalMatrix;

        Vec3 worldTranslation;
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

        virtual void onTransformChange();

        void setPosition(Vec3 newPosition);
        void setScale(Vec3 newScale);
        void setRotation(Vec3 newRotation);
        void setTransform(Vec3 newPosition, Vec3 newRotation, Vec3 newScale);

        void addChild(Object* child);
    private:
        //so that matrixes are only recalculated once
        bool matrixRecalculationRequested = true;
};
class PhysicsObject : public Object{
    public:
        virtual void physicsTick(float delta);
};

//actual objects
class OBJ_Scene : public Object{};

struct ModuleData;
class Model;
struct Animation;

class OBJ_Module : public Object{
    public:
        //initializes the module using a pointer to the data
        void createFrom(ModuleData* moduleData);

        int width;
        int height;

        int mainHitboxID;

        void onTransformChange() override;
};
struct CablePoint;
//components
class OBJ_Component : public Object{
    public:
        virtual void initComponent(const juce::var& fromObject,ModuleData& moduleData);
};
class OBJ_Comp_Mesh : public OBJ_Component{
    public:
        void onDraw() override;
        void initComponent(const juce::var& fromObject,ModuleData& moduleData) override;
        Model* model;
};
class OBJ_Comp_Socket : public OBJ_Component{
    public:
        void onDraw() override;
        void initComponent(const juce::var& fromObject,ModuleData& moduleData) override;
        void onTransformChange() override;
        CablePoint* connectedPoint;
};
class OBJ_Comp_Input : public OBJ_Component{
    public:
        void onDraw() override;
        Animation* animation;
        Vec2 values;
        int hitboxID;
        void onTransformChange() override;
        void initComponent(const juce::var& fromObject,ModuleData& moduleData) override;
};
//physics objects
struct CablePoint{
    Vec3 position;
    Vec3 previousPosition;
    bool fixed;

    //might replace later with storing the matrix
    Vec3 rotation;
    Mat4 matrix;

    //these are only really needed for the first and last point
    bool isFirst;
    OBJ_Comp_Socket* connectedSocket=nullptr;
    
    void connectToSocket();
    void detachFromSocket();
    
    void lookAtSocket(OBJ_Comp_Socket& socket);
    
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

//unwrapping functions PROBABLY MOVE LATER
inline float getArrayValueOr(const juce::Array<juce::var>* arr, int index, float defaultVal) {
    if (arr != nullptr && index < arr->size())
        return (float)arr->getUnchecked(index);
    return defaultVal;
}
inline Vec3 readVec3FromObj(const juce::var& obj, Vec3 defaultValues = { 0.0f, 0.0f, 0.0f }) {
    juce::Array<juce::var>* arr = obj.getArray();
    return Vec3(
        getArrayValueOr(arr, 0, defaultValues.x),
        getArrayValueOr(arr, 1, defaultValues.y),
        getArrayValueOr(arr, 2, defaultValues.z)
    );
}
