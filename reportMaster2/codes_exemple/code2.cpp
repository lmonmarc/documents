#include <cstdlib>
#include <CL/sycl.hpp>
#include "helper-constants.hpp"
using namespace cl;
int main(int argc, char **argv) {
  read_args(argc, argv);
  float *a_test = (float *) malloc(sizeof(float)*M*N);
  float *b_test = (float *) malloc(sizeof(float)*M*N);
  for (size_t i = 0; i < M; ++i){
    for (size_t j = 0; j < N; ++j){
      float value = ((float) i*(j+2) + 10) / N;
      a_test[i*N+j] = value;
      b_test[i*N+j] = value;
    }
  }
  for (unsigned int t = 0; t < NB_ITER; ++t){
    for (size_t i = 1; i < M - 1; ++i){
#pragma omp parallel for
      for (size_t j = 1; j < N - 1; ++j){
	b_test[i*N+j] = MULT_COEF * (a_test[i*N+j] + a_test[i*N+(j-1)] + a_test[i*N+(1+j)] + a_test[(1+i)*N+j] + a_test[(i-1)*N+j]);
      }
    }
    for (size_t i = 1; i < M - 1; ++i){
      for (size_t j = 1; j < N - 1; ++j){
	a_test[i*N+j] = b_test[i*N+j];
      }
    }  
  }  
  free(a_test);
  free(b_test);
  return 0;
}
