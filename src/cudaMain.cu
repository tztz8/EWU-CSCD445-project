// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include "cudaMain.cuh"

// Called when setting things up before graphs loop
__host__ void cudaMainInitialize() {
    SPDLOG_INFO("Initialize Cuda");

}

// Called for every frame
__host__ void cudaMainUpdate(double time) {
    bool error = false;
    if (error) {
        // Avoid logging messages
        SPDLOG_ERROR("Problem Happen");
    }
}

// Called as the program close
__host__ void cudaMainCleanUp() {
    SPDLOG_INFO("Clean Up Cuda");

    int someNumber = 5;
    SPDLOG_DEBUG(spdlog::fmt_lib::format("something is {}", someNumber));
}
