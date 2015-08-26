__kernel void jacobi2d(__global float *input, __global float *output, int N, float h){
    int i = get_global_id(0);
    int j = get_global_id(1);
    int i_prev = i-1;
    int i_next = i+1;
    int j_prev = j-1;
    int j_next = j+1;
    int index = i*N + j;
    if( i > 0 && j > 0 && i < (N-1) && j <(N-1))
	output[index] = 0.25f * (input[i_prev * N + j] + input[i_next * N + j] + input[i * N + j_prev] + input[i * N + j_next ] - 4*h*h);
}
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <CL/opencl.h>
#include "util.h"
#define MAX_PLATFORMS 3
#define MAX_DEVICES   5
#define ITERATIONS    10
#define KERNEL_NAME  "jacobi2d"
unsigned SIZE = 1024;
unsigned TILE = 16;
cl_platform_id pf[MAX_PLATFORMS];
cl_uint nb_platforms = 0;
cl_int err;
cl_device_type device_type = CL_DEVICE_TYPE_ALL;
cl_device_id devices[MAX_DEVICES];
cl_uint nb_devices = 0;
cl_context context = NULL;
cl_program program = NULL;
cl_kernel kernel = NULL;
cl_command_queue queue = NULL;
static void perform_cpu_jacobi(float *t, float *t_prev){
    float h,x,y;
    h = 1.0f/(SIZE-1);
    for(unsigned int i=0;i<SIZE;i++) {
	x = i*h;
	t_prev[i*SIZE+0] = x*x;
	t_prev[i*SIZE+(SIZE-1)] = x*x + 1.0f;
    }
    for(unsigned int j=0;j < SIZE; j++) {
	y = j*h;
	t_prev[0*SIZE+j] = y*y;
	t_prev[((SIZE-1) * SIZE) + j] = 1.0f + y*y;
    }
    for(unsigned int k=0;k<ITERATIONS;k++) {
	    for(unsigned int j=1;j<(SIZE-1);j++) {
		    for(unsigned int i=1;i<(SIZE-1);i++) {
			    t[i*SIZE+j] = 0.25f * (t_prev[(i-1)*SIZE+j] + t_prev[(i+1)*SIZE+j] + t_prev[i*SIZE+(j-1)] + t_prev[i*SIZE+(j+1)] - 4*h*h);
			}
		}
	    float* pingPong = t_prev;
	    t_prev = t;
	    t = pingPong;
	}
}
static inline void launch_kernel(float *h, size_t *global, size_t *local, cl_mem d_t, cl_mem d_t_prev){
	err = 0;
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_t_prev);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_t);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &SIZE);
	err |= clSetKernelArg(kernel, 3, sizeof(float), h);
	check(err, "Failed to set kernel arguments");
	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, NULL);
	check(err, "Failed to execute kernel");
}
static void perform_gpu_jacobi(cl_mem d_t, cl_mem d_t_prev, float *t, float *t_prev){
    float h,x,y;
    size_t global[2] = {SIZE, SIZE};
    size_t local[2] = {TILE, TILE};
    h = 1.0f/(SIZE-1);
    memset(t_prev, 0, sizeof(float) * SIZE*SIZE);
    err = clEnqueueWriteBuffer(queue, d_t, CL_TRUE, 0,sizeof(float) * SIZE*SIZE,t_prev, 0, NULL, NULL);
    check(err, "Failed to write d_t");
    for(unsigned int i=0;i<SIZE;i++) {
	x = i*h;
	t_prev[i*SIZE+0] = x*x;
	t_prev[i*SIZE+(SIZE-1)] = x*x + 1.0f;
    }
    for(unsigned int j=0;j < SIZE; j++) {
	y = j*h;
	t_prev[0*SIZE+j] = y*y;
	t_prev[((SIZE-1) * SIZE) + j] = 1.0f + y*y;
    }
    err = clEnqueueWriteBuffer(queue, d_t_prev, CL_TRUE, 0, sizeof(float) * SIZE*SIZE, t_prev, 0, NULL, NULL);
    check(err, "Failed to write d_t_prev");
    launch_kernel(&h, global, local, d_t, d_t_prev);
    cl_mem pingpong = d_t_prev;
    d_t_prev = d_t;
    d_t = pingpong;
    for(unsigned int k=1;k<ITERATIONS;k++) {
	launch_kernel(&h, global, local, d_t, d_t_prev);
	cl_mem pingpong = d_t_prev;
	d_t_prev = d_t;
	d_t = pingpong;
    }
    clFinish(queue);
    err = clEnqueueReadBuffer(queue, d_t_prev, CL_TRUE, 0, sizeof(float) * SIZE*SIZE, t, 0, NULL, NULL);
    check(err, "Failed to read d_t_prev");
}
static void alloc_cpu(float **t, float **t_prev){
    *t = calloc(SIZE*SIZE, sizeof(float));
    *t_prev = calloc(SIZE*SIZE, sizeof(float));
}
static void free_cpu(float *t, float *t_prev){
    if(t !=NULL)
	free(t);
    if(t_prev != NULL)
	free(t_prev);
}
static void alloc_gpu(cl_mem *d_t, cl_mem *d_t_prev){
    unsigned int totalsize = SIZE*SIZE * sizeof(float);
    *d_t = clCreateBuffer(context, CL_MEM_READ_WRITE, totalsize, NULL, &err);
    check(err, "Failed to allocate buffer t");
    *d_t_prev = clCreateBuffer(context,  CL_MEM_READ_WRITE, totalsize, NULL, &err);
    check(err, "Failed to allocate buffer t_prev");
}
static void free_gpu(cl_mem d_t, cl_mem d_t_prev){
    clReleaseMemObject(d_t);
    clReleaseMemObject(d_t_prev);
}
void cl_init(cl_int p){
    char name[1024], vendor[1024];
    err = clGetPlatformIDs(MAX_PLATFORMS, pf, &nb_platforms);
    check(err, "Failed to get platform IDs");
    err = clGetPlatformInfo(pf[p], CL_PLATFORM_NAME, 1024, name, NULL);
    check(err, "Failed to get Platform Info");
    err = clGetPlatformInfo(pf[p], CL_PLATFORM_VENDOR, 1024, vendor, NULL);
    check(err, "Failed to get Platform Info");
    err = clGetDeviceIDs(pf[p], device_type, MAX_DEVICES, devices, &nb_devices);
    check(err, "Failed to get device IDs");
    context = clCreateContext(0, nb_devices, devices, NULL, NULL, &err);
    check(err, "Failed to create compute context");
    queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &err);
    check(err, "Failed to creatre command queue");
}
static void cl_load_prog(){
    const char *opencl_prog = file_load(KERNEL_FILE);
    program = clCreateProgramWithSource(context, 1, &opencl_prog, NULL, &err);
    check(err, "Failed to create program");
    char flags[1024];
    sprintf (flags, "-cl-mad-enable -cl-fast-relaxed-math");
    err = clBuildProgram(program, 0, NULL, flags, NULL, NULL);
    if(err != CL_SUCCESS) {
	error("Failed to build program!\n");
    }
    kernel = clCreateKernel(program, KERNEL_NAME, &err);
    check(err, "Failed to create compute kernel");
}
static void cl_release(){
    clReleaseCommandQueue(queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseContext(context);
}
int main(int argc, char** argv){
    float *t = NULL, *t_prev = NULL;
    cl_mem d_t = NULL, d_t_prev = NULL;
    if (argc < 2)
	usage();
    if (argc >= 3)
	TILE=atoi(argv[2]);
    cl_int p = atoi(argv[1]);
    cl_init(p);
    cl_load_prog();
    alloc_cpu(&t, &t_prev);
    alloc_gpu(&d_t, &d_t_prev);
    perform_cpu_jacobi(t, t_prev);
    float *tmp = calloc(SIZE*SIZE, sizeof(float));
    memcpy(tmp, t_prev, SIZE*SIZE * sizeof(float));
    perform_gpu_jacobi(d_t, d_t_prev, t, t_prev);
    free(tmp);
    free_cpu(t, t_prev);
    free_gpu(d_t, d_t_prev);
    cl_release();
    return 0;
}
