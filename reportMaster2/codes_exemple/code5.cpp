#include <CL/sycl.hpp>
#include "helper-constants.hpp"
using namespace cl;
int main(int argc, char **argv) {
  read_args(argc, argv);
  sycl::buffer<float,2> ioABuffer = cl::sycl::buffer<float,2>(sycl::range<2> {M, N});
  sycl::buffer<float,2> ioBBuffer = sycl::buffer<float,2>(sycl::range<2> {M, N}); 
  for (size_t i = 0; i < M; ++i){
    for (size_t j = 0; j < N; ++j){
      float value = ((float) i*(j+2) + 10) / N;
      sycl::id<2> id = {(int) i, (int) j};
      ioABuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
      ioBBuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
    }
  }
  {  
    sycl::queue myQueue;
    for (unsigned int i = 0; i < NB_ITER; ++i){
      sycl::command_group(myQueue, [&]() {
	  sycl::accessor<float, 2, sycl::access::read>  a(ioABuffer);
	  sycl::accessor<float, 2, sycl::access::write> b(ioBBuffer);
	  sycl::parallel_for_workgroup<class KernelCompute>(sycl::nd_range<2> {
	      sycl::range<2> {M-2, N-2}, 
		sycl::range<2> {CL_DEVICE_MAX_WORK_GROUP_SIZE0, CL_DEVICE_MAX_WORK_GROUP_SIZE1}, 
		  sycl::id<2> {1, 1}}, 
	    [=](sycl::group<2> group){
	      float * local = new float[(CL_DEVICE_MAX_WORK_GROUP_SIZE0+2)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)];
	      sycl::parallel_for_workitem(group, [=,&local](sycl::nd_item<2> it){
		  sycl::range<2> l_range = it.get_local_range();
		  sycl::id<2> g_ind = it.get_global_id();
		  sycl::id<2> l_ind = it.get_local_id();
		  sycl::id<2> offset = it.get_offset();
		  sycl::id<2> id1(sycl::range<2> {0,1});
		  sycl::id<2> id2(sycl::range<2> {1,0});
		  sycl::id<2> id1_s(sycl::range<2> {0,l_range.get(1)});
		  sycl::id<2> id2_s(sycl::range<2> {l_range.get(0),0});
		  local[(l_ind+offset).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset).get(1)] = a[g_ind+offset];
		  if (l_ind.get(0) == 0) {
		    local[(l_ind+offset).get(1)] = a[g_ind-id2+offset];
		    local[(id2_s+offset).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset).get(1)] = a[g_ind+id2_s+offset];
		  }
		  if (l_ind.get(1) == 0) {
		    local[(l_ind+offset).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2)] = a[g_ind-id1+offset];
		    local[(l_ind+offset).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (id1_s+offset).get(1)] = a[g_ind+id1_s+offset];
		  }
		});
	      sycl::parallel_for_workitem(group, [=,&local](sycl::nd_item<2> it){
		  sycl::id<2> g_ind = it.get_global_id();
		  sycl::id<2> l_ind = it.get_local_id();
		  sycl::id<2> offset = it.get_offset();
		  sycl::id<2> id1(sycl::range<2> {0,1});
		  sycl::id<2> id2(sycl::range<2> {1,0});
		  b[g_ind+offset] = local[(l_ind+offset).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset).get(1)];
		  b[g_ind+offset] += local[(l_ind+offset+id1).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset+id1).get(1)];
		  b[g_ind+offset] += local[(l_ind+offset+id2).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset+id2).get(1)];
		  b[g_ind+offset] += local[(l_ind+offset-id1).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset-id1).get(1)];
		  b[g_ind+offset] += local[(l_ind+offset-id2).get(0)*(CL_DEVICE_MAX_WORK_GROUP_SIZE1+2) + (l_ind+offset-id2).get(1)];
		});
	      delete [] local;
	    });
	});
      sycl::command_group(myQueue, [&]() {
      	  sycl::accessor<float, 2, sycl::access::write> a(ioABuffer);
      	  sycl::accessor<float, 2, sycl::access::read>  b(ioBBuffer);
      	  sycl::parallel_for<class KernelCopy>(sycl::range<2> {M-2, N-2},
      					       sycl::id<2> {1, 1},
      					       [=] (sycl::item<2> it) {
      						 a[it] = MULT_COEF * b[it];
      					       });
      	});
    }
  }
  return 0;
}
