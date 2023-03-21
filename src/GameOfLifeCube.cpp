#include <sys/time.h>
#include <stdlib.h>
#include "GameOfLifeCube.h"
#include "cudaMain.cuh"
#include "cudaInfo.cuh"
#include "imgui.h"
#include "OpenGLHelperMethods.h"
#include "GameOfLifeLogic.h"

void GameOfLifeCube::cubeCreate() {
    SPDLOG_INFO("Initialize GameOfLife Cube");
    float side = 0.5f;

    GLfloat cube_vertices[] = {side, side, side, 1.0f,  // v0,v1,v2,v3 (front)
                               -side, side, side, 1.0f,
                               -side, -side, side, 1.0f,
                               side, -side, side, 1.0f,
                               side, side, side, 1.0f,  // v0,v3,v4,v5 (right)
                               side, -side, side, 1.0f,
                               side, -side, -side, 1.0f,
                               side, side, -side, 1.0f,
                               side, side, side, 1.0f,  // v0,v5,v6,v1 (top)
                               side, side, -side, 1.0f,
                               -side, side, -side, 1.0f,
                               -side, side, side, 1.0f,
                               -side, side, side, 1.0f, // v1,v6,v7,v2 (left)
                               -side, side, -side, 1.0f,
                               -side, -side, -side, 1.0f,
                               -side, -side, side, 1.0f,
                               -side, -side, -side, 1.0f,// v7,v4,v3,v2 (bottom)
                               side, -side, -side, 1.0f,
                               side, -side, side, 1.0f,
                               -side, -side, side, 1.0f,
                               side, -side, -side, 1.0f,// v4,v7,v6,v5 (back)
                               -side, -side, -side, 1.0f,
                               -side, side, -side, 1.0f,
                               side, side, -side, 1.0f};


    GLushort cube_indices[] = {0, 1, 2, 2, 3, 0,      // front
                               4, 5, 6, 6, 7, 4,      // right
                               8, 9, 10, 10, 11, 8,      // top
                               12, 13, 14, 14, 15, 12,      // left
                               16, 17, 18, 18, 19, 16,      // bottom
                               20, 21, 22, 22, 23, 20};    // back

    GLfloat cube_normals[6][4][3] = {
            // Front
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            // Right
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            // Top
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            // Left
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            // Bottom
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            // Back
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f
    };

    GLfloat cube_textures[] = {
//                                    0.0f, 1.0f,  // v0,v1,v2,v3 (front)
//                                1.0f, 1.0f,   // 0 1
//                                1.0, 0.0f,   // 0 1/6
//                                0.0f, 0.0f,
                                0.0f, 1.0f,  // v0,v1,v2,v3 (front)
                                1.0f/6.0f, 1.0f,   // 0 1
                                1.0f/6.0f, 0.0f,   // 0 1/6
                                0.0f, 0.0f,

                                4.0f/6.0f, 1.0f, // v1,v6,v7,v2 (left)
                                4.0f/6.0f, 0.0f,
                                3.0f/6.0f, 0.0f,
                                3.0f/6.0f, 1.0f,

                                1.0f, 1.0f, // v7,v4,v3,v2 (bottom)
                                1.0f, 0.0f,
                                5.0f/6.0f, 0.0f,
                                5.0f/6.0f, 1.0f,

                                1.0f/6.0f, 1.0f,
                                2.0f/6.0f, 1.0f, // v0,v3,v4,v5 (right)
                                2.0f/6.0f, 0.0f,
                                1.0f/6.0f, 0.0f,

                                5.0f/6.0f, 0.0f,
                                4.0f/6.0f, 0.0f,
                                4.0f/6.0f, 1.0f,
                                5.0f/6.0f, 1.0f, // v0,v5,v6,v1 (top)

                                3.0f/6.0f, 0.0f, // v4,v7,v6,v5 (back)
                                2.0f/6.0f, 0.0f,
                                2.0f/6.0f, 1.0f,
                                3.0f/6.0f, 1.0f };

    glGenVertexArrays(1, &this->cube_vao);
    glBindVertexArray(this->cube_vao);

    unsigned int handle[4];
    glGenBuffers(4, handle);

    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint) 1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);  // Vertex Normals

    glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_textures), cube_textures, GL_STATIC_DRAW);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);  // Vertex texture

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GameOfLifeCube::cpuCreate(int size) {
    SPDLOG_INFO("Initialize GameOfLife CPU code");
    // TODO: remove using for debugging
//    this->cpuTexID = loadTexture("res/textures/Test/testImage.png");

    this->row = size;
    this->column = size * 6;

    // make 2d grid
    this->board = (int *) calloc(this->row * this->column, sizeof(int));
    this->pboard = (int *) calloc(this->row * this->column, sizeof(int));
    // RGBA -> 4
    this->imgBoard = (GLuint *) calloc(this->row * this->column, sizeof(GLuint));

    // TODO: make a glider
//    this->board[(1 * column )+ 3] = 255;
//    this->board[(2 * column )+ 3] = 255;
//    this->board[(3 * column )+ 3] = 255;
    for (int i = 0; i < column; ++i) {
        this->board[(3 * column )+ i] = 1;
    }


    // make image
    genCPUTexImg(false);
}

void GameOfLifeCube::create() {
    SPDLOG_INFO("Making Game Of Life Cube");
//    this->worldSize = 1306;
    this->worldSize = 36;
    this->cubeCreate();
    this->cpuCreate(this->worldSize);
    if (!checkCuda()) {
        SPDLOG_ERROR("Cuda Not Available");
        this->havaCuda = false;
        this->usingCuda = false;
    } else {
        this->havaCuda = true;
        this->usingCuda = true;
        cudaMainInitialize(this->worldSize);
        // time the kernel launches using CUDA events
        cudaEventCreate(&this->launch_begin);
        cudaEventCreate(&this->launch_end);
        this->cudaAvgTime = 0.0;
        this->qtyCuda = 0;
    }
    this->run = true;
    this->speed = 5.0f;
    this->timeStart = 0;
}

void GameOfLifeCube::draw() {
    if (usingCuda) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cudaTexID);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->cpuTexID);
    }
    glBindVertexArray(this->cube_vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
}

double currentTime() {

    struct timeval now;
    gettimeofday(&now, NULL);

    return now.tv_sec + now.tv_usec / 1000000.0;
}

void GameOfLifeCube::update(GLfloat deltaTime, double time) {
    if (this->run) {
        if (time- this->timeStart > speed) {
            this->timeStart = time;

            if (this->havaCuda) {
//                cudaEventRecord(launch_begin, 0);
                double now, then;
                now = currentTime();
                cudaMainUpdate();
                then = currentTime();
                this->cudaTime = static_cast<float>(then - now);
//                cudaEventRecord(launch_end, 0);
//                cudaEventSynchronize(launch_end);
//                cudaEventElapsedTime(&this->cudaTime, launch_begin, launch_end);
//                this->cudaTime = this->cudaTime / 1000;
                this->qtyCuda++;
//                this->cudaAvgTime += (this->cudaTime - this->cudaAvgTime) / static_cast<double>(qtyCuda);
                this->cudaAvgTime += ((then - now) - this->cudaAvgTime) / static_cast<double>(qtyCuda);
            }

            double now, then;
            now = currentTime();
            this->cpuUpdate(time);
            then = currentTime();
            this->cpuTime = static_cast<float>(then - now);
            this->qtyCpu++;
            this->cpuAvgTime += ((then - now) - this->cpuAvgTime) / static_cast<double>(qtyCpu);
        }
    }
}

void GameOfLifeCube::ImGUIHeader() {
    if (ImGui::CollapsingHeader("Game Of Life")) {
        if (ImGui::SliderInt("Size Of World (Not Setup Yet)", &this->worldSize, 6, (GL_MAX_TEXTURE_SIZE/6))) {
            // TODO: update world size in GPU and CPU
        }
        ImGui::SliderFloat("Speed of Game of Life (sec)",  &this->speed, 0.001f, 15.0f);
        ImGui::Checkbox("Run game of life", &this->run);
        if (this->havaCuda) {
            ImGui::Checkbox("Use CUDA output instead of CPU output", &this->usingCuda);
        } else {
            ImGui::Text("Cuda not avable");
        }
        ImGui::Text("CPU        Time %f (ms)", this->cpuTime * 1000);
        ImGui::Text("GPU (CUDA) Time %f (ms)", this->cudaTime * 1000);
        ImGui::Text("The speedup(SerialTimeCost / CudaTimeCost) when using GPU is %lf", this->cpuTime / this->cudaTime);
        ImGui::Text("CPU        Avg Time %f (ms)", this->cpuAvgTime * 1000);
        ImGui::Text("GPU (CUDA) Avg Time %f (ms)", this->cudaAvgTime * 1000);
        ImGui::Text("The avg speedup(SerialTimeCost / CudaTimeCost) when using GPU is %lf", this->cpuAvgTime / this->cudaAvgTime);
        ImGui::Text("CPU State: %ld", this->qtyCpu);
        ImGui::Text("GPU State: %ld", this->qtyCuda);
    }
}

void GameOfLifeCube::cleanUp() {
    if (this->havaCuda) {
        cudaMainCleanUp();
//        cudaEventDestroy(this->launch_begin);
//        cudaEventDestroy(this->launch_end);
    }
    free(this->board);
    free(this->pboard);
    free(this->imgBoard);
}

void GameOfLifeCube::cpuUpdate(double time) {
    // TODO: CPU code
    runlife(this->board, this->pboard, this->row, this->column);
    // swap boards
    int *tempBoard = this->pboard;
    this->pboard = this->board;
    this->board = tempBoard;

    // static GLubyte checkImage[checkImageHeight][checkImageWidth][4]

    for(size_t x = 0; x < this->column; x++) {

        for(size_t y = 0; y < this->row; y++) {
            this->imgBoard[x + y * this->column] = UINT32_MAX * (this->board[x + y * this->column] && true);

        }
    }

//    for (int z = 0; z < this->column; ++z) {
//        for (int y = 0; y < this->row; ++y) {
//            for (int x = 0; x < 4; ++x) {
//                if (x == 0) { // red
//                    this->imgBoard[(z * this->row * this->column) + (y * this->column) + x] = this->board[(z * this->column) + y] * INT_MAX;
//                } else {
//                    this->imgBoard[(z * this->row * this->column) + (y * this->column) + x] = 0;
//                }
//            }
//        }
//    }

    // TODO: update image
    genCPUTexImg(true);
    SPDLOG_INFO(spdlog::fmt_lib::format("State {}", this->qtyCpu));
    printboard(this->board, this->row, this->column);

}

void GameOfLifeCube::genCPUTexImg(bool freeOldImg) {
//    if (freeOldImg) {
//        glDeleteTextures(1, &this->cpuTexID);
//    }
    glGenTextures(1, &this->cpuTexID);
    glBindTexture(GL_TEXTURE_2D, this->cpuTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 this->column, this->row, 0,
                 GL_RGBA,GL_UNSIGNED_INT_8_8_8_8, this->imgBoard);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, this->cpuTexID);
}
