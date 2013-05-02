#include<string>
#include<SDL/SDL.h>
#include<SDL/SDL_opengl.h>
#include<SDL/SDL_image.h>
#include<SDL/SDL_ttf.h>
void RenderText(const TTF_Font *Font, float R, float G ,float B,
                const double& X, const double& Y, const std::string& Text,int& W,int &H)
	{
		/*Create some variables.*/
        SDL_Color Color={255,255,255};
		SDL_Surface *Message = TTF_RenderText_Blended(const_cast<TTF_Font*>(Font), Text.c_str(), Color);
		unsigned Texture = 0;
		W=Message->w;
		H=Message->h;
		/*Generate an OpenGL 2D texture from the SDL_Surface*.*/
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Message->w, Message->h, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, Message->pixels);

		/*Draw this texture on a quad with the given xyz coordinates.*/
		glBegin(GL_QUADS);
			glColor3f(R,G,B);
			glTexCoord2d(0, 0); glVertex2d(X, Y-Message->h/2.0);
			glTexCoord2d(1, 0); glVertex2d(X+Message->w, Y-Message->h/2.0);
			glTexCoord2d(1, 1); glVertex2d(X+Message->w, Y+Message->h/2.0);
			glTexCoord2d(0, 1); glVertex2d(X, Y+Message->h/2.0);
		glEnd();

		/*Clean up.*/
		glDeleteTextures(1, &Texture);
		SDL_FreeSurface(Message);
}
void RenderText(const TTF_Font *Font, float R, float G ,float B,
                const double& X, const double& Y, const std::string& Text)
	{
		/*Create some variables.*/
        SDL_Color fg={255,255,255};
		SDL_Surface *Message = TTF_RenderText_Blended(const_cast<TTF_Font*>(Font), Text.c_str(), fg);
		unsigned Texture = 0;
		/*Generate an OpenGL 2D texture from the SDL_Surface*.*/
		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Message->w, Message->h, 0, GL_BGRA,
					 GL_UNSIGNED_BYTE, Message->pixels);

		/*Draw this texture on a quad with the given xyz coordinates.*/
		glBegin(GL_QUADS);
			glColor3f(R,G,B);
			glTexCoord2d(0, 0); glVertex2d(X, Y-Message->h/2.0);
			glTexCoord2d(1, 0); glVertex2d(X+Message->w, Y-Message->h/2.0);
			glTexCoord2d(1, 1); glVertex2d(X+Message->w, Y+Message->h/2.0);
			glTexCoord2d(0, 1); glVertex2d(X, Y+Message->h/2.0);
		glEnd();
		/*Clean up.*/
		glDeleteTextures(1, &Texture);
		SDL_FreeSurface(Message);
}
