#include "FDTD2D.h"

#define sigma 10

//__device__ double
//Hsource(int type, double t){
//	switch(type){
//	case Sin:
//		return (t>0)?atan(t/20)*sin((t*1e-2*2*3.14)):0;
//		break;
//	case Pulse:
//		return (t>0)?exp(-0.5*(t-3*sigma)*(t-3*sigma)/(sigma*sigma)):0;
//		break;
//	case None:
//		return 0;
//		break;
//	}
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
		m[k].AccumSource += Hsource(m[k].Srctype, m[k].sourceTimer);
		m[k].Hz+=m[k].AccumSource;
		m[k].sourceTimer=(m[k].Srctype==None)?0:m[k].sourceTimer+1;
	}
}

#define BLOCKSIZ 32

extern "C"
void cudaUpdateKernel(mesh* d_m, int Nx, int Ny){
	dim3 dimBlock(BLOCKSIZ,BLOCKSIZ);
	dim3 dimGrid(ceil(Nx/BLOCKSIZ), ceil(Ny/BLOCKSIZ));
	updateH<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateSource<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
	updateE<<<dimGrid, dimBlock>>>(d_m, Nx, Ny);
}

