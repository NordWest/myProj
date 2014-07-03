////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993-2013 NVIDIA Corporation.  All rights reserved.
//
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of
// this software. Any use, reproduction, disclosure, or distribution of
// this software and related documentation outside the terms of the EULA
// is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////////

/* Example of integrating CUDA functions into an existing
 * application / framework.
 * Host part of the device code.
 * Compiled with Cuda compiler.
 */

// System includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_cuda.h>
#include <helper_functions.h>

#ifndef MAX
#define MAX(a,b) (a > b ? a : b)
#endif

#define EKV 0.409092804
#define ka 0.017202098955
#define CAU 173.144632685

#include "rada.h"

extern ever_params *eparam;
extern int nofzbody;
extern double *mass;

////////////////////////////////////////////////////////////////////////////////
// declaration, forward

extern "C" void computeGold(char *reference, char *idata, const unsigned int len);
extern "C" void computeGold2(int2 *reference, int2 *idata, const unsigned int len);

///////////////////////////////////////////////////////////////////////////////
//! Simple test kernel for device functionality
//! @param g_odata  memory to process (in and out)
///////////////////////////////////////////////////////////////////////////////
__global__ void kernel(int *g_data)
{
    // write data to global memory
    const unsigned int tid = threadIdx.x;
    int data = g_data[tid];

    // use integer arithmetic to process all four bytes with one thread
    // this serializes the execution, but is the simplest solutions to avoid
    // bank conflicts for this very low number of threads
    // in general it is more efficient to process each byte by a separate thread,
    // to avoid bank conflicts the access pattern should be
    // g_data[4 * wtid + wid], where wtid is the thread id within the half warp
    // and wid is the warp id
    // see also the programming guide for a more in depth discussion.
    g_data[tid] = ((((data <<  0) >> 24) - 10) << 24)
                  | ((((data <<  8) >> 24) - 10) << 16)
                  | ((((data << 16) >> 24) - 10) <<  8)
                  | ((((data << 24) >> 24) - 10) <<  0);
}


///////////////////////////////////////////////////////////////////////////////
//! Demonstration that int2 data can be used in the cpp code
//! @param g_odata  memory to process (in and out)
///////////////////////////////////////////////////////////////////////////////
__global__ void
kernel2(int2 *g_data)
{
    // write data to global memory
    const unsigned int tid = threadIdx.x;
    int2 data = g_data[tid];

    // use integer arithmetic to process all four bytes with one thread
    // this serializes the execution, but is the simplest solutions to avoid
    // bank conflicts for this very low number of threads
    // in general it is more efficient to process each byte by a separate thread,
    // to avoid bank conflicts the access pattern should be
    // g_data[4 * wtid + wid], where wtid is the thread id within the half warp
    // and wid is the warp id
    // see also the programming guide for a more in depth discussion.
    g_data[tid].x = data.x - data.y;
}

extern "C" __global__ void VecAdd_kernel(const float *A, const float *B, float *C, int N)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < N)
        C[i] = A[i] + B[i];
}

__global__ void
vectorAdd(const float *A, const float *B, float *C, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        C[i] = A[i] + B[i];
    }
}

////////////////////////////////////////////////////////////////////////////////
//! Entry point for Cuda functionality on host side
//! @param argc  command line argument count
//! @param argv  command line arguments
//! @param data  data to process on the device
//! @param len   len of \a data
////////////////////////////////////////////////////////////////////////////////
extern "C" bool
runTest(const int argc, const char **argv, char *data, int2 *data_int2, unsigned int len)
{
    // use command-line specified CUDA device, otherwise use device with highest Gflops/s
    findCudaDevice(argc, (const char **)argv);

    const unsigned int num_threads = len / 4;
    assert(0 == (len % 4));
    const unsigned int mem_size = sizeof(char) * len;
    const unsigned int mem_size_int2 = sizeof(int2) * len;

    // allocate device memory
    char *d_data;
    checkCudaErrors(cudaMalloc((void **) &d_data, mem_size));
    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_data, data, mem_size,
                               cudaMemcpyHostToDevice));
    // allocate device memory for int2 version
    int2 *d_data_int2;
    checkCudaErrors(cudaMalloc((void **) &d_data_int2, mem_size_int2));
    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_data_int2, data_int2, mem_size_int2,
                               cudaMemcpyHostToDevice));

    // setup execution parameters
    dim3 grid(1, 1, 1);
    dim3 threads(num_threads, 1, 1);
    dim3 threads2(len, 1, 1); // more threads needed fir separate int2 version
    // execute the kernel
    kernel<<< grid, threads >>>((int *) d_data);
    kernel2<<< grid, threads2 >>>(d_data_int2);

    // check if kernel execution generated and error
    getLastCudaError("Kernel execution failed");

    // compute reference solutions
    char *reference = (char *) malloc(mem_size);
    computeGold(reference, data, len);
    int2 *reference2 = (int2 *) malloc(mem_size_int2);
    computeGold2(reference2, data_int2, len);

    // copy results from device to host
    checkCudaErrors(cudaMemcpy(data, d_data, mem_size,
                               cudaMemcpyDeviceToHost));
    checkCudaErrors(cudaMemcpy(data_int2, d_data_int2, mem_size_int2,
                               cudaMemcpyDeviceToHost));

    // check result
    bool success = true;

    for (unsigned int i = 0; i < len; i++)
    {
        if (reference[i] != data[i] ||
            reference2[i].x != data_int2[i].x ||
            reference2[i].y != data_int2[i].y)
        {
            success = false;
        }
    }

    // cleanup memory
    checkCudaErrors(cudaFree(d_data));
    checkCudaErrors(cudaFree(d_data_int2));
    free(reference);
    free(reference2);

    return success;
}


extern "C" bool
runSumm(void)
{
    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    // Print the vector length to be used, and compute its size
    int numElements = 50000;
    size_t size = numElements * sizeof(float);
    printf("[Vector addition of %d elements]\n", numElements);

    // Allocate the host input vector A
    float *h_A = (float *)malloc(size);

    // Allocate the host input vector B
    float *h_B = (float *)malloc(size);

    // Allocate the host output vector C
    float *h_C = (float *)malloc(size);

    // Verify that allocations succeeded
    if (h_A == NULL || h_B == NULL || h_C == NULL)
    {
        fprintf(stderr, "Failed to allocate host vectors!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the host input vectors
    for (int i = 0; i < numElements; ++i)
    {
        h_A[i] = rand()/(float)RAND_MAX;
        h_B[i] = rand()/(float)RAND_MAX;
    }

    // Allocate the device input vector A
    float *d_A = NULL;
    err = cudaMalloc((void **)&d_A, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector A (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Allocate the device input vector B
    float *d_B = NULL;
    err = cudaMalloc((void **)&d_B, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector B (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Allocate the device output vector C
    float *d_C = NULL;
    err = cudaMalloc((void **)&d_C, size);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector C (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy the host input vectors A and B in host memory to the device input vectors in
    // device memory
    printf("Copy input data from the host memory to the CUDA device\n");
    err = cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector A from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    err = cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector B from host to device (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Launch the Vector Add CUDA Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid =(numElements + threadsPerBlock - 1) / threadsPerBlock;
    printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, numElements);
    err = cudaGetLastError();

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch vectorAdd kernel (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Copy the device result vector in device memory to the host result vector
    // in host memory.
    printf("Copy output data from the CUDA device to the host memory\n");
    err = cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy vector C from device to host (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Verify that the result vector is correct
    for (int i = 0; i < numElements; ++i)
    {
        if (fabs(h_A[i] + h_B[i] - h_C[i]) > 1e-5)
        {
            fprintf(stderr, "Result verification failed at element %d!\n", i);
            exit(EXIT_FAILURE);
        }
    }
    printf("Test PASSED\n");

    // Free device global memory
    err = cudaFree(d_A);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    err = cudaFree(d_B);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector B (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
    err = cudaFree(d_C);

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to free device vector C (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    // Free host memory
    free(h_A);
    free(h_B);
    free(h_C);

    // Reset the device and exit
    err = cudaDeviceReset();

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to deinitialize the device! error=%s\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    printf("Done\n");
    return 0;
}

__global__ void test_force_GN_kernel(double X[], double V[], double F[], int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    int teloi=i/3;
    if (i < numElements)
    {

        F[i] = X[i]*V[i];
    }
}

extern "C" void test_force_GN_CU(double X[], double V[], double F[])
{
  int iNum = nofzbody;
  int Ni = iNum*3;
  int i;

  cudaError_t err = cudaSuccess;

  printf("nofzbody: %d\nNi: %d\n", nofzbody, Ni);

  // Allocate the device input vector A
  size_t size = Ni * sizeof(double);
  double *d_X = NULL;
  err = cudaMalloc((void **)&d_X, size);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to allocate device vector X (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  // Allocate the device input vector B
  double *d_V = NULL;
  err = cudaMalloc((void **)&d_V, size);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to allocate device vector V (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  // Allocate the device output vector C
  double *d_F = NULL;
  err = cudaMalloc((void **)&d_F, size);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to allocate device vector F (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  double *d_Mass = NULL;
  err = cudaMalloc((void **)&d_Mass, size);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to allocate device vector Mass (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  printf("Copy input data X from the host memory to the CUDA device\n");
  err = cudaMemcpy(d_X, X, size, cudaMemcpyHostToDevice);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to copy vector X from host to device (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  err = cudaMemcpy(d_V, V, size, cudaMemcpyHostToDevice);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to copy vector V from host to device (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  err = cudaMemcpy(d_F, F, size, cudaMemcpyHostToDevice);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to copy vector V from host to device (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  err = cudaMemcpy(d_Mass, mass, size, cudaMemcpyHostToDevice);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to copy vector V from host to device (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  int numBlocks = 1;
//  dim3 threadsPerBlock(Ni, Ni);
  test_force_GN_kernel<<<numBlocks, Ni>>>(d_X, d_V, d_F, Ni);

  //double *FS = new double[iNum];

  for(i=0;i<iNum;i++)
  {
      printf("F[%d]: %f ?= %f\n", i, d_F[i], X[i]*V[i]);

  }

  // Free device global memory
  err = cudaFree(d_X);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to free device vector X (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }
  err = cudaFree(d_V);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to free device vector V (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }
  err = cudaFree(d_F);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to free device vector C (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }

  err = cudaFree(d_Mass);

  if (err != cudaSuccess)
  {
      fprintf(stderr, "Failed to free device vector Mass (error code %s)!\n", cudaGetErrorString(err));
      exit(EXIT_FAILURE);
  }
}
