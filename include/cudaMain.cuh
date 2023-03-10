#ifndef EWU_CSCD445_PROJECT_CUDAMAIN_CUH
#define EWU_CSCD445_PROJECT_CUDAMAIN_CUH

#ifdef __CUDACC__

__host__ void cudaMainInitialize();

__host__ void cudaMainUpdate(double time);

__host__ void cudaMainCleanUp();

#else

void cudaMainInitialize();

void cudaMainUpdate(double time);

void cudaMainCleanUp();

#endif

#endif //EWU_CSCD445_PROJECT_CUDAMAIN_CUH
