#include "helper-constants.hpp"
#include "5-stencil.hpp"
using namespace cl;
float& fdl_out(int a,int b, sycl::accessor<float, 2, sycl::access::write> acc) {return acc[a][b];}
float  fdl_in(int a,int b, sycl::accessor<float, 2, sycl::access::read>  acc) {return acc[a][b];}
float  fac(int a,int b, int c, int d, sycl::accessor<float, 1, sycl::access::read>  acc) {return MULT_COEF*acc[0];}
float  fac_id(int a,int b, int c, int d, sycl::accessor<float, 1, sycl::access::read>  acc) {return acc[0];}
sycl::buffer<float,2> ioBuffer;
sycl::buffer<float,2> ioABuffer;
sycl::buffer<float,1> ioBBuffer;
int main(int argc, char **argv) {
  read_args(argc, argv);
  float tab_var = 1.0;
  ioBuffer = sycl::buffer<float,2>(sycl::range<2> {M, N});
  ioABuffer = sycl::buffer<float,2>(sycl::range<2> {M, N}); 
  ioBBuffer = sycl::buffer<float,1>(&tab_var, sycl::range<1> {1}); 
  for (size_t i = 0; i < M; ++i){
    for (size_t j = 0; j < N; ++j){
      float value = ((float) i*(j+2) + 10) / N;
      sycl::id<2> id = {(int) i, (int) j};
      ioBuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
      ioABuffer.get_access<sycl::access::write, sycl::access::host_buffer>()[id] = value;
    }
  }
  coef_var2D<0, 0> c1;  
  coef_var2D<1, 0> c2;
  coef_var2D<0, 1> c3;
  coef_var2D<-1, 0> c4;
  coef_var2D<0, -1> c5;
  auto st = c1+c2+c3+c4+c5;
  input_var2D<float, &ioABuffer, &ioBBuffer, &fdl_in, &fac> work_in;
  output_2D<float, &ioBuffer, &fdl_out> work_out;
  auto op_work = work_out << st << work_in;
  auto st_id = c1.toStencil();
  input_var2D<float, &ioBuffer, &ioBBuffer, &fdl_in, &fac_id> copy_in;
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
