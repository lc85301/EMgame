#include "FDTD2D.h"
//To Merge
material::material(){
    epsilon=epsilon_0;
    mu=mu_0;
    this->sigma_x=0;
    this->sigma_y=0;
    this->msigma_x=0;
    this->msigma_y=0;
    eta=sqrt(mu/epsilon);
    c=1/sqrt(mu*epsilon);
    type=Air;
}
material::material(double mu, double epsilon){
    this->epsilon=epsilon;
    this->mu=mu;
    this->sigma_x=0;
    this->sigma_y=0;
    this->msigma_x=0;
    this->msigma_y=0;
    this->eta=sqrt(mu/epsilon);
    this->c=1/sqrt(mu*epsilon);
}
void material::update(){
    eta=sqrt(mu/epsilon);
    c=1/sqrt(mu*epsilon);
}
mesh::mesh(){
    Ex=0;
    Ey=0;
    Ez=0;
    Hx=0;
    Hy=0;
    Hz=0;
    Ds=1;
    Dt=1;
    update_C();
    update_D();
    sourceTimer=0;
    Srctype=None;
    AccumSource=0;
}
mesh::mesh(double mu=mu_0, double epsilon=epsilon_0){
    Ex=0;
    Ey=0;
    Ez=0;
    Hx=0;
    Hy=0;
    Hz=0;
    Ds=1;
    Dt=1;
    m.mu=mu;
    m.epsilon=epsilon;
    m.update();
    update_C();
    update_D();
    sourceTimer=0;
    Srctype=None;
    AccumSource=0;
}
void mesh::reset(){
    Ex=0;
    Ey=0;
    Ez=0;
    Hx=0;
    Hy=0;
    Hz=0;
    sourceTimer=0;
    AccumSource=0;
}
void mesh::clearall(){
	reset();
	set_material(Air);
	Srctype=None;
    AccumSource=0;
}
void mesh::set_material(double mu=mu_0, double epsilon=epsilon_0){
    m.mu=mu;
    m.epsilon=epsilon;
    m.update();
    update_C();
    update_D();
}
void mesh::set_material(double mu, double epsilon, double sigma, double msigma){
    m.mu=mu;
    m.epsilon=epsilon;
    m.sigma_x=sigma;
    m.sigma_y=sigma;
    m.msigma_x=msigma;
    m.msigma_y=msigma;
    m.update();
    update_C();
    update_D();
}
void mesh::set_PML(std::string type,double sigma, double msigma){
    if(type=="x"){
        m.sigma_x=sigma;
        m.msigma_x=msigma;
    }else if(type=="y"){
        m.sigma_y=sigma;
        m.msigma_y=msigma;
    }
    update_C();
    update_D();
}
void mesh::set_material(matype type){
    m.type=type;
    switch(type){
    case PEC:
        Cax=-1;Cay=-1;Cbx=0;Cby=0;
        update_D();
        Ex=0;
        Ey=0;
        Hx=0;
        Hy=0;
        Hz=0;
        break;
    case PMC:
        Dax=-1;Day=-1;Dbx=0;Dby=0;
        update_C();
        Ex=0;
        Ey=0;
        Hx=0;
        Hy=0;
        Hz=0;
        break;
    case Air:
        set_material(mu_0,epsilon_0,0,0);
        break;
	case Glass:
		set_material(mu_0,3.5*epsilon_0,0,0);
		break;
    }
}
void mesh::set_size(double Ds, double Dt){
    this->Ds=Ds;
    this->Dt=Dt;
    update_C();
    update_D();
}
void mesh::update_C(){
    Cax=(2*m.epsilon-m.sigma_y*Dt)/(2*m.epsilon+m.sigma_y*Dt);
    Cay=(2*m.epsilon-m.sigma_x*Dt)/(2*m.epsilon+m.sigma_x*Dt);
    Cbx=(2*Dt)/(2*m.epsilon*Ds+m.sigma_y*Dt*Ds);
    Cby=(2*Dt)/(2*m.epsilon*Ds+m.sigma_x*Dt*Ds);
}
void mesh::update_D(){
    Dax=(2*m.mu-m.msigma_x*Dt)/(2*m.mu+m.msigma_x*Dt);
    Day=(2*m.mu-m.msigma_y*Dt)/(2*m.mu+m.msigma_y*Dt);
    Dbx=(2*Dt)/(2*m.mu*Ds+m.msigma_x*Ds*Dt);
    Dby=(2*Dt)/(2*m.mu*Ds+m.msigma_y*Ds*Dt);
}
void mesh::update_Ez(){
    Ez=Ex+Ey;
}
void mesh::update_Hz(){
    Hz=Hx+Hy;
}
float* mesh::matcolor(){
    float color[4];
    switch(m.type){
    case PEC:
        color[0]=0.7;
        color[1]=0.7;
        color[2]=0.7;
        color[3]=1;
        break;
    case PMC:
        color[0]=0.3;
        color[1]=0.3;
        color[2]=0.3;
        color[3]=1;
        break;
    case Air:
        color[0]=0;
        color[1]=0;
        color[2]=0;
        color[3]=0;
        break;
	case Glass:
        color[0]=0.1;
        color[1]=0.1;
        color[2]=0.1;
        color[3]=1;
        break;
    }
    return color;
}
double mesh::Hsource(double t){
    double sigma=10;
	switch(Srctype){
	case Sin:
		return (t>0)?atan(t/20)*sin((t*1e-2*2*3.14)):0;
		break;
	case Pulse:
		return (t>0)?exp(-0.5*(t-3*sigma)*(t-3*sigma)/(sigma*sigma)):0;
		break;
	case None:
		return 0;
		break;
	}
}
void mesh::update_src(){
    AccumSource+=Hsource(sourceTimer);//(Cbx*Dby+Cby*Dbx)*AccumSource;
    Hz+=AccumSource;
//Hz+=Hsource(sourceTimer)+2*(Cbx*Dby+Cby*Dbx)*Hsource(sourceTimer-1);
//	if(Srctype!=None)
//		Hz=Hsource(sourceTimer);
    sourceTimer=(Srctype==None)?0:sourceTimer+1;
}
