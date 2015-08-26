#include "helper-constants.hpp"
#include "5-stencil.hpp"
using namespace cl;
float& fdl_out(int a,int b, sycl::accessor<float, 2, sycl::access::write> acc) {return acc[a][b];}
float  fdl_in(int a,int b, sycl::accessor<float, 2, sycl::access::read>  acc) {return acc[a][b];}
sycl::buffer<float,2> ioBuffer;
sycl::buffer<float,2> ioABuffer;
int main(int argc, char **argv) {
  read_args(argc, argv);
  ioBuffer = sycl::buffer<float,2>(sycl::range<2> {M, N});
  ioABuffer = sycl::buffer<float,2>(sycl::range<2> {M, N}); 
  for (size_t i = 0; i < M; ++i){
    for (size_t j = 0; j < N; ++j){
      float value = ((float) i*(j+2) + 10) / N;
      sycl::id<2> id = {(int) i, (int) j};
      ioBuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
      ioABuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
    }
  }
  coef_fxd2D<0,0> c_id {1.0f};
  coef_fxd2D<0, 0> c1 {MULT_COEF};  
  coef_fxd2D<1, 0> c2 {MULT_COEF};
  coef_fxd2D<0, 1> c3 {MULT_COEF};
  coef_fxd2D<-1, 0> c4 {MULT_COEF};
  coef_fxd2D<0, -1> c5 {MULT_COEF};
  auto st = c1+c2+c3+c4+c5;
  input_fxd2D<float, &ioABuffer, &fdl_in> work_in;
  output_2D<float, &ioBuffer, &fdl_out> work_out;
  auto op_work = work_out << st << work_in;
  auto st_id = c_id.toStencil();
  input_fxd2D<float, &ioBuffer, &fdl_in> copy_in;
  output_2D<float, &ioABuffer, &fdl_out> copy_out;
  auto op_copy = copy_out << st_id << copy_in;
  {   
    sycl::queue myQueue; 
    for (unsigned int i = 0; i < NB_ITER; ++i){      
      op_work.doLocalComputation(myQueue);
      op_copy.doComputation(myQueue);
    }
  }
  return 0;
}
