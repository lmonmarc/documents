#include <cstdlib>
#include <CL/sycl.hpp>
#include "helper-constants.hpp"
using namespace cl;
int main(int argc, char **argv) {
  read_args(argc, argv);
  float * data_a = (float *) malloc(sizeof(float)*M*N);
  float * data_b = (float *) malloc(sizeof(float)*M*N);
  for (size_t i = 0; i < M; ++i){
    size_t ind = i*N;
    for (size_t j = 0; j < N; ++j){
      float value = ((float) i*(j+2) + 10) / N;
      data_a[ind] = value;
      data_b[ind] = value;
      ++ind;
    }
  }
  for (size_t t = 0; t < NB_ITER; ++t){
    for (size_t grp0 = 0; grp0 < (M-2)/CL_DEVICE_MAX_WORK_GROUP_SIZE0; ++grp0) {
      size_t gl_ind0 = grp0*CL_DEVICE_MAX_WORK_GROUP_SIZE0;
      for (size_t grp1 = 0; grp1 < (N-2)/CL_DEVICE_MAX_WORK_GROUP_SIZE1; ++grp1) {
	size_t gl_ind1 = grp1*CL_DEVICE_MAX_WORK_GROUP_SIZE1;
	float *local = new float[(CL_DEVICE_MAX_WORK_GROUP_SIZE0+2)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)];
#pragma omp parallel 
	{
#pragma omp single nowait
	  {
	    for(size_t l_ind0 = 0; l_ind0 < CL_DEVICE_MAX_WORK_GROUP_SIZE0; ++l_ind0){
	      for(size_t l_ind1 = 0; l_ind1 < CL_DEVICE_MAX_WORK_GROUP_SIZE1; ++l_ind1){
#pragma omp task firstprivate(l_ind0,l_ind1,gl_ind0,gl_ind1)
		{      
		  size_t g_ind0 = gl_ind0 + l_ind0;
		  size_t g_ind1 = gl_ind1 + l_ind1;
		  size_t left_ind = (l_ind0+1)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)+(l_ind1+1);
		  size_t right_ind = (g_ind0+1)*N+g_ind1+1;
		  (local[left_ind]) = data_a[right_ind];
		  if (l_ind0 == 0) {
		    size_t left_ind_top = l_ind1+1;
		    size_t right_ind_top = g_ind0*N+g_ind1+1;
		    size_t left_ind_bottom = (CL_DEVICE_MAX_WORK_GROUP_SIZE0+1)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)+(l_ind1+1);
		    size_t right_ind_bottom = (g_ind0+CL_DEVICE_MAX_WORK_GROUP_SIZE0+1)*N+g_ind1+1;
		    (local[left_ind_top]) = data_a[right_ind_top];
		    (local[left_ind_bottom]) = data_a[right_ind_bottom];
		  }
		  if (l_ind1 == 0) {
		    size_t left_ind_top = (l_ind0+1)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2);
		    size_t right_ind_top = (g_ind0+1)*N+g_ind1;
		    size_t left_ind_bottom = (l_ind0+1)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)+(CL_DEVICE_MAX_WORK_GROUP_SIZE1+1);
		    size_t right_ind_bottom = (g_ind0+1)*N+g_ind1+CL_DEVICE_MAX_WORK_GROUP_SIZE1+1;
		    (local[left_ind_top]) = data_a[right_ind_top];
		    (local[left_ind_bottom]) = data_a[right_ind_bottom];
		  }
		}
	      }
	    }
	  }
	}
#pragma omp parallel 
	{
#pragma omp single nowait
	  {
	    for(size_t l_ind0 = 0; l_ind0 < CL_DEVICE_MAX_WORK_GROUP_SIZE0; ++l_ind0){
	      for(size_t l_ind1 = 0; l_ind1 < CL_DEVICE_MAX_WORK_GROUP_SIZE1; ++l_ind1){
#pragma omp task firstprivate(l_ind0,l_ind1,gl_ind0,gl_ind1)
		{      		  
		  size_t g_ind0 = gl_ind0 + l_ind0;
		  size_t g_ind1 = gl_ind1 + l_ind1;
		  data_b[i*N+j] = MULT_COEF * (data_a[i*N+j] + data_a[i*N+(j-1)] + data_a[i*N+(1+j)] + data_a[(1+i)*N+j] + data_a[(i-1)*N+j]);
		}
	      }
	    }
	  }
	}
	delete [] local;
      }
    }
#pragma omp parallel for
    for (size_t i = 1; i < M - 1; ++i){
      for (size_t j = 1; j < N - 1; ++j){
	data_a[i*N+j] = data_b[i*N+j];
      }
    }
  }
  free(data_a);
  free(data_b);  
  return 0;
}
