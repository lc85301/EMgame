#include<iostream>
#include<fstream>
#include<string>
#include <sstream>
#include <deque>
#include "FDTD2D.h"
#include "button.h"
#define FDTDSIZE 290
#define D_CELL 20
#define RATIO 500/(FDTDSIZE-2*D_CELL)
#define savecolumns 5
//TODO money
//TODO hard/soft/source
using namespace std;
enum brushStyle{Square,Circle,Line};
enum wheelMode{bSize,MaxF};
enum fieldType{Ex,Ey,Hz};
void set_mesh(mesh* Mesh,int X,int Y,double Ds, double d_cell);
void reset_mesh(mesh* Mesh, int X,int Y);
void clear_mesh(mesh* Mesh, int X,int Y);
void save_field(mesh*,int,int,bool);
void load_field(mesh*,int,int);
mesh* FDTD2DInit(double, double, int, int);
void update_TEz(mesh* Mesh,int X,int Y);
SDL_Surface* InitialSetting(string,int,int,int);
double source(int,int,double,bool);
GLuint loadTexture( const std::string &fileName );
bool sourceEnable=true;
int main(int argc,char* argv[]){
	const int Matnum=4;
	const int bStylenum=3;
	const int Srcnum=3;
	const int wModenum=2;
	const int fTypenum=3;
	const string Matname[Matnum]={"Air","PEC","PMC","Glass"};
	const string bStylname[bStylenum]={"Square","Circle","Line"};
	const string Srcname[Srcnum]={"Sin","Pulse","None"};
	const string wModename[wModenum]={"Brushsize","MaxField"};
	const string fTypename[fTypenum]={"Ex","Ey","Hz"};
	const float MatR[Matnum]={0,0.7,0.3,0.1};
	const float MatG[Matnum]={0,0.7,0.3,0.1};
	const float MatB[Matnum]={0,0.7,0.3,0.1};
    SDL_Surface* surface=NULL;
    surface=InitialSetting("EMEMU",500,700,FDTDSIZE);
    mesh* Mesh=FDTD2DInit(0.025,5e-11,FDTDSIZE,FDTDSIZE);
    bool isRunning=true;
    bool isDrag=false;
    int X,Y,Xi,Yi;
    int brushsize=20;
    double Maxfield=125;
    bool isRight=false;
    bool isPause=false;
    bool isReset=false;
    bool isRelease=false;
    bool isClear=false;
    bool isSave=false;
    bool isLoad=false;
    bool isMonitor1=false;
    bool isMonitor2=false;
    int MonitorX1=125;
    int MonitorY1=125;
    int MonitorX2=125;
    int MonitorY2=125;
    int Startx,Starty;
    int Endx,Endy;
    int saveY=D_CELL;
    const int buttonMidx=20;
    const int buttonMidy=520;
    SDL_Event event;
    matype currentMat=Air;
    brushStyle currentbStyle=Square;
    srctype currentSrc=None;
    wheelMode currentwMode=bSize;
    fieldType currentfType=Hz;
    button Matv[Matnum];
    button bStylev[bStylenum];
    button Srcv[Srcnum];
    button wModev[wModenum];
    button fTypev[fTypenum];
    button Monitor1,Monitor2;
    deque<mesh> Monitorvalue1;
    deque<mesh> Monitorvalue2;
    for(int i=0;i<Matnum;i++){
		Matv[i].button_set(buttonMidx,buttonMidy+16*i,Matname[i],MatR[i],MatG[i],MatB[i]);
    }
    for(int i=0;i<bStylenum;i++){
		bStylev[i].button_set(buttonMidx+400,buttonMidy+16*i,bStylname[i]);
    }
    for(int i=0;i<Srcnum;i++){
		Srcv[i].button_set(buttonMidx,buttonMidy+16*(i+5),Srcname[i]);
    }
    for(int i=0;i<wModenum;i++){
		wModev[i].button_set(buttonMidx+100,buttonMidy+16*i,wModename[i]);
    }
    for(int i=0;i<fTypenum;i++){
		fTypev[i].button_set(buttonMidx+400,buttonMidy+16*(i+4),fTypename[i]);
    }
    Monitor1.button_set((MonitorX1+0.5-D_CELL)*RATIO,(MonitorY2+0.5-D_CELL)*RATIO,"o",0,1,0);
    Monitor2.button_set((MonitorX2+0.5-D_CELL)*RATIO,(MonitorY2+0.5-D_CELL)*RATIO,"o",1,0,0);

    while(isRunning){
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT)
                isRunning=false;
            if(event.type==SDL_MOUSEBUTTONDOWN){
                if(event.button.button==SDL_BUTTON_LEFT){
                    isRight=false;
                    isDrag=true;
                    Xi=event.button.x;
                    Yi=event.button.y;
					for(int i=0;i<Matnum;i++){
						if(Matv[i].isHit(Xi,Yi)){
							currentMat=(matype)i;
						}
					}
					for(int i=0;i<bStylenum;i++){
						if(bStylev[i].isHit(Xi,Yi)){
							currentbStyle=(brushStyle)i;
						}
					}
					for(int i=0;i<Srcnum;i++){
						if(Srcv[i].isHit(Xi,Yi)){
							currentSrc=(srctype)i;
						}
					}
					for(int i=0;i<wModenum;i++){
						if(wModev[i].isHit(Xi,Yi)){
							currentwMode=(wheelMode)i;
						}
					}
					for(int i=0;i<fTypenum;i++){
						if(fTypev[i].isHit(Xi,Yi)){
							currentfType=(fieldType)i;
						}
					}
					if(Monitor1.isHit(Xi,Yi)){
						isMonitor1=true;
					}else if(Monitor2.isHit(Xi,Yi)){
						isMonitor2=true;
					}
                    X=D_CELL+event.button.x*(FDTDSIZE-2*D_CELL)/500;
                    Y=D_CELL+event.button.y*(FDTDSIZE-2*D_CELL)/500;
                    Startx=X;
                    Starty=Y;
                }else if(event.button.button==SDL_BUTTON_RIGHT){
                    isRight=true;
                    isDrag=true;
                    Xi=event.button.x;
                    Yi=event.button.y;
                    X=D_CELL+event.button.x*(FDTDSIZE-2*D_CELL)/500;
                    Y=D_CELL+event.button.y*(FDTDSIZE-2*D_CELL)/500;
                    Startx=X;
                    Starty=Y;
                }else if(event.button.button==SDL_BUTTON_WHEELUP){
                	switch(currentwMode){
					case bSize:
						brushsize+=1;
						break;
					case MaxF:
						Maxfield*=2;
						break;
					}
                }else if(event.button.button==SDL_BUTTON_WHEELDOWN){
                	switch(currentwMode){
					case bSize:
						brushsize-=(brushsize>1)?1:0;
						break;
					case MaxF:
						Maxfield/=(Maxfield>1e-4)?2.0:1;
						break;
                	}
                }
            }
            else if(event.type==SDL_MOUSEMOTION){
				Xi=event.button.x;
				Yi=event.button.y;
                X=D_CELL+event.button.x*(FDTDSIZE-2*D_CELL)/500;
                Y=D_CELL+event.button.y*(FDTDSIZE-2*D_CELL)/500;
            }
            if(event.type==SDL_MOUSEBUTTONUP){
				if(event.button.button==SDL_BUTTON_LEFT){
					isDrag=false;
					Endx=D_CELL+event.button.x*(FDTDSIZE-2*D_CELL)/500;
					Endy=D_CELL+event.button.y*(FDTDSIZE-2*D_CELL)/500;
					isRelease=true;
					isMonitor1=false;
					isMonitor2=false;
				}else if(event.button.button==SDL_BUTTON_RIGHT){
					isDrag=false;
					Endx=D_CELL+event.button.x*(FDTDSIZE-2*D_CELL)/500;
					Endy=D_CELL+event.button.y*(FDTDSIZE-2*D_CELL)/500;
					isRelease=true;
				}
            }
            if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                case SDLK_p:
                    isPause=!isPause;
                    break;
                case SDLK_r:
                    isReset=true;
                    break;
                case SDLK_c:
                    isClear=true;
                    break;
                case SDLK_s:
                    isSave=true;
                    break;
                case SDLK_l:
                    isLoad=true;
                    break;
                case SDLK_ESCAPE:
                    isRunning=false;
                    break;
                default:
                    ;
                }
            }
        }
        if(isDrag&&!isSave){
            if(isRight){

			}else if(Xi>=0&&Xi<500&&Yi>=0&&Yi<500){
            	switch(currentbStyle){
				case Square:
					for(int i=X-brushsize;i<X+brushsize;i++){
						for(int j=Y-brushsize;j<Y+brushsize;j++){
							if(i>=D_CELL&&i<FDTDSIZE-D_CELL&&j<FDTDSIZE-D_CELL&&j>=D_CELL)
							Mesh[i*FDTDSIZE+j].set_material(currentMat);
						}
					}
					break;
				case Circle:
					for(int i=X-brushsize;i<X+brushsize;i++){
						for(int j=Y-brushsize;j<Y+brushsize;j++){
							if(i>=D_CELL&&i<FDTDSIZE-D_CELL&&j<FDTDSIZE-D_CELL&&j>=D_CELL&&((i-X)*(i-X)+(j-Y)*(j-Y)<brushsize*brushsize))
							Mesh[i*FDTDSIZE+j].set_material(currentMat);
						}
					}
					break;
            	}
            }
        }
        if(isRelease&&!isSave){
			if(!isRight){
				if(Xi>0&&Xi<500&&Yi>0&&Yi<500){
					if(currentbStyle==Line){
						double dX=Endx-Startx;
						double dY=Endy-Starty;
						double norm=sqrt(dX*dX+dY*dY);
						double polyX[4]={Startx-brushsize*dY/norm,Startx+brushsize*dY/norm,Endx+brushsize*dY/norm,Endx-brushsize*dY/norm};
						double polyY[4]={Starty+brushsize*dX/norm,Starty-brushsize*dX/norm,Endy-brushsize*dX/norm,Endy+brushsize*dX/norm};
						int maxY=round((dY>0)?(dX>0)?polyY[3]:polyY[2]:(dX>0)?polyY[0]:polyY[1]);
						int minY=round((dY>0)?(dX>0)?polyY[1]:polyY[0]:(dX>0)?polyY[2]:polyY[3]);
						int nodeX[2]={0,0};
						for(int y=minY;y<maxY;y++){
							int node=0;
							int k=3;
							for(int i=0;i<4;i++){
								if((polyY[i]<(double)y&&polyY[k]>=(double)y)||(polyY[i]>=(double)y&&polyY[k]<(double)y)){
									nodeX[node]=(int)(polyX[i]+(y-polyY[i])/(polyY[k]-polyY[i])*(polyX[k]-polyX[i]));
									node++;
								}
								k=i;
							}
							for(int i=min(nodeX[0],nodeX[1]);i<max(nodeX[0],nodeX[1]);i++)
								Mesh[i*FDTDSIZE+y].set_material(currentMat);

						}
					}
				}
			}else{
				if(currentSrc!=None){
					if(abs(Endx-Startx)>abs(Endy-Starty)){
						for(int i=min(Startx,Endx);i<=max(Startx,Endx);i++){
							if(i>D_CELL&&i<FDTDSIZE-D_CELL&&Starty<FDTDSIZE-D_CELL&&Starty>D_CELL)
							Mesh[i*FDTDSIZE+Starty].Srctype=currentSrc;
						}
					}else{
						for(int j=min(Starty,Endy);j<=max(Starty,Endy);j++){
							if(Startx>D_CELL&&Startx<FDTDSIZE-D_CELL&&j<FDTDSIZE-D_CELL&&j>D_CELL)
							Mesh[Startx*FDTDSIZE+j].Srctype=currentSrc;
						}
					}
				}else{
					for(int i=min(Startx,Endx);i<=max(Startx,Endx);i++){
						for(int j=min(Starty,Endy);j<=max(Starty,Endy);j++){
							if(i>D_CELL&&i<FDTDSIZE-D_CELL&&j<FDTDSIZE-D_CELL&&j>D_CELL)
							Mesh[i*FDTDSIZE+j].Srctype=currentSrc;
						}
					}
				}
				isRight=false;
			}
			isRelease=false;
        }
        if(isMonitor1){
			MonitorX1=X;
			MonitorY1=Y;
			Monitor1.button_set((MonitorX1-D_CELL-0.5)*RATIO,(MonitorY1-D_CELL)*RATIO,"o",0,1,0);
		}
        if(isMonitor2){
			MonitorX2=X;
			MonitorY2=Y;
			Monitor2.button_set((MonitorX2-D_CELL-0.5)*RATIO,(MonitorY2-D_CELL)*RATIO,"o",1,0,0);
		}
        if(isReset){
            reset_mesh(Mesh,FDTDSIZE,FDTDSIZE);
            isReset=false;
        }
        if(isClear){
            reset_mesh(Mesh,FDTDSIZE,FDTDSIZE);
            clear_mesh(Mesh,FDTDSIZE,FDTDSIZE);
            isClear=false;
        }
        if(isSave){
			if(saveY==FDTDSIZE-D_CELL){
				saveY=D_CELL;
				isSave=false;
			}else if(saveY==D_CELL){
				save_field(Mesh,FDTDSIZE,saveY,true);
				saveY+=savecolumns;
			}else{
				save_field(Mesh,FDTDSIZE,saveY,false);
				saveY+=savecolumns;
			}
        }else if(isLoad){
			load_field(Mesh,FDTDSIZE,FDTDSIZE);
			isLoad=false;
        }
        else if(!isPause){
            update_TEz(Mesh,FDTDSIZE,FDTDSIZE);
        }
        if(Monitorvalue1.size()==250){
			Monitorvalue1.pop_back();
        }
        if(Monitorvalue2.size()==250){
			Monitorvalue2.pop_back();
        }
        Monitorvalue1.push_front(Mesh[MonitorX1*FDTDSIZE+MonitorY1]);
        Monitorvalue2.push_front(Mesh[MonitorX2*FDTDSIZE+MonitorY2]);
        //rendering
        glClear(GL_COLOR_BUFFER_BIT);
        glPointSize(ceil(500.0/(FDTDSIZE-2*D_CELL)));
        glPushMatrix();
        glLoadIdentity();
        glBegin( GL_POINTS );
        //Drawing Material
            for ( int i = D_CELL; i < FDTDSIZE-D_CELL+1; ++i ){
                for(int j=D_CELL;j<FDTDSIZE-D_CELL+1;j++){
                    float* color=Mesh[i*FDTDSIZE+j].matcolor();
                    glColor4f(color[0],color[1],color[2],color[3]);
                    glVertex2f( (i+0.5-D_CELL)*RATIO,(j+0.5-D_CELL)*RATIO );
                }
            }
		glEnd();
        glBegin( GL_POINTS );
        //Drawing fields
            for ( int i = D_CELL; i < FDTDSIZE-D_CELL+1; ++i ){
                for(int j=D_CELL;j<FDTDSIZE-D_CELL+1;j++){
                    float color;
                    switch(currentfType){
					case Ex:
						color=(Mesh[i*FDTDSIZE+j].Ex)/Maxfield;
						break;
					case Ey:
						color=(Mesh[i*FDTDSIZE+j].Ey)/Maxfield;
						break;
					case Hz:
						color=(Mesh[i*FDTDSIZE+j].Hz)/Maxfield*eta_0;
						break;
                    }
                    if(color>0)
                        glColor4f( 1.0, 0.0, 0.0, color );
                    else
                        glColor4f( 0.0, 0.0, 1.0 ,-color );
                    glVertex2f( (i+0.5-D_CELL)*RATIO,(j+0.5-D_CELL)*RATIO );
                }
            }
        glEnd();
        //Drawing Cursor
        if(Xi>0&&Xi<500&&Yi>0&&Yi<500){
			if(!isRight){
				switch(currentbStyle){
				case Square:
					glBegin(GL_LINE_LOOP);
					glColor4f(1,0,0,1);
					glVertex2f((X+0.5-D_CELL-brushsize)*RATIO,(Y+0.5-D_CELL-brushsize)*RATIO);
					glVertex2f((X+0.5-D_CELL-brushsize)*RATIO,(Y+0.5-D_CELL+brushsize)*RATIO);
					glVertex2f((X+0.5-D_CELL+brushsize)*RATIO,(Y+0.5-D_CELL+brushsize)*RATIO);
					glVertex2f((X+0.5-D_CELL+brushsize)*RATIO,(Y+0.5-D_CELL-brushsize)*RATIO);
					glEnd();
					break;
				case Circle:
					glBegin(GL_LINE_LOOP);
					glColor4f(1,0,0,1);
					for(int i=0;i<30;i++)
						glVertex2f((X+0.5-D_CELL-brushsize*cos(2*pi/30.0*i))*RATIO,(Y+0.5-D_CELL-brushsize*sin(2*pi/30.0*i))*RATIO);
					glEnd();
					break;
				case Line:
					if(isDrag){
						double dX=X-Startx;
						double dY=Y-Starty;
						double norm=sqrt(dX*dX+dY*dY);
						glBegin(GL_LINE_LOOP);
						glColor4f(1,0,0,1);
						glVertex2f((Startx+0.5-D_CELL-brushsize*dY/norm)*RATIO,(Starty+0.5-D_CELL+brushsize*dX/norm)*RATIO);
						glVertex2f((Startx+0.5-D_CELL+brushsize*dY/norm)*RATIO,(Starty+0.5-D_CELL-brushsize*dX/norm)*RATIO);
						glVertex2f((X+0.5-D_CELL+brushsize*dY/norm)*RATIO,(Y+0.5-D_CELL-brushsize*dX/norm)*RATIO);
						glVertex2f((X+0.5-D_CELL-brushsize*dY/norm)*RATIO,(Y+0.5-D_CELL+brushsize*dX/norm)*RATIO);
						glEnd();
					}else{
						glBegin(GL_LINES);
						glColor4f(1,0,0,1);
						glVertex2f((X+0.5-D_CELL-brushsize)*RATIO,(Y+0.5-D_CELL)*RATIO);
						glVertex2f((X+0.5-D_CELL+brushsize)*RATIO,(Y+0.5-D_CELL)*RATIO);
						glVertex2f((X+0.5-D_CELL)*RATIO,(Y+0.5-D_CELL+brushsize)*RATIO);
						glVertex2f((X+0.5-D_CELL)*RATIO,(Y+0.5-D_CELL-brushsize)*RATIO);
						glEnd();
					}
					break;
				}
			}else{
				if(currentSrc!=None){
					if(abs(X-Startx)>abs(Y-Starty)){
						glBegin(GL_LINES);
						glColor4f(1,0,0,1);
						glVertex2f((Startx+0.5-D_CELL)*RATIO,(Starty+0.5-D_CELL)*RATIO);
						glVertex2f((X+0.5-D_CELL)*RATIO,(Starty+0.5-D_CELL)*RATIO);
						glEnd();
					}else{
						glBegin(GL_LINES);
						glColor4f(1,0,0,1);
						glVertex2f((Startx+0.5-D_CELL)*RATIO,(Starty+0.5-D_CELL)*RATIO);
						glVertex2f((Startx+0.5-D_CELL)*RATIO,(Y+0.5-D_CELL)*RATIO);
						glEnd();
					}
				}else{
					glBegin(GL_LINE_LOOP);
					glColor4f(1,0,0,1);
					glVertex2f((Startx+0.5-D_CELL)*RATIO,(Starty+0.5-D_CELL)*RATIO);
					glVertex2f((Startx+0.5-D_CELL)*RATIO,(Y+0.5-D_CELL)*RATIO);
					glVertex2f((X+0.5-D_CELL)*RATIO,(Y+0.5-D_CELL)*RATIO);
					glVertex2f((X+0.5-D_CELL)*RATIO,(Starty+0.5-D_CELL)*RATIO);
					glEnd();
				}
			}
        }
        //Drawing Back
        glBegin(GL_QUADS);
        glColor4f(0.5,0.5,0.5,1);
        glVertex2f(0,500);
        glVertex2f(500,500);
        glVertex2f(500,700);
        glVertex2f(0,700);
        glEnd();
        //Drawing Button
		for(int i=0;i<Matnum;i++){
			Matv[i].draw();
		}
		for(int i=0;i<bStylenum;i++){
			bStylev[i].draw();
		}
		for(int i=0;i<Srcnum;i++){
			Srcv[i].draw();
		}
		for(int i=0;i<wModenum;i++){
			wModev[i].draw();
		}
		for(int i=0;i<fTypenum;i++){
			fTypev[i].draw();
		}
		Monitor1.draw();
		Monitor2.draw();
		ostringstream s;
		s.precision(1);
		s<<scientific<<Maxfield;
		button mFieldtext(wModev[1].X+wModev[1].W+16,wModev[1].Y,s.str(),1,0,0,14);
		s.str("");
		s<<brushsize;
		button bSizetext(wModev[0].X+wModev[0].W+16,wModev[0].Y,s.str(),1,0,0,14);
		mFieldtext.draw();
		bSizetext.draw();
		if(isSave){
			float color=(((saveY-D_CELL)/savecolumns)%5)/5.0;
			button savetext(440,492,"Saving",color,color,color,16);
			savetext.draw();
		}
        //indicator
        const int offset=5;
        glBegin(GL_QUADS);
        glColor4f(1.0,0,0,1);
        glVertex2f(Matv[(int)currentMat].X-2-offset,Matv[(int)currentMat].Y-2);
        glVertex2f(Matv[(int)currentMat].X-2-offset,Matv[(int)currentMat].Y+2);
        glVertex2f(Matv[(int)currentMat].X+2-offset,Matv[(int)currentMat].Y+2);
        glVertex2f(Matv[(int)currentMat].X+2-offset,Matv[(int)currentMat].Y-2);
        glVertex2f(bStylev[(int)currentbStyle].X-2-offset,bStylev[(int)currentbStyle].Y-2);
        glVertex2f(bStylev[(int)currentbStyle].X-2-offset,bStylev[(int)currentbStyle].Y+2);
        glVertex2f(bStylev[(int)currentbStyle].X+2-offset,bStylev[(int)currentbStyle].Y+2);
        glVertex2f(bStylev[(int)currentbStyle].X+2-offset,bStylev[(int)currentbStyle].Y-2);
        glVertex2f(Srcv[(int)currentSrc].X-2-offset,Srcv[(int)currentSrc].Y-2);
        glVertex2f(Srcv[(int)currentSrc].X-2-offset,Srcv[(int)currentSrc].Y+2);
        glVertex2f(Srcv[(int)currentSrc].X+2-offset,Srcv[(int)currentSrc].Y+2);
        glVertex2f(Srcv[(int)currentSrc].X+2-offset,Srcv[(int)currentSrc].Y-2);
        glVertex2f(wModev[(int)currentwMode].X-2-offset,wModev[(int)currentwMode].Y-2);
        glVertex2f(wModev[(int)currentwMode].X-2-offset,wModev[(int)currentwMode].Y+2);
        glVertex2f(wModev[(int)currentwMode].X+2-offset,wModev[(int)currentwMode].Y+2);
        glVertex2f(wModev[(int)currentwMode].X+2-offset,wModev[(int)currentwMode].Y-2);
        glVertex2f(fTypev[(int)currentfType].X-2-offset,fTypev[(int)currentfType].Y-2);
        glVertex2f(fTypev[(int)currentfType].X-2-offset,fTypev[(int)currentfType].Y+2);
        glVertex2f(fTypev[(int)currentfType].X+2-offset,fTypev[(int)currentfType].Y+2);
        glVertex2f(fTypev[(int)currentfType].X+2-offset,fTypev[(int)currentfType].Y-2);
        glEnd();
		glBegin(GL_QUADS);
        glColor4f(0.0,0,0,1);
        glVertex2f(buttonMidx+100,570);
        glVertex2f(buttonMidx+100,670);
        glVertex2f(buttonMidx+350,670);
        glVertex2f(buttonMidx+350,570);
		glEnd();

        glBegin(GL_LINE_STRIP);
        glColor4f(0,1.0,0,1);
        for(int i=0;i<Monitorvalue1.size();i++){
			double y;
			switch(currentfType){
			case Ex:
				y=Monitorvalue1[i].Ex*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			case Ey:
				y=Monitorvalue1[i].Ey*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			case Hz:
				y=Monitorvalue1[i].Hz*eta_0*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			}
			glVertex2f(100+buttonMidx+i,620+y);
        }
        glEnd();
		glBegin(GL_LINE_STRIP);
        glColor4f(1,0,0,1);
        for(int i=0;i<Monitorvalue2.size();i++){
			double y;
			switch(currentfType){
			case Ex:
				y=Monitorvalue2[i].Ex*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			case Ey:
				y=Monitorvalue2[i].Ey*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			case Hz:
				y=Monitorvalue2[i].Hz*eta_0*50.0/Maxfield;
				y=(y>50)?50:(y<-50)?-50:y;
				break;
			}
			glVertex2f(100+buttonMidx+i,620+y);
        }
        glEnd();

        glPopMatrix();
        SDL_GL_SwapBuffers();
    }
    SDL_Quit();
    return 0;
}
SDL_Surface* InitialSetting(string title,int width,int height,int FDTDsize){
	SDL_Surface* surface=NULL;
    //initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
    //Set OpenGL memory usage
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    //Caption of the window
    SDL_WM_SetCaption( title.c_str(), NULL );

    //Size of the window
    surface=SDL_SetVideoMode(width,height,32,SDL_OPENGL);

//    Specific the clear color
    glClearColor(0,0,0,1); //RED,GREEN,BLUE,ALPHA

    //What portion of the screen we will display
    glOrtho(0,width,height,0,-10,10);

    //Shader model - Use this
    glShadeModel(GL_SMOOTH);

    //2D rendering
    glMatrixMode(GL_PROJECTION);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
    //"Save" it
    glLoadIdentity();

    //Disable depth checking
    glDisable(GL_DEPTH_TEST);

    glEnable( GL_TEXTURE_2D );
    return surface;
}
void set_mesh(mesh* Mesh,int X,int Y,double Ds, double Dt,double d_cell){
    int m=2;
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            Mesh[i*FDTDSIZE+j].set_size(Ds,Dt);
        }
    }
    for(int j=0;j<d_cell;j++){
        for(int i=0;i<X;i++){
            double d_E=(d_cell-j)/d_cell;
            double d_M=(d_cell-j-0.5)/d_cell;
            double smax=0.8*(m+1)/Mesh[i*FDTDSIZE+j].m.eta/Ds;
            Mesh[i*FDTDSIZE+j].set_PML("y",pow(d_E,m)*smax,pow(d_M,m)*smax*mu_0/epsilon_0);
            d_M=(d_cell-j+0.5)/d_cell;
            Mesh[i*FDTDSIZE+Y-1-j].set_PML("y",pow(d_E,m)*smax,pow(d_M,m)*smax*mu_0/epsilon_0);
        }
    }
    for(int i=0;i<d_cell;i++){
        for(int j=0;j<Y;j++){
            double d_E=(d_cell-i)/d_cell;
            double d_M=(d_cell-i-0.5)/d_cell;
            double smax=0.8*(m+1)/(Mesh[i*FDTDSIZE+j].m.eta*Ds);
            Mesh[i*FDTDSIZE+j].set_PML("x",pow(d_E,m)*smax,pow(d_M,m)*smax*mu_0/epsilon_0);
            d_M=(d_cell-i+0.5)/d_cell;
            Mesh[(X-1-i)*FDTDSIZE+j].set_PML("x",pow(d_E,m)*smax,pow(d_M,m)*smax*mu_0/epsilon_0);
        }
    }
    cout<<"Mesh Initialized\n";
}
void reset_mesh(mesh* Mesh,int X, int Y){
    for(int i=0;i<X;i++){
        for(int j=0;j<Y;j++){
            Mesh[i*FDTDSIZE+j].reset();
        }
    }
}
void clear_mesh(mesh* Mesh,int X, int Y){
    for(int i=D_CELL;i<X-D_CELL;i++){
        for(int j=D_CELL;j<Y-D_CELL;j++){
            Mesh[i*FDTDSIZE+j].clearall();
        }
    }
}
void update_TEz(mesh* Mesh,int X,int Y){
    for(int i=1;i<X-1;i++){
        for(int j=1;j<Y-1;j++){
            Mesh[i*FDTDSIZE+j].Hx=Mesh[i*FDTDSIZE+j].Dax*Mesh[i*FDTDSIZE+j].Hx-Mesh[i*FDTDSIZE+j].Dbx*(Mesh[(i+1)*FDTDSIZE+j].Ey-Mesh[i*FDTDSIZE+j].Ey);
            Mesh[i*FDTDSIZE+j].Hy=Mesh[i*FDTDSIZE+j].Day*Mesh[i*FDTDSIZE+j].Hy+Mesh[i*FDTDSIZE+j].Dby*(Mesh[i*FDTDSIZE+j+1].Ex-Mesh[i*FDTDSIZE+j].Ex);
            Mesh[i*FDTDSIZE+j].update_Hz();
			Mesh[i*FDTDSIZE+j].update_src();
            Mesh[i*FDTDSIZE+j].Ex=Mesh[i*FDTDSIZE+j].Cax*Mesh[i*FDTDSIZE+j].Ex+Mesh[i*FDTDSIZE+j].Cbx*(Mesh[i*FDTDSIZE+j].Hz-Mesh[i*FDTDSIZE+j-1].Hz);
            Mesh[i*FDTDSIZE+j].Ey=Mesh[i*FDTDSIZE+j].Cay*Mesh[i*FDTDSIZE+j].Ey-Mesh[i*FDTDSIZE+j].Cby*(Mesh[i*FDTDSIZE+j].Hz-Mesh[(i-1)*FDTDSIZE+j].Hz);
        }
    }
}
mesh* FDTD2DInit(double Ds, double Dt, int X, int Y){
    //mesh ** Mesh=new mesh*[X];
    mesh * Mesh=new mesh[X*Y];
    //for(int i=0;i<X;i++){
    //    Mesh[i]=new mesh[Y];
    //}
    set_mesh(Mesh,X,Y,Ds,Dt,D_CELL);
//    for(int n=0;n<T;n++){
//        update_TEz(Mesh,X,Y,n);
//    }
    return Mesh;
}
void save_field(mesh* Mesh,int X, int Y, bool first){
	ofstream myfile;
	if(first)
		myfile.open("savefield.txt");
	else
		myfile.open("savefield.txt",ios::app);
	if(!myfile.is_open())
		cout<<"fail to open"<<endl;
	for(int j=Y;j<Y+savecolumns;j++){
		for(int i=D_CELL;i<X-D_CELL;i++){
			myfile<<(int)Mesh[i*FDTDSIZE+j].m.type<<" "<<(int)Mesh[i*FDTDSIZE+j].Srctype<<" "<<Mesh[i*FDTDSIZE+j].Ex<<" "<<Mesh[i*FDTDSIZE+j].Ey<<" "<<Mesh[i*FDTDSIZE+j].Hx<<" "<<Mesh[i*FDTDSIZE+j].Hy<<" "<<Mesh[i*FDTDSIZE+j].sourceTimer<<" "<<Mesh[i*FDTDSIZE+j].AccumSource<<endl;
		}
	}
	myfile.close();
}
void load_field(mesh* Mesh,int X, int Y){
	//clear_mesh(Mesh,X,Y);
	ifstream myfile;
	myfile.open("savefield.txt");
	if(!myfile.is_open())
		cout<<"fail to open"<<endl;
	for(int j=D_CELL;j<X-D_CELL;j++){
		for(int i=D_CELL;i<Y-D_CELL;i++){
			string buffer;
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].set_material((matype)atoi(buffer.c_str()));
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].Srctype=(srctype)atoi(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].Ex=atof(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].Ey=atof(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].Hx=atof(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].Hy=atof(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].sourceTimer=atof(buffer.c_str());
			myfile>>buffer;
			Mesh[i*FDTDSIZE+j].AccumSource=atof(buffer.c_str());
		}
	}
	myfile.close();
}

