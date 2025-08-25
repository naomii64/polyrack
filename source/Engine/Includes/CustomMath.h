#pragma once
namespace Math{
    template<typename T>
    inline T lerp(T a,T b,float i){
        return ((b-a)*i)+a;
    }
}