#define pi 3.1416
#define epsilon_0 8.85e-12
#define mu_0 1.256e-6
#define eta_0 377
#include <math.h>
#include <string>
#include <vector>
#include<iostream>
enum matype{Air,PEC,PMC,Glass};
enum srctype{Sin,Pulse,None};
class material {
public:
    double mu,epsilon,sigma_x,sigma_y,msigma_x,msigma_y,eta,c;
    material();
    material(double mu, double epsilon);
    void update();
    matype type;
};
class mesh {
public:
    double Ex,Ey,Ez,Hx,Hy,Hz,Ds,Dt,Cax,Cay,Cbx,Cby,Dax,Day,Dbx,Dby;
    double sourceTimer;
    double AccumSource;
    srctype Srctype;
    material m;
    mesh();
    mesh(double mu, double epsilon);
    void set_material(matype type);
    void set_PML(std::string, double, double);
    void set_size(double Ds,double Dt);
    void update_C();
    void update_D();
    void update_Ez();
    void update_Hz();
    void reset();
    void clearall();
    double Hsource(double t);
    void update_src();
    float* matcolor();
private:
    void set_material(double mu, double epsilon);
    void set_material(double mu, double epsilon, double sigma,double msigma);
};
