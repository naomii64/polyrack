#pragma once
#include "Object.h"
#include "../Engine.h"
#include "../EngineAssets.h"
#include "../Hitboxes.h"
#include "../ModuleManager.h"

#pragma warning(disable : 4100)

Object::Object(){}
Object::~Object(){}
void Object::updateMatrix(){
    if (parent){
        matrix = parent->matrix * transform.getMatrix();
    }else{
        matrix = transform.getMatrix();
    }

    //maybe change this to only calculate for classes that need it later
    worldTranslation=matrix.getTranslation();

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

void Object::setPosition(Vec3f newPosition)
{
    transform.position = newPosition;
    matrixRecalculationRequested=true;
}
void Object::setScale(Vec3f newScale)
{
    transform.scale = newScale;
    matrixRecalculationRequested=true;
}
void Object::setRotation(Vec3f newRotation)
{
    transform.rotation = newRotation;
    matrixRecalculationRequested=true;
}
void Object::setTransform(Vec3f newPosition, Vec3f newRotation, Vec3f newScale)
{
    transform.position = newPosition;
    transform.rotation = newRotation;
    transform.scale = newScale;
    matrixRecalculationRequested=true;
}

void Object::addChild(Object *child)
{
    child->parent=this;
    children.push_back(child);
}

void PhysicsObject::physicsTick(float delta) {}

//subclasses
int POBJ_Cable::variantCounter=0;
std::vector<POBJ_Cable*> POBJ_Cable::allCables;
void POBJ_Cable::applyGravityAndMomentum(float delta){
    for(CablePoint& point : points){
        if(point.fixed){
            point.previousPosition=point.position;
            continue;
        }
        Vec3f copyPos=point.position;

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

            Vec3f diff=pointA.position-pointB.position;
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
            Vec3f dir=diff/distance;

            float distanceError = distance-desiredDistance;
            Vec3f distErr=dir*distanceError;
            
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

        if(pointA.lockRotation) continue;

        pointA.rotation = pointA.position.lookAt(pointB.position);

        if(i>0){
            pointA.rotation=(pointA.rotation+points[i-1].rotation)*0.5f;
        }
    }
    if(!points.back().lockRotation) points.back().rotation=points[points.size()-2].rotation;
}
void POBJ_Cable::calculateMatrices(){
    for(CablePoint& point : points){
        //order is handled differently for cables
        Mat4f rotX = Mat4f::rotationX(point.rotation.x);
        Mat4f rotY = Mat4f::rotationY(point.rotation.y);
        Mat4f rotZ = Mat4f::rotationZ(point.rotation.z);
        Mat4f rotationMatrix = rotY * rotX * rotZ;
        point.matrix=Mat4f::translation(point.position)*rotationMatrix;

        point.inverseRotationMatrix=Mat4f::inverseRotation(point.rotation);
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
void POBJ_Cable::createCable(int pointCount,Vec3f start,Vec3f end){
    hasBeenCreated=true;
    points.resize(pointCount);
    for(int i=0;i<points.size();i++){
        float interpolationValue=float(i)/float(points.size()-1);
        Vec3f currentPointPos = Vec3f::lerp(start,end,interpolationValue);
        points[i].position=currentPointPos;
        points[i].previousPosition=currentPointPos;
    }
    points[0].isFirst=true;

    //create and assign hitboxes
    hitboxAID = createPointHitbox(points[0]);
    hitboxBID = createPointHitbox(points.back());
    
    //THIS IS PROBABLY JUST TEMPORARY
    points[0].fixed=true;
    
    //===CABLE VARIANT===//
    cableVariant=POBJ_Cable::variantCounter++ % POBJ_Cable::VARIANT_COUNT;

    allCables.push_back(this);
}
void POBJ_Cable::drawAll(){
    Engine::renderer->setDrawTint(Vec4f(1.0f));
    //draw ends of cables
    Engine::renderer->loadModel(EngineAssets::mCableEnd);
    Engine::renderer->setDrawTexture(EngineAssets::tRack);

    for(auto* cablePtr : allCables){
        POBJ_Cable& cable = *cablePtr;

        Mat4f rotateStart={
            0.0f,0.0,1.0,0.0,
            -1.0f,0.0,0.0,0.0,
            0.0f,-1.0,0.0,0.0,
            0.0f,0.0,0.0,1.0
        };
        Mat4f rotateEnd={
            0.0f,0.0,1.0,0.0,
            1.0f,0.0,0.0,0.0,
            0.0f,1.0,0.0,0.0,
            0.0f,0.0,0.0,1.0
        };
    
        CablePoint& start = cable.points[0];
        CablePoint& end = cable.points.back();

        Engine::renderer->loadModelMatrix(start.matrix*rotateStart);
        Engine::renderer->drawLoadedModel();
        Engine::renderer->loadModelMatrix(end.matrix*rotateEnd);
        Engine::renderer->drawLoadedModel();
    }
    Engine::renderer->unloadModel();

    //draw the actual cable part
    Engine::renderer->loadModel(EngineAssets::mCableModel);
    Engine::renderer->setDrawTexture(EngineAssets::tCable);
    Engine::renderer->loadModelMatrix(Mat4f());
    
    std::array<Mat4f,17> matrices;

    for(auto* cablePtr : allCables){
        POBJ_Cable& cable = *cablePtr;
        
        size_t currentMatrix=1;
        for(CablePoint& point : cable.points){
            matrices[currentMatrix]=point.matrix;
            currentMatrix++;
        }
        Engine::renderer->setUVMatrix(POBJ_Cable::cableVariantTextures[cable.cableVariant]);

        Engine::renderer->uploadMatrixList(matrices);
        
        Engine::renderer->drawLoadedModel();
    }
    Engine::renderer->unloadModel();
    Engine::renderer->setUVMatrix(Mat3f());
}

int POBJ_Cable::createPointHitbox(CablePoint& point)
{
    int hitboxID = HitboxManager::createHitboxID();
    Hitbox& hitbox = HitboxManager::hitboxes[hitboxID];
    
    const float hitboxSize = 0.5f;
    hitbox.bounds = Vec3f(hitboxSize);
    
    CablePoint* pointPtr = &point;
    hitbox.mouseDown=[pointPtr]{
        pointPtr->fixed=true;
        pointPtr->detachFromSocket();
    };
    hitbox.mouseUp=[pointPtr]{
        pointPtr->fixed=false;
        pointPtr->lockRotation=false;
        if(pointPtr->connectedSocket){
            pointPtr->connectToSocket();
        }
    };
    hitbox.onDrag=[pointPtr](Vec2f delta,Vec2f mousePos){
        //put this as a constant somewhere since its the same as the distance the plugs should be at
        const float zIntersectValue = 1.2f;
        pointPtr->position=Engine::screenPosToZPlane(mousePos,zIntersectValue);

        //find the nearest socket
        constexpr float connectionDistance = 2.0f;
        constexpr float lookAtDistance = 3.0f;

        constexpr float lookAtDistnaceSquared=lookAtDistance*lookAtDistance;
        constexpr float connectionDistnaceSquared=connectionDistance*connectionDistance;
        float closestDistanceSquared=0.0f;
        OBJ_Comp_Socket* closestSocket = nullptr;
        pointPtr->connectedSocket=nullptr;

        for(OBJ_Comp_Socket* socket : Engine::sockets){
            //ignore sockets that are taken
            if(socket->connectedPoint) continue;

            Vec3f difference = pointPtr->position-socket->worldTranslation;
            //probably dont even need this to be squared yet
            Vec3f differenceSquared = difference*difference;
            float distanceSquared = differenceSquared.x+differenceSquared.y+differenceSquared.z;
            if((!closestSocket)||(distanceSquared<closestDistanceSquared)){
                closestSocket=socket;
                closestDistanceSquared=distanceSquared;
            }
        }   

        pointPtr->lockRotation=false;
        if(closestSocket){
            const bool canLookAt = closestDistanceSquared<lookAtDistnaceSquared;
            const bool canConnect = closestDistanceSquared<connectionDistnaceSquared;
            
            if(canLookAt){
                pointPtr->lockRotation=true;
                pointPtr->lookAtSocket(*closestSocket);
            }
            if(canConnect){
                pointPtr->connectedSocket=closestSocket;
            }
        }    
    };

    return hitboxID;
}
#include "../../UI/UI.h"

void OBJ_Module::createFrom(ModuleData *moduleData)
{
    width = moduleData->width;
    height = moduleData->height;

    mainHitboxID=HitboxManager::createHitboxID();
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.bounds=Vec3f(float(width),float(height),1.0f)*0.5f;
    Hitbox* hbptr=&hitbox;
    hitbox.onDrag=[this,hbptr](Vec2f delta, Vec2f mousePos){
        Vec2f prevPos = mousePos-delta;

        const float zValue = 0.0f;

        Vec3f pointA = Engine::screenPosToZPlane(prevPos,zValue);
        Vec3f pointB = Engine::screenPosToZPlane(mousePos,zValue);

        Vec3f delta3D = pointB-pointA;
        this->truePosition+=delta3D;
        this->setPosition(Vec3f::round(this->truePosition));
        
        //std::cout<<"dragging: "<< delta3D.toString()<<"\n";
    };
    hitbox.rightClick = [this]{
        UI::openModuleContext(this);        
    };

    //TODO: FIX THIS LATERRRR
    for (const juce::var& obj : moduleData->layout){
        juce::String componentType = obj.getProperty("type", "[NONE]");
        //std::cout<<componentType<<"\n";

        OBJ_Component* component;
        //MOVE ALL THIS STUFF TO FUNCTIONS LATER
        if (componentType == "static mesh") {
            component = &Engine::createObject<OBJ_Comp_Mesh>();
        } else if (componentType == "socket") {
            component = &Engine::createObject<OBJ_Comp_Socket>();
        } else if (componentType == "input") {
            component = &Engine::createObject<OBJ_Comp_Input>();
        }else{
            continue;
        }

        //read the values like position and stuff
        Vec3f vPosition = readVec3FromObj(obj["position"]);
        Vec3f vRotation = readVec3FromObj(obj["rotation"]);
        Vec3f vScale = readVec3FromObj(obj["scale"], Vec3f(1.0f));

        component->setTransform(vPosition,vRotation,vScale);
        component->initComponent(obj,*moduleData);

        addChild(component);
    }
}


void Object::deleteObj(){
    active=false;
}
void OBJ_Module::deleteObj()
{
    //dettatch all cables
    for (Object* child : children) {
            child->deleteObj();
    }
    HitboxManager::deleteHitbox(mainHitboxID);
    active=false;
}
void OBJ_Comp_Socket::deleteObj()
{
    //detatch any connected cables
    if(connectedPoint){
        //might need to change where this is idk
        connectedPoint->lockRotation=false;
        connectedPoint->fixed=false;
        connectedPoint->detachFromSocket();
    }
    active=false;
}
void OBJ_Comp_Input::deleteObj()
{
    HitboxManager::deleteHitbox(hitboxID);
    active=false;
}

void OBJ_Module::onTransformChange()
{
    //update the hitbox position
    auto& hitbox = HitboxManager::hitboxes[mainHitboxID];
    hitbox.position=transform.position-(hitbox.bounds*Vec3f{-1.0f,-1.0f,1.0f});
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
    hitbox.position=worldTranslation;
}
void OBJ_Comp_Socket::onTransformChange()
{
    if(!connectedPoint) return;
    connectedPoint->connectToSocket();
}

void OBJ_Component::initComponent(const juce::var &fromObject,ModuleData& moduleData){}
void OBJ_Comp_Input::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    int animationID = fromObject.getProperty("animationID",0);
    animation=&moduleData.animations[animationID];
            
    hitboxID=HitboxManager::createHitboxID();
    Hitbox& hitbox = HitboxManager::hitboxes[hitboxID];
    hitbox.bounds=animation->hitboxSize;

    hitbox.onDrag = [this](Vec2f delta,Vec2f mousePos){
        const float sensitivity=0.005f;
        this->values+=delta*Vec2f(1.0f,-1.0f)*sensitivity;
        this->values.x=std::max(this->values.x,0.0f);
        this->values.y=std::max(this->values.y,0.0f);
        this->values.x=std::min(this->values.x,1.0f);
        this->values.y=std::min(this->values.y,1.0f);
    };
}
void OBJ_Comp_Socket::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    Engine::sockets.push_back(this);
}
void OBJ_Comp_Mesh::initComponent(const juce::var &fromObject,ModuleData& moduleData){
    int modelID = fromObject.getProperty("modelID",0);
    model = &moduleData.models[modelID];
}

void CablePoint::connectToSocket()
{
    //if the socket is taken
    if(!connectedSocket) return;
    if(!(connectedSocket->connectedPoint==this||connectedSocket->connectedPoint==nullptr)) return;

    lockRotation=true;
    fixed=true;
    //the rubber part of the plug is 1.0 and the socket rim is 0.2
    Vec3f positionOffset = {0.0f,0.0f,1.2f};
    position=connectedSocket->worldTranslation+positionOffset;
    lookAtSocket(*connectedSocket);
    connectedSocket->connectedPoint=this;
}
void CablePoint::detachFromSocket()
{
    if(!connectedSocket) return;
    connectedSocket->connectedPoint=nullptr;
    connectedSocket=nullptr;
}
void CablePoint::lookAtSocket(OBJ_Comp_Socket &socket)
{
    if(isFirst) rotation=socket.worldTranslation.lookAt(position);
    else rotation=position.lookAt(socket.worldTranslation);
}
