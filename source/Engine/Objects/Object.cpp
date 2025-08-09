#pragma once
#include "Object.h"
#include "../Engine.h"
#include "../EngineAssets.h"
#include "../Hitboxes.h"
#include "../ModuleManager.h"

Object::Object(){}
Object::~Object(){}
void Object::updateMatrix(){
    if (parent){
        matrix = parent->matrix * transform.getMatrix();
    }else{
        matrix = transform.getMatrix();
    }
    for(Object* obj : children){
        obj->updateMatrix();
    }

    matrixRecalculationRequested=false;
    onTransformChange();
}
void Object::callDraw()
{
    if(matrixRecalculationRequested) updateMatrix();
    onDraw();
    for(Object* child : children){
        child->callDraw();
    }
}
void Object::onDraw(){}
void Object::onTransformChange(){}

void Object::setPosition(Vec3 newPosition)
{
    transform.position = newPosition;
    matrixRecalculationRequested=true;
}
void Object::setScale(Vec3 newScale)
{
    transform.scale = newScale;
    matrixRecalculationRequested=true;
}
void Object::setRotation(Vec3 newRotation)
{
    transform.rotation = newRotation;
    matrixRecalculationRequested=true;
}
void Object::setTransform(Vec3 newPosition, Vec3 newRotation, Vec3 newScale)
{
    transform.position = newPosition;
    transform.rotation = newRotation;
    transform.scale = newScale;
    matrixRecalculationRequested=true;
}

void PhysicsObject::physicsTick(float delta) {}

//subclasses
int POBJ_Cable::variantCounter=0;
void POBJ_Cable::applyGravityAndMomentum(float delta){
    for(CablePoint& point : points){
        if(point.fixed){
            point.previousPosition=point.position;
            continue;
        }
        Vec3 copyPos=point.position;

        point.position=point.position*2-point.previousPosition+Engine::GRAVITY*delta*delta;

        point.previousPosition=copyPos;
    }
}
void POBJ_Cable::applyConstraints(){
    const int ITERATION_COUNT=10;
    for(int iteration=0;iteration<ITERATION_COUNT;iteration++){
        for(int i=1;i<points.size();i++){
            CablePoint& pointA = points[i-1];
            CablePoint& pointB = points[i];

            //will probably never happen but still
            if(pointA.fixed && pointB.fixed) continue;

            Vec3 diff=pointA.position-pointB.position;
            //avoid div by 0
            int x=*reinterpret_cast<int*>(&diff.x);
            int y=*reinterpret_cast<int*>(&diff.y);
            int z=*reinterpret_cast<int*>(&diff.z);
            if((x|y|z)==0){
                //feels too biased in one direction, might change later
                diff.y=-0.0001f;
            }

            //calculate the distance once
            float distance = diff.length();
            //normalize using that distance
            Vec3 dir=diff/distance;

            float distanceError = distance-desiredDistance;
            Vec3 distErr=dir*distanceError;
            
            //should be the most common case, gets checked first
            if(!pointA.fixed && !pointB.fixed){
                distErr*=0.5f;
                pointB.position+=distErr;
                pointA.position-=distErr;
                continue;
            }
            //now we know for sure that one that point is fixed and the other isnt, we only need to check one
            if(pointA.fixed){
                pointB.position+=distErr;
                continue;
            }else{
                pointA.position-=distErr;
                continue;
            }


        }
    }
}
void POBJ_Cable::rotatePoints(){
    for (int i = 0; i < points.size() - 1; i++) {
        CablePoint& pointA = points[i];
        CablePoint& pointB = points[i + 1];

        Vec3 dir = pointB.position - pointA.position;

        // Normalize the direction
        Vec3 fwd = dir.normalized(); // Assuming you have a .normalized() function

        // Calculate pitch and yaw
        float pitch = std::atan2(-fwd.y, std::sqrt(fwd.x * fwd.x + fwd.z * fwd.z)); // rotation around X
        float yaw   = std::atan2(fwd.x, fwd.z); // rotation around Y
        float roll=0.0f;

        pointA.rotation = { pitch, yaw, roll }; // roll = 0
    }
    points.back().rotation=points[points.size()-2].rotation;
}
void POBJ_Cable::calculateMatrices(){
    for(CablePoint& point : points){
        //order is handled differently for cables
        Mat4 rotX = Mat4::rotationX(point.rotation.x);
        Mat4 rotY = Mat4::rotationY(point.rotation.y);
        Mat4 rotZ = Mat4::rotationZ(point.rotation.z);
        Mat4 rotationMatrix = rotY * rotX * rotZ;
        point.matrix=Mat4::translation(point.position)*rotationMatrix;
    }
}
void POBJ_Cable::moveHitboxes(){
    Hitbox& hitboxA = HitboxManager::hitboxes[hitboxAID];
    Hitbox& hitboxB = HitboxManager::hitboxes[hitboxBID];

    CablePoint& pointA = points[0];
    CablePoint& pointB = points.back();

    hitboxA.position=pointA.position;
    hitboxB.position=pointB.position;
}

void POBJ_Cable::physicsTick(float delta){
    if(!hasBeenCreated)return;
    applyGravityAndMomentum(delta);
    applyConstraints();
    rotatePoints();
    calculateMatrices();
    moveHitboxes();
}
void POBJ_Cable::createCable(int pointCount,Vec3 start,Vec3 end){
    hasBeenCreated=true;
    points.resize(pointCount);
    for(int i=0;i<points.size();i++){
        float interpolationValue=float(i)/float(points.size()-1);
        Vec3 currentPointPos = Vec3::lerp(start,end,interpolationValue);
        points[i].position=currentPointPos;
        points[i].previousPosition=currentPointPos;
    }

    //create and assign hitboxes
    hitboxAID = createPointHitbox(points[0]);
    hitboxBID = createPointHitbox(points.back());
    
    //THIS IS PROBABLY JUST TEMPORARY
    points[0].fixed=true;
    
    //===CABLE VARIANT===//
    cableVariant=POBJ_Cable::variantCounter++ % POBJ_Cable::VARIANT_COUNT;
    cableTextureMatrix=Mat3::scaling(1.0f/float(POBJ_Cable::VARIANT_COUNT),1.0f)*Mat3::translation(float(cableVariant),0.0f);
}

void POBJ_Cable::onDraw(){
    std::vector<Mat4> matrices;
    matrices.reserve(points.size()+1);
    //first should be identity
    matrices.push_back(Mat4());
    //main cable model
    for(CablePoint& point : points){
        Mat4& matrix=point.matrix;
        matrices.push_back(matrix);
        //Engine::renderer->drawModelWithMatrix(EngineAssets::mAxis,matrix,Mat4(),EngineAssets::tAxis);    
    }

    Engine::renderer->setUVMatrix(cableTextureMatrix);

    Engine::renderer->uploadMatrixList(matrices);
    Engine::renderer->drawModelAt(EngineAssets::mCableModel,Vec3(0.0f),Vec3(0.0f),Vec3(1.0f),EngineAssets::tCable);

    //reset the matrix. will probably restructure this to speed it up later so it doesnt reset every time
    Engine::renderer->setUVMatrix(Mat3());

    //draw cable ends
    //move these later
    //or might just change to be handled better in the future instead of using matrices
    Mat4 rotateStart={
        1.0f,0.0,0.0,0.0,
        0.0f,0.0,1.0,0.0,
        0.0f,-1.0,0.0,0.0,
        0.0f,0.0,0.0,1.0
    };
    Mat4 rotateEnd={
        -1.0f,0.0,0.0,0.0,
        0.0f,0.0,1.0,0.0,
        0.0f,1.0,0.0,0.0,
        0.0f,0.0,0.0,1.0
    };
    Engine::renderer->drawModelWithMatrix(EngineAssets::mCableEnd,points[0].matrix*rotateStart,Mat4(),EngineAssets::tRack);
    Engine::renderer->drawModelWithMatrix(EngineAssets::mCableEnd,points.back().matrix*rotateEnd,Mat4(),EngineAssets::tRack);
}

int POBJ_Cable::createPointHitbox(CablePoint& point)
{
    int hitboxID = HitboxManager::createHitboxID();
    Hitbox& hitbox = HitboxManager::hitboxes[hitboxID];
    
    const float hitboxSize = 0.5f;
    hitbox.bounds = Vec3(hitboxSize);
    
    CablePoint* pointPtr = &point;
    hitbox.mouseDown=[pointPtr]{
        pointPtr->fixed=true;
    };
    hitbox.mouseUp=[pointPtr]{
        pointPtr->fixed=false;
    };
    hitbox.onDrag=[pointPtr](Vec2 delta,Vec2 mousePos){
        const float zIntersectValue = 0.0f;
        //calculate where it intersects the given z value

        Ray mousePosRay = Engine::renderer->rayFrom(mousePos);        
        Vec3 targetPosition = mousePosRay.direction;
        
        //find the difference
        float difference = zIntersectValue-mousePosRay.origin.z;
        
        targetPosition/=targetPosition.z;
        targetPosition*=difference;

        pointPtr->position=mousePosRay.origin+targetPosition;
    };

    return hitboxID;
}

void OBJ_Module::createFrom(ModuleData *moduleData)
{
    width = moduleData->width;
    height = moduleData->height;

    mainHitboxID=HitboxManager::createHitboxID();
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.bounds=Vec3(float(width),float(height),1.0f)*0.5f;

    //TODO: FIX THIS LATERRRR
    for (const juce::var& obj : moduleData->layout){
        juce::String componentType = obj.getProperty("type", "[NONE]");
        std::cout<<componentType<<"\n";

        Object* component;
        //MOVE ALL THIS STUFF TO FUNCTIONS LATER
        if (componentType == "static mesh") {
            auto& objRef = static_cast<OBJ_Comp_Mesh&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Mesh>()));
            component = &objRef;

            int modelID = obj.getProperty("modelID",0);
            objRef.model = &moduleData->models[modelID];
        } else if (componentType == "socket") {
            auto& objRef = static_cast<OBJ_Comp_Socket&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Socket>()));
            component = &objRef;
        } else if (componentType == "input") {
            auto& objRef = static_cast<OBJ_Comp_Input&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Input>()));
            component = &objRef;
            OBJ_Comp_Input* objPtr = &objRef;

            int animationID = obj.getProperty("animationID",0);
            objRef.animation=&moduleData->animations[animationID];
            
            objRef.hitboxID=HitboxManager::createHitboxID();
            Hitbox& hitbox = HitboxManager::hitboxes[objRef.hitboxID];
            hitbox.bounds=objRef.animation->hitboxSize;

            hitbox.onDrag = [objPtr](Vec2 delta,Vec2 mousePos){
                const float sensitivity=0.005f;
                objPtr->values+=delta*Vec2(1.0f,-1.0f)*sensitivity;
                objPtr->values.x=std::max(objPtr->values.x,0.0f);
                objPtr->values.y=std::max(objPtr->values.y,0.0f);
                objPtr->values.x=std::min(objPtr->values.x,1.0f);
                objPtr->values.y=std::min(objPtr->values.y,1.0f);
            };
        }else{
            continue;
        }

        //read the values like position and stuff
        Vec3 vPosition = readVec3FromObj(obj["position"]);
        Vec3 vRotation = readVec3FromObj(obj["rotation"]);
        Vec3 vScale = readVec3FromObj(obj["scale"], Vec3(1.0f));

        component->setTransform(vPosition,vRotation,vScale);

        children.push_back(component);
    }
}
void OBJ_Module::onTransformChange(){
    //update the hitbox position
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.position=transform.position-(hitbox.bounds*Vec3{-1.0f,-1.0f,1.0f});
}

void OBJ_Comp_Mesh::onDraw()
{
    if (!model) return;
    Engine::renderer->drawModelWithMatrix(*model,matrix,normalMatrix,model->textureID);
}
void OBJ_Comp_Socket::onDraw()
{
    Engine::renderer->drawModelWithMatrix(EngineAssets::mCablePort,matrix,normalMatrix,EngineAssets::tRack);
}
void OBJ_Comp_Input::onDraw()
{
    if(!animation) return;
    animation->draw(values,*Engine::renderer,matrix);
}
void OBJ_Comp_Input::onTransformChange()
{
    auto& hitbox = HitboxManager::hitboxes[hitboxID];
    hitbox.position=matrix.getTranslation();
}