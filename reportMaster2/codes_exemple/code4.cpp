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
	  sycl::parallel_for<class KernelCompute>(sycl::range<2> {M-2, N-2}, 
						  sycl::id<2> {1, 1},
						  [=] (sycl::item<2> it) {
						    sycl::id<2> index = it.get_global_id();
						    sycl::id<2> id1(sycl::range<2> {0,1});
						    sycl::id<2> id2(sycl::range<2> {1,0});
						    b[index] = a[index] + a[index+id1] + a[index+id2] + a[index-id1] + a[index-id2];
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
