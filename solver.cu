#include "FDTD2D.h"
#include <stdio.h>

#define sigma 10

__device__ double
Hsource(mesh* m){
    //double sigma=10;
	double t = m->sourceTimer;
	switch(m->Srctype){
	case Sin:
		return sin((t*1e-2*2*3.14));
		break;
	case Pulse: 
		return exp(-0.5*(t-3*sigma)*(t-3*sigma)/(sigma*sigma));
		break;
	case None:
		return 0;
		break;
	}
}
__device__ void 
source(mesh *m, double a){
    //m->AccumSource+=Hsource(m, m->sourceTimer);
    m->AccumSource+=a;
    //m->Hz-=m->AccumSource;
    m->Hz=10;
    m->sourceTimer=(m->Srctype==None)?0:m->sourceTimer+1;
}

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
		m[k].Hz = m[k].Hx+m[k].Hy;
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
updateSource(mesh *m, int W, int H){
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	int idy = blockIdx.y * blockDim.y + threadIdx.y;
	int k = idy*W+idx;
	if(idx<W-1 && idx > 1 && idy > 1 && idy<H-1){
		m[k].Hz = 100;
		//double a = Hsource(&m[k]);
		//source(&m[k], a);
	}
}

#define BLOCKSIZ 128

extern "C"
void cudaUpdateKernel(mesh* d_m, int Nx, int Ny){
	dim3 dimBlock(BLOCKSIZ,BLOCKSIZ);
	dim3 dimGrid(ceil(Nx/BLOCKSIZ), ceil(Ny/BLOCKSIZ));
	updateH<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateE<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateSource<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
}

