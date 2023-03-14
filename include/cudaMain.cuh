#ifndef EWU_CSCD445_PROJECT_CUDAMAIN_CUH
#define EWU_CSCD445_PROJECT_CUDAMAIN_CUH

extern GLuint cudaTexID;

#ifdef __CUDACC__

__host__ void cudaMainInitialize(int size);

__host__ void cudaMainUpdate();

__host__ void cudaMainCleanUp();

#else

void cudaMainInitialize(int size);

void cudaMainUpdate();

void cudaMainCleanUp();

#endif

#endif //EWU_CSCD445_PROJECT_CUDAMAIN_CUH
