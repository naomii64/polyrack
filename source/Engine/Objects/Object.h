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
        Mat4f matrix;
        Mat4f normalMatrix;

        Vec3f worldTranslation;
        //tree structure
        std::vector<Object*> children;
        Object* parent;
        //functions
        void updateMatrix();

        virtual void deleteObj();

        bool deleted=false;/*flags objects for deletion so they can be deleted in bulk*/
        //drawing
        //draws the object as well as all its children
        void callDraw();
        //where the actual drawing for the object itself should be done
        virtual void onDraw();

        virtual void onTransformChange();

        void setPosition(Vec3f newPosition);
        void setScale(Vec3f newScale);
        void setRotation(Vec3f newRotation);
        void setTransform(Vec3f newPosition, Vec3f newRotation, Vec3f newScale);

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

        void deleteObj() override;/*deletes the object from drawing idk*/

        //stores the unrounded position so that it can be rounded
        Vec3f truePosition;

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
        void deleteObj() override;
};
class OBJ_Comp_Input : public OBJ_Component{
    public:
        void onDraw() override;
        Animation* animation;
        Vec2f values;
        int hitboxID;
        void onTransformChange() override;
        void initComponent(const juce::var& fromObject,ModuleData& moduleData) override;
        void deleteObj() override;
};
//physics objects
struct CablePoint{
    Vec3f position;
    Vec3f previousPosition;
    bool fixed;
    bool lockRotation;

    //might replace later with storing the matrix
    Vec3f rotation;

    Mat4f matrix;
    Mat4f inverseRotationMatrix;

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
        
        void createCable(int pointCount,Vec3f start={0.0f,0.0f,0.0f},Vec3f end={0.0f,0.0f,0.0f});
        
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
        
        //MIGHT BE ABLE TO BE CONSTEXPR and run on compile time later
        static inline const std::array<Mat3f,POBJ_Cable::VARIANT_COUNT> generateVariantTextureMatrices(){
            std::array<Mat3f, POBJ_Cable::VARIANT_COUNT> arr{};

            for (size_t i = 0; i < POBJ_Cable::VARIANT_COUNT; ++i) {
                arr[i] = Mat3f::scaling(1.0f/float(POBJ_Cable::VARIANT_COUNT),1.0f)*Mat3f::translation(float(i),0.0f);
            }

            return arr;
        }
        static inline const auto cableVariantTextures=generateVariantTextureMatrices();
        int cableVariant;
        
        static void drawAll();//cables will be drawn all at once to save time
        static std::vector<POBJ_Cable*> allCables;
    private:
        bool hasBeenCreated=false;
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
inline Vec3f readVec3FromObj(const juce::var& obj, Vec3f defaultValues = { 0.0f, 0.0f, 0.0f }) {
    juce::Array<juce::var>* arr = obj.getArray();
    return Vec3f(
        getArrayValueOr(arr, 0, defaultValues.x),
        getArrayValueOr(arr, 1, defaultValues.y),
        getArrayValueOr(arr, 2, defaultValues.z)
    );
}
