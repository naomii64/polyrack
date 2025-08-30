//this is where compile settings for the project will be stored
#pragma once
#define draw_hitboxes true


#define monitor_performance true
#if monitor_performance
    #define PERF_START_FLAG(FLAG) Performance::startTimer(Performance::category::FLAG);
    #define PERF_END_FLAG(FLAG) Performance::stopTimer(Performance::category::FLAG);
#else
    #define PERF_START_FLAG(FLAG)
    #define PERF_END_FLAG(FLAG)
#endif