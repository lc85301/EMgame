#include "FDTD2D.h"

//__device__ double
//source(int i, int j, double t){
//    return (j==200)?8*sin((t*1e10*2*pi)):0;
//}

//H
__global__ void
updateH(mesh* m, int W, int H)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int idy = blockIdx.y * blockDim.y + threadIdx.y;
	int k = idy*W+idx;
	//update
	if(idx<W-1 && idx > 1 && idy > 1 && idy<H-1){
		m[k].Hx=m[k].Dax*m[k].Hx-m[k].Dbx*(m[k+W].Ey-m[k].Ey);
		m[k].Hy=m[k].Day*m[k].Hy+m[k].Dby*(m[k+1].Ex-m[k].Ex);
	}
}

//E
__global__ void
updateE(mesh *m, int W, int H)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int idy = blockIdx.y * blockDim.y + threadIdx.y;
	int k = idy*W+idx;
	//update
	if(idx<W-1 && idx > 1 && idy > 1 && idy<H-1){
		m[k].Ex=m[k].Cax*m[k].Ex+m[k].Cbx*(m[k].Hz-m[k-1].Hz);
		m[k].Ey=m[k].Cay*m[k].Ey-m[k].Cby*(m[k].Hz-m[k-W].Hz);
	}
}

__global__ void 
updateSource(mesh *m, int W, int H, double time){
//	int idx = blockIdx.x * blockDim.x + threadIdx.x;
//	int idy = blockIdx.y * blockDim.y + threadIdx.y;
//	int k = idy*W+idx;
//	if(idx<W && idx>0 && idy>0 && idy<H){
//		double s = source(idx, idy, time);
//		//double Hs = source(idx, idy, time);
//		//double Es = source(idx, idy, time);
//		m[k].Hzy+= m[k].DHx2*	s;
//		m[k+1].Ey	+= m[k+1].CEy2*s/(120*pi);
//	}
}

#define BLOCKSIZ 128

extern "C"
void cudaUpdateKernel(mesh* d_m, int Nx, int Ny, double t){
	dim3 dimBlock(BLOCKSIZ,BLOCKSIZ);
	dim3 dimGrid(ceil(Nx/BLOCKSIZ), ceil(Ny/BLOCKSIZ));
	updateH<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateE<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateSource<<<dimGrid, dimBlock>>>(d_m, Nx, Ny, t);
}

