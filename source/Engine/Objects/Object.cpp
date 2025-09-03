#include "Object.h"

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
    //RIGHT NOW DELETED OBJECTS ARENT FULLY GONE
    if(deleted) return;

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

void Object::deleteObj(){
    deleted=true;
}
