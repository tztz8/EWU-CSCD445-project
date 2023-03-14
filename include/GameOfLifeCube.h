#ifndef EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H
#define EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H

// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <device_types.h>
#include <cuda_runtime_api.h>

class GameOfLifeCube {
private:
    bool run;
    float speed;
    double timeStart;

    int worldSize;

    bool havaCuda;
    bool usingCuda;

    GLuint cube_vao;

//    // Texture ID
//    GLuint cudaTexID;

    // Texture ID
    GLuint cpuTexID;

    void cubeCreate();
    void cpuCreate(int size);
    cudaEvent_t launch_begin, launch_end;

    float cudaTime;
    double cudaAvgTime;
    long qtyCuda;

    void cpuUpdate(double time);
    float cpuTime;
    double cpuAvgTime;
    long qtyCpu;
public:
    void create();
    void ImGUIHeader();
    void draw();
    void update(GLfloat deltaTime, double time);
    void cleanUp();
};


#endif //EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H
