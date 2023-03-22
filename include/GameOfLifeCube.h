#ifndef EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H
#define EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H

// Logger Lib
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <device_types.h>
#include <cuda_runtime_api.h>

/**
 * Game of Life Cube
 */
class GameOfLifeCube {
private:
    // Flag if update does anything
    bool run;
    // The min time between updates
    float speed;
    /*
     * Time start (for check how often to update)
     * The time of last run of update
     */
    double timeStart;

    // Size of the world
    int worldSize = 16;// 1350

    // Flag if we have cuda
    bool havaCuda;
    // If we are using cuda
    bool usingCuda;

    // Cuda model data
    GLuint cube_vao;

    // Texture ID
    GLuint cudaTexID;

    // Texture ID
    GLuint cpuTexID;

    /**
     * From the board make a pixle array that gl can use
     * @param board the board to read
     * @param outputImgBoard the pixel array to save to
     */
    void genImg(int * board, GLuint *outputImgBoard);
    /**
     * Gen a OpenGL Texture (tex) from pixel data
     * @param texId the place to save tex to
     * @param freeOldImg do we need to clean up old texture at texID?
     * @param inputImgBoard the pixel data
     */
    void genTexImg(GLuint *texId, bool freeOldImg, GLuint *inputImgBoard);

    // Texture image to help know what side we are look at
    GLuint helpTexID;

    void cubeCreate();
    void cpuCreate(int size);
    cudaEvent_t launch_begin, launch_end;

    // Time it took last time it run
    float cudaTime;
    // The Continuous average time of the gpu
    double cudaAvgTime;
    // Number of times gpu update is called
    long qtyCuda;
    int * cudaBoard;

    /**
     * Update cpu board
     * @param time na
     */
    void cpuUpdate(double time);
    // Time it took last time it run
    float cpuTime;
    // The Continuous average time of the cpu
    double cpuAvgTime;
    // Number of times cpu update is called
    long qtyCpu;

    // Num of rows (curren world size)
    int row;
    // num of columns (row * 6) (6 for number of faces of a cube)
    int column;
    // Current board
    int *board;
    // Next board (often has last board)
    int *pboard;
    // GL Pixle data
    GLuint *imgBoard;
public:
    // Flag for using help texture instead game of life world
    bool useHelpImg;
    /**
     * create game of life cube
     */
    void create();
    /**
     * GUI code for game of life
     */
    void ImGUIHeader();
    /**
     * Draw game of life cube
     */
    void draw();
    /**
     * Check and update the game of life world
     * @param deltaTime na
     * @param time current time
     */
    void update(GLfloat deltaTime, double time);
    /**
     * Clean up the game of life world
     */
    void cleanUp();
};

// TODO: remove
extern int DEBUG_GLOBAL_ROWS;
// TODO: remove
extern int DEBUG_GLOBAL_COLS;
#endif //EWU_CSCD445_PROJECT_GAMEOFLIFECUBE_H
