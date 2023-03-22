#include <sys/time.h>
#include <stdlib.h>
#include "GameOfLifeCube.h"
#include "cudaMain.cuh"
#include "cudaInfo.cuh"
#include "imgui.h"
#include "OpenGLHelperMethods.h"
#include "GameOfLifeLogic.h"

int DEBUG_GLOBAL_ROWS;
int DEBUG_GLOBAL_COLS;


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
            1.0f / 6.0f, 1.0f,   // 0 1
            1.0f / 6.0f, 0.0f,   // 0 1/6
            0.0f, 0.0f,

            4.0f / 6.0f, 1.0f, // v1,v6,v7,v2 (left)
            4.0f / 6.0f, 0.0f,
            3.0f / 6.0f, 0.0f,
            3.0f / 6.0f, 1.0f,

            1.0f, 1.0f, // v7,v4,v3,v2 (bottom)
            1.0f, 0.0f,
            5.0f / 6.0f, 0.0f,
            5.0f / 6.0f, 1.0f,

            1.0f / 6.0f, 1.0f,
            2.0f / 6.0f, 1.0f, // v0,v3,v4,v5 (right)
            2.0f / 6.0f, 0.0f,
            1.0f / 6.0f, 0.0f,

            5.0f / 6.0f, 0.0f,
            4.0f / 6.0f, 0.0f,
            4.0f / 6.0f, 1.0f,
            5.0f / 6.0f, 1.0f, // v0,v5,v6,v1 (top)

            3.0f / 6.0f, 0.0f, // v4,v7,v6,v5 (back)
            2.0f / 6.0f, 0.0f,
            2.0f / 6.0f, 1.0f,
            3.0f / 6.0f, 1.0f};

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
    glVertexAttribPointer((GLuint) 2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);  // Vertex texture

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void GameOfLifeCube::cpuCreate(int size) {
    SPDLOG_INFO("Initialize GameOfLife CPU code");

    this->row = size;
    this->column = size * 6;

    DEBUG_GLOBAL_ROWS = this->row;
    DEBUG_GLOBAL_COLS = this->column;

    // make 2d grid
    this->board = (int *) calloc(this->row * this->column, sizeof(int));
    this->pboard = (int *) calloc(this->row * this->column, sizeof(int));

    this->imgBoard = (GLuint *) calloc(this->row * this->column, sizeof(GLuint));

    //Initialize a pattern in the conway grid
//    for (int i = 0; i < (4*(this->column/6)); ++i) {
//        this->board[(3 * column )+ i] = 1;
//    }
    for (int i = 0; i < column; ++i) {
        this->board[(3 * column) + i] = 1;
    }

    genImg(this->board, this->imgBoard);

    genTexImg(&this->cpuTexID, false, this->imgBoard);
}

void GameOfLifeCube::create() {
    SPDLOG_INFO("Making Game Of Life Cube");
    this->worldSize -= this->worldSize % 6;
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
        this->cudaTime = 0;
        genTexImg(&this->cudaTexID, false, this->imgBoard);
    }
    this->run = true;
    this->speed = 5.0f;
    this->timeStart = 0;
    this->useHelpImg = false;
    this->helpTexID = loadTexture("res/textures/Test/testImage.png");
    this->qtyCpu = 0;
    this->cpuAvgTime = 0.0;
    this->cpuTime = 0;

}

void GameOfLifeCube::draw() {
    if (useHelpImg) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->helpTexID);
    } else if (usingCuda) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->cudaTexID);
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
        if (time - this->timeStart > speed) {
            this->timeStart = time;

            if (this->usingCuda && this->havaCuda) {
//                cudaEventRecord(launch_begin, 0);
                double now, then;
                now = currentTime();
                this->cudaBoard = cudaMainUpdate();
                genImg(this->cudaBoard, this->imgBoard);
                genTexImg(&this->cudaTexID, true, this->imgBoard);
                then = currentTime();
                this->cudaTime = static_cast<float>(then - now);
//                cudaEventRecord(launch_end, 0);
//                cudaEventSynchronize(launch_end);
//                cudaEventElapsedTime(&this->cudaTime, launch_begin, launch_end);
//                this->cudaTime = this->cudaTime / 1000;
                this->qtyCuda++;
//                this->cudaAvgTime += (this->cudaTime - this->cudaAvgTime) / static_cast<double>(qtyCuda);
                this->cudaAvgTime += ((then - now) - this->cudaAvgTime) / static_cast<double>(qtyCuda);
            } else {
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
}

void GameOfLifeCube::ImGUIHeader() {
    if (ImGui::CollapsingHeader("Game Of Life")) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Reset Menu")) {
            int max_texture_size;
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
            if (ImGui::SliderInt("New Size Of World", &this->worldSize, 6, (max_texture_size / 6))) {
                this->worldSize -= this->worldSize % 6;
            }
            if (ImGui::Button("Reset game of life world")) {
                bool beforeRun = this->run;
                double beforeTimeStart = this->timeStart;
                float beforeSpeed = this->speed;
                this->cleanUp();
                this->create();
                this->run = beforeRun;
                this->timeStart = beforeTimeStart;
                this->speed = beforeSpeed;
            }
            ImGui::Spacing();
        }
        ImGui::SliderFloat("Speed of Game of Life (sec)", &this->speed, 0.001f, 15.0f);
        ImGui::Checkbox("Run game of life", &this->run);
        ImGui::Checkbox("Use help image (f, l, r, b, t, b)", &this->useHelpImg);
        if (this->havaCuda) {
            if (this->useHelpImg) {
                ImGui::Text("Warring, Using Help Image");
            }
            ImGui::Checkbox("Use CUDA instead of CPU", &this->usingCuda);
        } else {
            ImGui::Text("Cuda not available");
        }
        if (ImGui::Button("Console Print CPU State")) {
            SPDLOG_INFO(spdlog::fmt_lib::format("CPU State {}", this->qtyCpu));
            printboard(this->board, this->row, this->column);
        }
        if (ImGui::Button("Console Print CUDA/GPU State")) {
            SPDLOG_INFO(spdlog::fmt_lib::format("CUDA/GPU State {}", this->qtyCuda));
            printboard(this->cudaBoard, this->row, this->column);
        }
        ImGui::Text("World Size: row: %d, col(Note: row * 6): %d", this->row, this->column);
        // TODO: sub header
        if (ImGui::CollapsingHeader("CPU Board")) {
            ImGui::Text(stringBoard(this->board, this->row, this->column).c_str());
        }
        if (this->qtyCuda > 0) {
            if (ImGui::CollapsingHeader("CUDA/GPU Board")) {
                ImGui::Text(stringBoard(this->cudaBoard, this->row, this->column).c_str());
            }
        }
        if (this->qtyCpu > 0) ImGui::Text("CPU        Time %f (ms)", this->cpuTime * 1000);
        if (this->qtyCuda > 0) ImGui::Text("GPU (CUDA) Time %f (ms)", this->cudaTime * 1000);
        if (this->qtyCpu > 0 && this->qtyCuda > 0) ImGui::Text("The speedup(SerialTimeCost / CudaTimeCost) when using GPU is %lf", this->cpuTime / this->cudaTime);
        ImGui::Spacing();
        if (this->qtyCpu > 0) ImGui::Text("CPU        Avg Time %f (ms)", this->cpuAvgTime * 1000);
        if (this->qtyCuda > 0) ImGui::Text("GPU (CUDA) Avg Time %f (ms)", this->cudaAvgTime * 1000);
        if (this->qtyCpu > 0 && this->qtyCuda > 0) ImGui::Text("The avg speedup(SerialTimeCost / CudaTimeCost) when using GPU is %lf",
                    this->cpuAvgTime / this->cudaAvgTime);
        ImGui::Spacing();
        ImGui::Text("     CPU State: %ld", this->qtyCpu);
        ImGui::Text("CUDA/GPU State: %ld", this->qtyCuda);
    }
}

void GameOfLifeCube::cleanUp() {
    if (this->havaCuda) {
        cudaMainCleanUp();
        cudaEventDestroy(this->launch_begin); // GameOfLifeCube::create()
        cudaEventDestroy(this->launch_end); // GameOfLifeCube::create()
        glDeleteTextures(1, &this->cudaTexID); // GameOfLifeCube::create()
    }
    free(this->board); // GameOfLifeCube::cpuCreate
    free(this->pboard); // GameOfLifeCube::cpuCreate
    free(this->imgBoard); // GameOfLifeCube::cpuCreate
    glDeleteTextures(1, &this->cpuTexID); // GameOfLifeCube::cpuCreate
    glDeleteTextures(1, &this->helpTexID); // GameOfLifeCube::create
    glDeleteVertexArrays(1, &this->cube_vao); // GameOfLifeCube::cubeCreate
}

void GameOfLifeCube::cpuUpdate(double time) {
    runlife(this->board, this->pboard, this->row, this->column);
    // swap boards
    int *tempBoard = this->pboard;
    this->pboard = this->board;
    this->board = tempBoard;

    genImg(this->board, this->imgBoard);

    genTexImg(&this->cpuTexID, true, this->imgBoard);
}

void GameOfLifeCube::genTexImg(GLuint *texId, bool freeOldImg, GLuint *inputImgBoard) {
    if (freeOldImg) {
        glDeleteTextures(1, texId);
    }
    glGenTextures(1, texId);
    glBindTexture(GL_TEXTURE_2D, *texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 this->column, this->row, 0,
                 GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, inputImgBoard);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, *texId);
}

void GameOfLifeCube::genImg(int *board, GLuint *outputImgBoard) {
    for (size_t x = 0; x < this->column; x++) {
        for (size_t y = 0; y < this->row; y++) {
            outputImgBoard[x + y * this->column] = UINT32_MAX * (board[x + y * this->column] && true);
        }
    }
}
