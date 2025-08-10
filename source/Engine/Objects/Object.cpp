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

void Object::addChild(Object *child)
{
    child->parent=this;
    children.push_back(child);
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

        if(pointA.fixed) continue;

        pointA.rotation = pointA.position.lookAt(pointB.position);
    }
    if(!points.back().fixed) points.back().rotation=points[points.size()-2].rotation;
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
    points[0].isFirst=true;

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
        0.0f,0.0,1.0,0.0,
        -1.0f,0.0,0.0,0.0,
        0.0f,-1.0,0.0,0.0,
        0.0f,0.0,0.0,1.0
    };
    Mat4 rotateEnd={
        0.0f,0.0,1.0,0.0,
        1.0f,0.0,0.0,0.0,
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
        pointPtr->detachFromSocket();
    };
    hitbox.mouseUp=[pointPtr]{
        pointPtr->fixed=false;
        if(pointPtr->connectedSocket){
            pointPtr->connectToSocket();
        }
    };
    hitbox.onDrag=[pointPtr](Vec2 delta,Vec2 mousePos){
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

            Vec3 difference = pointPtr->position-socket->worldTranslation;
            //probably dont even need this to be squared yet
            Vec3 differenceSquared = difference*difference;
            float distanceSquared = differenceSquared.x+differenceSquared.y+differenceSquared.z;
            if((!closestSocket)||(distanceSquared<closestDistanceSquared)){
                closestSocket=socket;
                closestDistanceSquared=distanceSquared;
            }
        }   
        if(closestSocket){
            if(closestDistanceSquared<lookAtDistnaceSquared){
                pointPtr->lookAtSocket(*closestSocket);
            }
            if(closestDistanceSquared<connectionDistnaceSquared){
                pointPtr->connectedSocket=closestSocket;
            }
        }    
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
    Hitbox* hbptr=&hitbox;
    hitbox.onDrag=[this,hbptr](Vec2 delta, Vec2 mousePos){
        Vec2 prevPos = mousePos-delta;

        const float zValue = 0.0f;

        Vec3 pointA = Engine::screenPosToZPlane(prevPos,zValue);
        Vec3 pointB = Engine::screenPosToZPlane(mousePos,zValue);

        Vec3 delta3D = pointB-pointA;
        this->setPosition(this->transform.position+delta3D);
        
        //std::cout<<"dragging: "<< delta3D.toString()<<"\n";
    };

    //TODO: FIX THIS LATERRRR
    for (const juce::var& obj : moduleData->layout){
        juce::String componentType = obj.getProperty("type", "[NONE]");
        std::cout<<componentType<<"\n";

        OBJ_Component* component;
        //MOVE ALL THIS STUFF TO FUNCTIONS LATER
        if (componentType == "static mesh") {
            component = &static_cast<OBJ_Comp_Mesh&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Mesh>()));
        } else if (componentType == "socket") {
            component = &static_cast<OBJ_Comp_Socket&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Socket>()));
        } else if (componentType == "input") {
            component = &static_cast<OBJ_Comp_Input&>(*Engine::objects.emplace_back(std::make_unique<OBJ_Comp_Input>()));
        }else{
            continue;
        }

        //read the values like position and stuff
        Vec3 vPosition = readVec3FromObj(obj["position"]);
        Vec3 vRotation = readVec3FromObj(obj["rotation"]);
        Vec3 vScale = readVec3FromObj(obj["scale"], Vec3(1.0f));

        component->setTransform(vPosition,vRotation,vScale);
        component->initComponent(obj,*moduleData);

        addChild(component);
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

    hitbox.onDrag = [this](Vec2 delta,Vec2 mousePos){
        const float sensitivity=0.005f;
        this->values+=delta*Vec2(1.0f,-1.0f)*sensitivity;
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

    fixed=true;
    //the rubber part of the plug is 1.0 and the socket rim is 0.2
    Vec3 positionOffset = {0.0f,0.0f,1.2f};
    position=connectedSocket->worldTranslation+positionOffset;
    lookAtSocket(*connectedSocket);
    connectedSocket->connectedPoint=this;
}
void CablePoint::detachFromSocket()
{
    if(connectedSocket){
        connectedSocket->connectedPoint=nullptr;
        connectedSocket=nullptr;
    }
}
void CablePoint::lookAtSocket(OBJ_Comp_Socket &socket)
{
    if(isFirst) rotation=socket.worldTranslation.lookAt(position);
    else rotation=position.lookAt(socket.worldTranslation);
}
