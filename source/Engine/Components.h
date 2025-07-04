#pragma once
#include "juce_core/juce_core.h"
#include "Includes/DataTypes.h"
#include <iostream>
#include "Includes/Model.h"
#include "Renderer.h"
#include "ModuleManager.h"

struct ModuleData;

class Component{
    public:
        Vec3 position;
        Vec3 rotation;
        Vec3 scale{1.0f,1.0f,1.0f};

        virtual ~Component() = default;
        virtual void draw(Renderer& renderer) const;
        virtual void init(const juce::var& obj,ModuleData& moduleData);
};

class StaticMeshComponent : public Component{
    public:
        void draw(Renderer& renderer) const override;
        void init(const juce::var& obj,ModuleData& moduleData) override;
        Model* model;
};

class SocketComponent : public Component{
    public:
        void draw(Renderer& renderer) const override;
        bool isInput;
};

//unwrapping functions
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

inline std::unique_ptr<Component> componentFromJSONObject(const juce::var& obj,ModuleData& moduleData) {
    juce::String componentType = obj.getProperty("type", "[NONE]");
    
    std::unique_ptr<Component> component;

    if (componentType == "static mesh") {
        component = std::make_unique<StaticMeshComponent>();
    } else if (componentType == "socket") {
        std::cout << "is socket!";
        component = std::make_unique<SocketComponent>();
    } else {
        component = std::make_unique<Component>();
    }

    component->init(obj,moduleData);

    Vec3 vPosition = readVec3FromObj(obj["position"]);
    Vec3 vRotation = readVec3FromObj(obj["rotation"]);
    Vec3 vScale = readVec3FromObj(obj["scale"], Vec3(1.0f));

    component->position = vPosition;
    component->rotation = vRotation;
    component->scale = vScale;

    return component;
}