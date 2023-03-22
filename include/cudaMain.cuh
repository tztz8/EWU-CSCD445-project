#ifndef EWU_CSCD445_PROJECT_CUDAMAIN_CUH
#define EWU_CSCD445_PROJECT_CUDAMAIN_CUH

#ifdef __CUDACC__

__host__ void cudaMainInitialize(int size);

__host__ int* cudaMainUpdate();

__host__ void cudaMainCleanUp();

#else

void cudaMainInitialize(int size);

int* cudaMainUpdate();

void cudaMainCleanUp();

#endif

#endif //EWU_CSCD445_PROJECT_CUDAMAIN_CUH
