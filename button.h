#include "text_rendering.h"
#include<iostream>

using namespace std;
class button{
public:
	button():X(0),Y(0),Name("none"),R(1.0),G(1.0),B(1.0),textsize(16){
	}
    button(int x,int y, string name){
		button_set(x,y,name);
    }
    button(int x,int y, string name,float r,float g,float b,int Textsize){
		button_set(x,y,name,r,g,b);
		textsize=Textsize;
    }
    void button_set(int x,int y, string name){
		X=x;
		Y=y;
		Name=name;
		R=1.0;
		G=1.0;
		B=1.0;
    }
    void button_set(int x,int y, string name,float r,float g,float b){
		X=x;
		Y=y;
		Name=name;
		R=r;
		G=g;
		B=b;
    }
    void draw(){
        TTF_Font *font=TTF_OpenFont("./Tiger Expert.ttf",textsize);
        float Color[3]={R,G,B};
        TTF_SetFontStyle(font,TTF_STYLE_BOLD);
        RenderText(font,Color[0],Color[1],Color[2],X,Y,Name,W,H);
        TTF_CloseFont(font);
	}
	bool isHit(float x,float y){
		return (x<X+W)&&(x>X)&&(y<Y+H/2.0)&&(y>Y-H/2.0);
	}
    int textsize;
    int X,Y,W,H;
private:
    string Name;
    float R,G,B;
};
