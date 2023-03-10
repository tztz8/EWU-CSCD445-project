// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include "cudaMain.cuh"

// Called when setting things up before graphs loop
void cudaMainInitialize() {
    SPDLOG_INFO("Initialize Cuda");

}

// Called for every frame
void cudaMainUpdate(double time) {
    bool error = false;
    if (error) {
        // Avoid logging messages
        SPDLOG_ERROR("Problem Happen");
    }
}

// Called as the program close
void cudaMainCleanUp() {
    SPDLOG_INFO("Clean Up Cuda");

    int someNumber = 5;
    SPDLOG_DEBUG(spdlog::fmt_lib::format("something is {}", someNumber));
}
