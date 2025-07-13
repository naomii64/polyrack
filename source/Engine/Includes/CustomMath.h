#pragma once
namespace Math{
    inline float lerp(float a,float b,float i){
        return ((b-a)*i)+a;
    }
}