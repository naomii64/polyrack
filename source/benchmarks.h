#pragma once
#include <chrono>
#include <iostream>

#include "Engine/Includes/DataTypes.h"

namespace benchmarks{

inline constexpr int tableSectionWidth=15;

inline void printFixedLengthString(std::string string,int length){
    int chars = 0;
    std::cout<<string;
    chars+=string.length();
    while(chars<length){
        std::cout<<" ";
        chars++;
    }
}
inline void printResults(std::string operationName ,size_t iterationCount, std::chrono::nanoseconds timeNanoSeconds){
    printFixedLengthString(operationName,tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString(std::to_string(iterationCount),tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString(std::to_string(timeNanoSeconds.count()),tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString(std::to_string(double(timeNanoSeconds.count())/double(iterationCount)),tableSectionWidth);
    std::cout<<"\n";    
}
inline void printTableHeader(){
    printFixedLengthString("operation:",tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString("count:",tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString("nanoseconds:",tableSectionWidth);
    std::cout<<"|";
    printFixedLengthString("nanoseconds per operation:",tableSectionWidth);
    std::cout<<"\n";    
}

template<size_t Count,typename T>
inline void Vec3Addition(){

    Vec3<T> vectors[Count];

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < Count; ++i) {
        vectors[i] += T(1);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start);

    benchmarks::printResults("Vec3 +=",Count,time);

}
template<size_t Count>
inline void Mat4Multiplication(){

}

inline void runAll(){
    printTableHeader();
    constexpr size_t count = 50;
    //Vec3Addition<count, float>();
    Mat4Multiplication<count>();
}

}