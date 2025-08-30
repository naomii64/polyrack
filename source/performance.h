#pragma once
#include <chrono>
#include <iostream>

#include "Engine/Renderer.h"
#include "Engine/EngineAssets.h"

#include "Engine/Includes/CustomMath.h"

class Performance{
public:
    struct timeline{
        static constexpr int size = 100;

        int values[size];
        void push(long long value){
            std::rotate(values,values+1,values+size);
            values[0] = value;
        }
        int average(int count){
            if(count<=0) return 0;
            const int usedCount = std::min(count,size);
            return std::accumulate(values,values+usedCount,0)/usedCount;
        }
    };

    inline static struct timer{
        void reset(){ startTime = std::chrono::high_resolution_clock::now(); }

        long long getTime(){
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime-startTime);
            return time.count();
        }

        private:
            std::chrono::steady_clock::time_point startTime;
    };
    inline static struct categorySettings{
        Vec4f color={1.0f,1.0f,1.0f,1.0f};
        long long timeStored=0;
        Performance::timer timerObject;
        Performance::timeline timeline;
        inline void startTimer(){
            timerObject.reset();
        }
        inline void stopTimer(){
            timeStored+=timerObject.getTime();
        }
        inline void storeTime(){
            timeline.push(timeStored);
        }
    };

    inline static constexpr int categoryCount = 4;
    enum category{
        PHYSICS,    
        RENDER_3D,
        RENDER_UI,
        RENDER_DEBUG    //contains hitboxes and the performance menu
    };
    inline static Performance::categorySettings categories[categoryCount];
    inline static void init(){
        categories[category::PHYSICS].color = Vec4f(1.0f,187.0f/255.0f,53.0f/255.0f,1.0f);
        categories[category::RENDER_3D].color = Vec4f(103.0f/255.0f, 1.0f, 43.0f/255.0f, 1.0f);
        categories[category::RENDER_UI].color = Vec4f(25.f,121.f,255.f,255.f)/255.f;
        categories[category::RENDER_DEBUG].color = Vec4f(1.0f,0.0f,1.0f,1.0f);
        
    }

    //actual static class stuff
    inline static Performance::timer frameTimer;

    inline static Performance::timeline mainFrameRateTimeline;

    inline static constexpr int referenceFPS = 144;
    inline static constexpr long long referenceNanoSeconds = 1'000'000'000LL / referenceFPS;

    inline static void startTimer(category timerID){
        categories[timerID].startTimer();
    }
    inline static void stopTimer(category timerID){
        categories[timerID].stopTimer();
    }

    inline static void frameStart(){
        frameTimer.reset();
        for(size_t i=0;i<categoryCount;i++){
            categories[i].timeStored=0;
        }
    }

    inline static void frameEnd(){
        long long currentFrameTime = frameTimer.getTime();
        mainFrameRateTimeline.push(currentFrameTime);
    }
    //might move this later but idk
    inline static void drawRect(Renderer* renderer,Vec2<int> position, Vec2<int> size,Vec4f color){
        Vec3f rectPos(0.0f);
        rectPos.x=float(position.x);
        rectPos.y=float(position.y);
        Vec3f rectScale(1.0f);
        rectScale.x=float(size.x);
        rectScale.y=float(size.y);
        
        renderer->drawModelAt(EngineAssets::mTestSquare,rectPos,Vec3f(0.0f),rectScale,EngineAssets::tBlank,color);
    }
    inline static void drawTimline(Performance::timeline& timeline){
        const Vec2f offset(0.0f,-1.0f);
        const Vec2f boundsMin=Vec2f(0.0f)+offset;
        const Vec2f boundsMax=Vec2f(1.0f)+offset;
        

        juce::gl::glBegin(juce::gl::GL_LINE_STRIP);
        for(int i=0;i<timeline.size;i++){
            auto& currentValue = timeline.values[i];

            Vec2f percentagePos(0.0f);
            percentagePos.y = float(currentValue)/float(referenceNanoSeconds);
            percentagePos.x = float(i)/(timeline.size-1);

            Vec2f finalPos(0.0f);
            finalPos.x=Math::lerp(boundsMin.x,boundsMax.x ,percentagePos.x);
            finalPos.y=Math::lerp(boundsMin.y,boundsMax.y ,percentagePos.y);

            juce::gl::glVertex2f(finalPos.x, finalPos.y);
        }
        juce::gl::glEnd();
    }
    inline static void drawFilledArc(Vec2f position,float radius,float radiansA, float radiansB,unsigned int resolution=10){
            const float startRad = std::max(radiansA,radiansB);
            const float endRad = std::min(radiansA,radiansB);
            
            juce::gl::glBegin(juce::gl::GL_TRIANGLE_FAN);
                juce::gl::glVertex2f(position.x, position.y);
                for(int i=0;i<=resolution;i++){
                    const float percentage = float(i)/float(resolution);
                    const float radians = Math::lerp(startRad,endRad,percentage);
                    Vec2f point(std::sin(radians),std::cos(radians));
                    point*=radius;
                    point+=position;
                    juce::gl::glVertex2f(point.x,point.y);
                }


            juce::gl::glEnd();
    }

    inline static void drawPerformanceData(Renderer* renderer){
        const Vec2f boundsMin(0.0f);
        const Vec2f boundsMax(250.0f);
        const Vec2f diff=boundsMax-boundsMin;
        renderer->drawModelAt(EngineAssets::mTestSquare,Vec3f(0.0f,float(renderer->getHeight())-diff.y,0.0f),Vec3f(0.0f),Vec3f(diff.x,diff.y,1.0f),EngineAssets::tBlank,Vec4f(0.0f,0.0f,0.0f,1.0f));
        renderer->setDrawTint(Vec4f(1.0f));
        
        //texCoord in center
        juce::gl::glDisableVertexAttribArray(3);
        juce::gl::glVertexAttrib2f(3, 0.5f, 0.5f);

        juce::gl::glBegin(juce::gl::GL_LINE_LOOP);
            juce::gl::glVertex2f(1.0f, 0.0f);
            juce::gl::glVertex2f(0.0f, 0.0f);
            juce::gl::glVertex2f(0.0f, -1.0f);
            juce::gl::glVertex2f(1.0f, -1.0f);
        juce::gl::glEnd();

        drawTimline(mainFrameRateTimeline);
        for(size_t i=0;i<categoryCount;i++){
            Performance::categorySettings& category = categories[i];
            category.storeTime();
            renderer->setDrawTint(category.color);
            drawTimline(category.timeline);
        }

        //now draw the pie chart
        float currentRadians=0.0f;
        constexpr int smoothing=10;
        const int totalTime = mainFrameRateTimeline.average(smoothing);
        for(size_t i=0;i<categoryCount;i++){
            Performance::categorySettings& category = categories[i];
            const int time = category.timeline.average(smoothing);
            float percentage = float(time)/float(totalTime);
            float radians=percentage*juce::MathConstants<float>::pi*2;

            renderer->setDrawTint(category.color);
            drawFilledArc(Vec2f(0.5f,0.5f),0.25f,currentRadians,currentRadians+radians);            
            currentRadians+=radians;
        }
    }
};