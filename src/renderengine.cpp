#include <string>
#include "renderengine.h"
#include "resourcemanager.h"
#include "game.h"
#include "memory.h"
#include "oswork.h"
#include "math.h"
#include "gui.h"
#include "collection.h"
#include "stb/stb.h"
#include "stb/stb_truetype.h"
//#include "ft2build.h"
//#include FT_FREETYPE_H
//#include FT_BITMAP_H

#ifdef OS_WINDOWS //其实用WIN32也行...
#pragma comment( lib, "opengl32.lib")
#pragma comment( lib, "glu32.lib")
#endif

//#include "GLFW\glfw3.h"
#include "SDL.h"
#include "SDL_opengl.h"

#ifdef OS_MAC
#include <OpenGL/glu.h> 
#else
#include <GL/glu.h>
#endif

#include "world.h"
#include "util.h"

int RE_InitQuicklyRender();
int RE_InitFontRenderer(int width,int height);
void RE_RenderCubeDoLeft(float lx,float ly,float lz,float rx,float ry,float rz);
void RE_RenderCubeDoCentre(float lx,float ly,float lz,float rx,float ry,float rz);
void RE_RenderCubeDoRight(float lx,float ly,float lz,float rx,float ry,float rz);
void RE_DestroyQuicklyRender();
void RE_DestroyFontRenderer();
void RE_BindChar(wchar_t c);
PFNGLCREATESHADEROBJECTARBPROC glCreateShader = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShader = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSource = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC 	glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
PFNGLUNIFORM4FPROC glUniform4f = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM1IPROC glUniform1i = NULL;

SDL_Window *window = NULL;
static SDL_GLContext glContext = NULL;
static byte *fontData = NULL;
static stbtt_fontinfo fontInfo;
static GLuint charTextures[sizeof(wchar_t) == 2 ? 256 : 4351];
static stbtt_bakedchar *charInfo[sizeof(wchar_t) == 2 ? 256 : 4351];
static BOOL fontRendering = FALSE;
static GLdouble aspect;
static GLuint quicklyRenderList[20]={0};
static int windowWidth;
static int windowHeight;
static GLuint programBackground = 0;
static GLint programBackgroundResolution;
static GLint programBackgroundGlobalTime;
static GLint programBackgroundChannel0;
static GLint programBackgroundSkyColor;

extern World *theWorld;
extern unsigned long long tickTime;

using namespace std;

int RE_InitWindow(int width,int height)
{
	int result;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24); //深度缓冲如果过大会有奇妙的效果,我想也许是因为超过硬件支持的范围后,OpenGL只能采用软件模拟了
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	window = SDL_CreateWindow(WINDOW_TITLE,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//window = glfwCreateWindow(width, height, WINDOW_TITLE, NULL, NULL);
	if(window==NULL)
		GameCrash("Initialized window failed.");
	LoggerInfo("Window initialized");
	glContext = SDL_GL_CreateContext(window);
	//glfwMakeContextCurrent(window);
	if(glContext==NULL)
	{
		GameCrash("Initialized opengl(2.1) failed.");
	}
	LoggerInfo("OpenGL(2.1) initialized");
	SDL_GL_SetSwapInterval(1);
	RE_Reshape(width,height);
	if(RE_InitQuicklyRender())
	{
		GameCrash("Initialized OpenGL quickly renderer failed.");
	}
	if(result=RE_InitFontRenderer(width,height))
	{
		LoggerFatal("Font renderer error:%d",result);
		GameCrash("Initialized Font renderer failed.");
	}
	LoggerInfo("Font renderer initialized");
	RE_InitShader();
	/*for (int w = 0; w < 256; w++)
	{
		RE_BindChar(w<<8);
	}*/

	return 0;
}

void RE_Reshape(int width,int height)
{
	windowWidth=width;
	windowHeight=height;
	glViewport(0,0,width,height);
	//aspect = (double)width/(double)height;
	aspect = (double)height/(double)width;
	//aspect=1;
}

void RE_DestroyWindow()
{
	if(glContext!=NULL)
		SDL_GL_DeleteContext(glContext);
	if(window!=NULL)
		SDL_DestroyWindow(window);
	if(quicklyRenderList[0]!=0)
	{
		RE_DestroyQuicklyRender();
		LoggerInfo("OpenGL quickly renderer destroyed");
	}
	/*if(library!=NULL)
	{
		RE_DestroyFontRenderer();
		LoggerInfo("Font renderer destroyed");
	}*/
}

int RE_BindTexture(Texture* texture)
{
	if(texture==NULL)
	{
		glBindTexture(GL_TEXTURE_2D,0);
		return 0;
	}	
	else
	{
		glBindTexture(GL_TEXTURE_2D,texture->id);
		return texture->id;
	}
}

void RE_SetMaterial(float* diffuse,float* ambient,float* specular,float* shininess)
{
	static float defaultDiffuse[] = {1,1,1,1};
	static float defaultAmbient[] = {0.5f,0.5f,0.5f};
	//static float defaultAmbient[] = {1,1,1};
	static float defaultSpecular[] = {0,0,0};
	static float defaultShininess = 0;

	if(diffuse!=NULL)
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	else
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultDiffuse);

	if(ambient!=NULL)
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	else
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, defaultAmbient);

	if(specular!=NULL)
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, specular);
	else
		glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, defaultSpecular);

	if(shininess!=NULL)
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, *shininess);
	else
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, defaultShininess);
}

int RE_Render()
{
	static float amLight[] = {1,1,1,1};
	static float light0Position[] = {0,0,1,1};
	static float light0Diffuse[] = {1,1,1,1};
	static Texture* texture = NULL;
	//------------------一些处理-------------------
	//RE_UpdateTextTextureCache();
	//-------------------绘制背景------------------
	glClearColor(RE_CLEAR_COLOR); //静怡的天蓝色 vec3(0.6,0.71,0.75)
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清理缓冲区
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800.0, -600.0, 0.0, 0.5, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);
	glEnable(GL_TEXTURE_2D);
	RE_RenderBackground(2.0f / 21.0f, 0.1f / 15.9f, 400.0f/WINDOW_WIDTH_FLOAT, 600.0f/WINDOW_HEIGHT_FLOAT);
	glDisable(GL_TEXTURE_2D);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	//-------------------绘制3D-------------------
	RE_CheckGLError(RE_STAGE_BEFORE_DRAW_3D);
	glMatrixMode(GL_PROJECTION); //重设定投影矩阵
	glLoadIdentity();
	glFrustum(-0.35,0.65,-aspect/2,aspect/2,1,1024);
	glMatrixMode( GL_MODELVIEW ); //设定模型视角矩阵
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST); //不开深度测试的话毁三观啊
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amLight);
	glLightfv(GL_LIGHT0,GL_POSITION,light0Position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0Diffuse);
	glTranslatef(0.0f, 0.0f, -42);
	if(theWorld!=NULL)
	{
		theWorld->Render();
	}
	RE_CheckGLError(RE_STAGE_AFTER_DRAW_3D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glFlush();
	RE_CheckGLError(RE_STAGE_FLUSH_3D);
	//-------------------绘制2D-------------------
	RE_CheckGLError(RE_STAGE_BEFORE_DRAW_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1.0, -1.0, 0.0, 0.5, 10.0); //将投影矩阵放置在第四象限
	//glOrtho(-1.0, 1.0, -1.0, 1.0, 0.5, 10.0);
	//glTranslatef(0.0f, 0.0f, 1.0f);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Gui_Render(theWorld);
	//glBindTexture(GL_TEXTURE_2D, charTextures[0]);
	//RE_DrawRectWithTexture(0, 0, 0.7, 0.7, 0, 0, 1, 1);
	//glBindTexture(GL_TEXTURE_2D, NULL);
	RE_CheckGLError(RE_STAGE_AFTER_DRAW_2D);
	glFlush();
	RE_CheckGLError(RE_STAGE_FLUSH_2D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	SDL_GL_SwapWindow(window);
	RE_CheckGLError(RE_STAGE_FINISH);
	return 0;
}

void RE_RenderCubeDoLeft(float lx,float ly,float lz,float rx,float ry,float rz)
{
	//绘制左面
	//glColor4f(0,1,0,1);
	glNormal3f(-1,0,0);
	glTexCoord2f(1,1);glVertex3f(lx,ly,rz);
	glTexCoord2f(1,0);glVertex3f(lx,ry,rz);
	glTexCoord2f(0,0);glVertex3f(lx,ry,lz);
	glTexCoord2f(0,1);glVertex3f(lx,ly,lz);
}

void RE_RenderCubeDoCentre(float lx,float ly,float lz,float rx,float ry,float rz)
{
	//绘制正面
	//glColor4f(1,0,0,1);
	glNormal3f(0,0,1);
	glTexCoord2f(1,1);glVertex3f(rx,ly,rz);
	glTexCoord2f(1,0);glVertex3f(rx,ry,rz);
	glTexCoord2f(0,0);glVertex3f(lx,ry,rz);
	glTexCoord2f(0,1);glVertex3f(lx,ly,rz);
	//绘制背面
	//glColor4f(1,0,0,1);
	glNormal3f(0,0,-1);
	glTexCoord2f(1,1);glVertex3f(lx,ly,lz);
	glTexCoord2f(1,0);glVertex3f(lx,ry,lz);
	glTexCoord2f(0,0);glVertex3f(rx,ry,lz);
	glTexCoord2f(0,1);glVertex3f(rx,ly,lz);
	//绘制顶面
	//glColor4f(0,0,1,1);
	glNormal3f(0,1,0);
	glTexCoord2f(1,1);glVertex3f(rx,ly,lz);
	glTexCoord2f(1,0);glVertex3f(rx,ly,rz);
	glTexCoord2f(0,0);glVertex3f(lx,ly,rz);
	glTexCoord2f(0,1);glVertex3f(lx,ly,lz);
	//绘制底面
	//glColor4f(0,0,1,1);
	glNormal3f(0,-1,0);
	glTexCoord2f(1,1);glVertex3f(lx,ry,lz);
	glTexCoord2f(1,0);glVertex3f(lx,ry,rz);
	glTexCoord2f(0,0);glVertex3f(rx,ry,rz);
	glTexCoord2f(0,1);glVertex3f(rx,ry,lz);
}

void RE_RenderCubeDoRight(float lx,float ly,float lz,float rx,float ry,float rz)
{
	//绘制右面
	//glColor4f(0,1,0,1);
	glNormal3f(1,0,0);
	glTexCoord2f(1,1);glVertex3f(rx,ly,lz);
	glTexCoord2f(1,0);glVertex3f(rx,ry,lz);
	glTexCoord2f(0,0);glVertex3f(rx,ry,rz);
	glTexCoord2f(0,1);glVertex3f(rx,ly,rz);
}

void RE_RenderCube( float lx,float ly,float lz,float rx,float ry,float rz )
{
	if(ly<ry)
	{
		float temp = ly;
		ly=ry;
		ry=temp;
	}
	glBegin(GL_QUADS);
		RE_RenderCubeDoLeft(lx,ly,lz,rx,ry,rz);
		RE_RenderCubeDoCentre(lx,ly,lz,rx,ry,rz);
		RE_RenderCubeDoRight(lx,ly,lz,rx,ry,rz);
	glEnd();
}

unsigned int RE_ProcessRawTexture( byte* rawData,int color,int format,unsigned long width,unsigned long height )
{
	GLuint texture;
	RE_CheckGLError(RE_STAGE_BEFORE_PROCESS_TEXTURE);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//GL_RGBA
	gluBuild2DMipmaps(GL_TEXTURE_2D,color,width,height,format,GL_UNSIGNED_BYTE, rawData);
	//glTexImage2D(GL_TEXTURE_2D, 0, color, width, height, 0, format, GL_UNSIGNED_BYTE, rawData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_BLEND); //诡异的反色!
	RE_CheckGLError(RE_STAGE_AFTER_PROCESS_TEXTURE);
	return texture;
}

unsigned int RE_ProcessRawTextureWithoutMipmap(byte* rawData,int color,int format,unsigned long width,unsigned long height)
{
	GLuint texture;
	RE_CheckGLError(RE_STAGE_BEFORE_PROCESS_TEXTURE);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, color, width, height, 0, format, GL_UNSIGNED_BYTE, rawData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	RE_CheckGLError(RE_STAGE_AFTER_PROCESS_TEXTURE);
	return texture;
}

void RE_UnloadTexture( unsigned int texture )
{
	glDeleteTextures(1,&texture);
}

int RE_CheckGLError(char* stage)
{
	GLenum error;
	/*#ifndef DEBUG
		if(stage!=RE_STAGE_FINISH)
		{
			return 0;
		}
	#endif*/
	error = glGetError();
	if(error!=GL_NO_ERROR)
	{
		LoggerWarn("An OpenGL error happened in [%s] : %s",stage,gluErrorString(error));
	}
	return error;
}

void RE_DrawRectWithTexture( float x,float y,float width,float height,float u,float v,float uw,float vh )
{
	//x=x*2.0f-1.0f;
	y=-y;
	v=1.0f-v;
	//width*=2.0f;
	//height*=2.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(u+uw,v-vh);glVertex3f(x+width,y-height,0);
		glTexCoord2f(u + uw, v); glVertex3f(x + width, y, 0);
		glTexCoord2f(u, v); glVertex3f(x, y, 0);
		glTexCoord2f(u, v - vh); glVertex3f(x, y - height, 0);
	glEnd();
}

int RE_InitQuicklyRender()
{
	int i,j;
	float x;
	quicklyRenderList[0] = glGenLists(20);
	if(quicklyRenderList[0]==0)
	{
		return 1;
	}
	for(i=1;i<20;i++)
	{
		quicklyRenderList[i]=quicklyRenderList[i-1]+1;
	}
	for(i=1;i<=20;i++)
	{
		glNewList(quicklyRenderList[i-1],GL_COMPILE);
		for(j=1;j<=i;j++)
		{
			if(j==1&&j==i)
			{
				RE_RenderCube(-0.5f,0.5f,-0.5f,0.5f,-0.5f,0.5f);
				break;
			}
			if(j==1)
			{
				x = - (float)(i-1)/2;
				//x = -0.5f - (float)(i-1)/2;
				glBegin(GL_QUADS);
				RE_RenderCubeDoLeft(-0.5f+x,0.5f,-0.5f,0.5f+x,-0.5f,0.5f);
				RE_RenderCubeDoCentre(-0.5f+x,0.5f,-0.5f,0.5f+x,-0.5f,0.5f);
				glEnd();
				x += 1.0f;
				continue;
			}
			if(j==i)
			{
				glBegin(GL_QUADS);
				RE_RenderCubeDoCentre(-0.5f+x,0.5f,-0.5f,0.5f+x,-0.5f,0.5f);
				RE_RenderCubeDoRight(-0.5f+x,0.5f,-0.5f,0.5f+x,-0.5f,0.5f);
				glEnd();
				x += 1.0f;
				continue;
			}
			glBegin(GL_QUADS);
			RE_RenderCubeDoCentre(-0.5f+x,0.5f,-0.5f,0.5f+x,-0.5f,0.5f);
			glEnd();
			x += 1.0f;
		}
		glEndList();
	}
	return 0;
}

void RE_DestroyQuicklyRender()
{
	glDeleteLists(quicklyRenderList[0],20);
}

void RE_RenderCubeQuick( int count )
{
	if(count<1||count>20)
	{
		LoggerWarn("Something is trying to quickly render %d cubes. Renderer ignored its request");
		return;
	}
	glCallList(quicklyRenderList[count-1]);
}

int RE_InitFontRenderer(int width,int height)
{
	char *font;
	size_t length;
	font = OS_GetFontPath(FONT_DEFAULT,FONT_BACKUP);
	if(font==NULL)
	{
		LoggerFatal("Can't find font %s and %s",FONT_DEFAULT,FONT_BACKUP);
		return -1;
	}

	fontData = (byte*)stb_file(font, &length);
	if (fontData == NULL)
	{
		LoggerFatal("Can't open font file %s", font);
		free_s(font);
		return -1;
	}
	if (!stbtt_InitFont(&fontInfo, fontData, 0))
	{
		LoggerFatal("Failed to init font %s", font);
		free_s(font);
		return -1;
	}
	free_s(font);
	int count = sizeof(wchar_t) == 2 ? 256 : 4351;
	memset(charTextures, 0xFFFFFFFF, count*sizeof(GLuint));
	return 0;
}

//旧的销毁缓存的函数,已经不用啦
/*int RE_DestroyTextTexture(void *texture)
{
	TextTexture *textTexture=(TextTexture*)texture;
	if(!textTexture->isStatic)
	{
		free_s(textTexture->text);
	}
	RE_UnloadTexture(textTexture->texture.id);
	free_s(textTexture);
	return 0;
}*/

void RE_DestroyFontRenderer()
{
	if(fontData!=NULL)
		free_s(fontData);
	/*if(face!=NULL)
		FT_Done_Face(face);
	FT_Done_FreeType(library);
	if(textTextureCache!=NULL)
	{
		LinkedListDestory(textTextureCache,RE_DestroyTextTexture);
	}*/
}

void stbtt_GetBakedQuad_Custom(stbtt_bakedchar *chardata, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, float scale)
{
	const float ipw = 1.0f / 512.0f, iph = 1.0f / 512.0f;
	stbtt_bakedchar *b = chardata + char_index;
	int round_x = (int)floor((*xpos + b->xoff) + 0.5);
	int round_y = (int)floor((*ypos + b->yoff) + 0.5) - b->yoff - 20.0f;
	float x = b->x1 - b->x0;
	float y = b->y1 - b->y0;
	q->x0 = round_x;
	q->y0 = round_y;

	q->x1 = q->x0 + x * scale;
	q->y1 = q->y0 + y * scale;

	q->s0 = b->x0 * ipw;
	q->t0 = b->y0 * iph;
	q->s1 = b->x1 * ipw;
	q->t1 = b->y1 * iph;

	*xpos += b->xadvance;
}

int stbtt_BakeFontBitmap_Custom(stbtt_fontinfo f, int offset,
	float pixel_height,
	unsigned char *pixels, int pw, int ph,
	int first_char, int num_chars,
	stbtt_bakedchar *chardata)
{
	float scale;
	int x, y, bottom_y, i;
	memset(pixels, 0, pw*ph);
	x = y = 1;
	bottom_y = 1;

	scale = stbtt_ScaleForPixelHeight(&f, pixel_height);

	for (i = 0; i < num_chars; ++i) {
		int advance, lsb, x0, y0, x1, y1, gw, gh;
		int g = stbtt_FindGlyphIndex(&f, first_char + i);
		stbtt_GetGlyphHMetrics(&f, g, &advance, &lsb);
		stbtt_GetGlyphBitmapBox(&f, g, scale, scale, &x0, &y0, &x1, &y1);
		gw = x1 - x0;
		gh = y1 - y0;
		if (x + gw + 1 >= pw)
			y = bottom_y, x = 1; // advance to next row
		if (y + gh + 1 >= ph) // check if it fits vertically AFTER potentially moving to next row
			return -i;
		//STBTT_assert(x + gw < pw);
		//STBTT_assert(y + gh < ph);
		stbtt_MakeGlyphBitmap(&f, pixels + x + y*pw, gw, gh, pw, scale, scale, g);
		chardata[i].x0 = (short)x;
		chardata[i].y0 = (short)y;
		chardata[i].x1 = (short)(x + gw);
		chardata[i].y1 = (short)(y + gh);
		chardata[i].xadvance = scale * advance;
		chardata[i].xoff = (float)x0;
		chardata[i].yoff = (float)y0;
		x = x + gw + 2;
		if (y + gh + 2 > bottom_y)
			bottom_y = y + gh + 2;
	}
	return bottom_y;
}

void RE_DrawText(wchar_t *text, float x, float y, float maxWidth)
{
	//return;
	wchar_t c;
	y =-y;
	x *= 800.0f;
	y *= 600.0f;
	maxWidth *= 800.0f;
	float startX = x;
	float height = 30.0f;
	float baseX = startX;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 800.0, -600.0, 0.0, 0.5, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glBegin(GL_QUADS);
	fontRendering = TRUE;
	for (; *text != NULL; text++)
	{
		c = *text;
		if (c == '\n' || (baseX - startX) > maxWidth)
		{
			x = startX;
			baseX = 0;
			y -= height;
			if(c == '\n')
				continue;
		}
		RE_BindChar(c);
		/*float u = (c & 15) * (1.0f / 16.0f); //与B1111(O15)做按位与,获得列数.
		float v = ((c >> 4) & 15) * (1.0f / 16.0f); //先右移4位去掉列数,然后与B1111(O15)做按位与,获得行数.
		const float uw = 1.0f / 16.0f;
		const float vh = 1.0f / 16.0f;
		v = 1.0f - v;*/
		/*
		glTexCoord2f(u + uw, v - vh); glVertex3f(x + width, y - height, 0);
		glTexCoord2f(u + uw, v); glVertex3f(x + width, y, 0);
		glTexCoord2f(u, v); glVertex3f(x, y, 0);
		glTexCoord2f(u, v - vh); glVertex3f(x, y - height, 0);
		*/
		stbtt_bakedchar *cInfo = charInfo[c >> 8];
		stbtt_aligned_quad q;
		int index = c & 255;
		stbtt_GetBakedQuad_Custom(cInfo, index, &x, &y, &q, 0.8f);
		//q.y1 = q.y0 - (q.y1 - q.y0);
		float xOffest = 5;
		float yOffset = (cInfo + index)->yoff + 20.0f;
		glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0 - xOffest, q.y0);
		glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1 - xOffest, q.y0);
		glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1 - xOffest, q.y1);
		glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0 - xOffest, q.y1);
		x -= 5.0f;
		baseX = x;
	}
	fontRendering = FALSE;
	glEnd();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void RE_BindChar(wchar_t c)
{
	//TODO:释放文字纹理...
	//static stbtt_bakedchar unusedData[256]; //被浪费了啊,可惜
	static GLuint currentTexture = 0xFFFFFFFF;
	GLuint texture = charTextures[c >> 8];
	if (texture == 0xFFFFFFFF)
	{
		if (fontRendering == TRUE)
			glEnd();
		long long time = OS_GetMsTime();
		static byte bitmap[512*512];
		charInfo[c >> 8] = (stbtt_bakedchar*)malloc_s(256 * sizeof(stbtt_bakedchar));
		//char *fileName = "";
		//FILE *cache = fopen();
		stbtt_BakeFontBitmap_Custom(fontInfo, 0, 30.0f,
			bitmap,512,512,
			(c >> 8) << 8, 256, charInfo[c >> 8]);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		/*static byte tempBytes[256];
		for (int i = 0; i < 128; i++)
		{
			memcpy(tempBytes, bitmap+i*256, 256 * sizeof(byte));
			memcpy(bitmap + i * 256, bitmap + (255-i) * 256, 256 * sizeof(byte));
			memcpy(bitmap + (255 - i) * 256, tempBytes, 256 * sizeof(byte));
		}*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		//free_s(bitmap);
		charTextures[c >> 8] = texture;
		LoggerDebug("A character bitmap:%d has been generated, took time: %ldms", c >> 8, OS_GetMsTime()-time);
		if (fontRendering == TRUE)
			glBegin(GL_QUADS);
	}
	else if (currentTexture == texture)
		return;
	if (fontRendering == TRUE)
		glEnd();
	glBindTexture(GL_TEXTURE_2D, texture);
	if (fontRendering == TRUE)
		glBegin(GL_QUADS);
	currentTexture = texture;
}

GLuint loadShader(char* file, GLenum shaderType)
{
	FILE *shaderFile = fopen(file, "r");
	if (shaderFile == NULL)
		return 0;
	int length;
	char** content = stb_stringfile(file, &length);
	fclose(shaderFile);
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
		return 0;
	glShaderSource(shader, length, (const GLchar**)content, NULL);
	glCompileShader(shader);
	if (RE_CheckGLError(RE_STAGE_CREATING_SHADER) != GL_NO_ERROR)
		return 0;
	return shader;
}

int RE_InitShader()
{
	//Get function pointers.
	glCreateShader = (PFNGLCREATESHADEROBJECTARBPROC)SDL_GL_GetProcAddress("glCreateShader");
	glCompileShader = (PFNGLCOMPILESHADERARBPROC)SDL_GL_GetProcAddress("glCompileShader");
	glShaderSource = (PFNGLSHADERSOURCEARBPROC)SDL_GL_GetProcAddress("glShaderSource");
	glCreateProgram = (PFNGLCREATEPROGRAMOBJECTARBPROC)SDL_GL_GetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
	glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
	glUniform3f = (PFNGLUNIFORM3FPROC)SDL_GL_GetProcAddress("glUniform3f");
	glUniform4f = (PFNGLUNIFORM4FPROC)SDL_GL_GetProcAddress("glUniform4f");
	glUniform1f = (PFNGLUNIFORM1FPROC)SDL_GL_GetProcAddress("glUniform1f");
	glUniform1i = (PFNGLUNIFORM1IPROC)SDL_GL_GetProcAddress("glUniform1i");

	programBackground = glCreateProgram();
	GLuint shaderBackgroundVert = loadShader("shader/cloud.vert", GL_VERTEX_SHADER);
	GLuint shaderBackgroundFrag = loadShader("shader/cloud.frag", GL_FRAGMENT_SHADER);
	if (shaderBackgroundVert != 0 && shaderBackgroundFrag != 0)
	{
		glAttachShader(programBackground, shaderBackgroundVert);
		glAttachShader(programBackground, shaderBackgroundFrag);
		glLinkProgram(programBackground);
		programBackgroundResolution = glGetUniformLocation(programBackground, "iResolution");
		programBackgroundGlobalTime = glGetUniformLocation(programBackground, "iGlobalTime");
		programBackgroundChannel0 = glGetUniformLocation(programBackground, "iChannel0");
		programBackgroundSkyColor = glGetUniformLocation(programBackground, "skyColor");
		
		if (RE_CheckGLError(RE_STAGE_CREATING_PROGRAM) == GL_NO_ERROR)
		{
			LoggerInfo("Background shader loaded.");
			return 0;
		}
	}
	LoggerError("Failed to load background shader.");
	return -1;
}

void RE_RenderBackground(float x, float y, float width, float height)
{
	static Texture* noissyTex = NULL;
	if (programBackground == 0)
		return;
	if (noissyTex == NULL)
		noissyTex = RM_GetTexture("image/cloudshader.png");
	RE_BindTexture(noissyTex);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glUseProgram(programBackground);
	glUniform3f(programBackgroundResolution, (width)*windowWidth, (height)*windowHeight, 1.0f);
	if(theWorld==NULL)
		glUniform1f(programBackgroundGlobalTime, 0);
	else
		glUniform1f(programBackgroundGlobalTime, theWorld->tick*WINDOW_FRAME/1000.0f);
	glUniform1i(programBackgroundChannel0, 0);
	glUniform4f(programBackgroundSkyColor, RE_CLEAR_COLOR);
	//RE_DrawRectWithTexture(x, y, width, height, 0, 0, 1, 1);
	//width*=2.0f;
	//height*=2.0f;
	y = -y;
	glBegin(GL_QUADS);
	glTexCoord2f(1,0);glVertex3f((x+width)*WINDOW_WIDTH_FLOAT,(y-height)*WINDOW_HEIGHT_FLOAT,0);
	glTexCoord2f(1, 1); glVertex3f((x + width)*WINDOW_WIDTH_FLOAT, y*WINDOW_HEIGHT_FLOAT, 0);
	glTexCoord2f(0,1); glVertex3f(x*WINDOW_WIDTH_FLOAT, y*WINDOW_HEIGHT_FLOAT, 0);
	glTexCoord2f(0, 0); glVertex3f(x*WINDOW_WIDTH_FLOAT, (y - height)*WINDOW_HEIGHT_FLOAT, 0);
	glEnd();
	glUseProgram(0);
}

//旧的字体渲染函数
/*void RE_DrawTextStatic( char* text,float x,float y,float width )
{
	LinkedListIterator *iterator;
	TextTexture *texture = NULL;
	for(iterator=LinkedListGetIterator(textTextureCache);LinkedListIteratorHasNext(iterator);)
	{
		TextTexture *temp = (TextTexture*)LinkedListIteratorGetNext(iterator);
		if(temp->text==text && MathFloatEqual(temp->width,width))
		{
			LinkedListIteratorPullUpCurrent(iterator);
			temp->life=100;
			texture=temp;
			break;
		}
	}
	if(texture==NULL)
	{
		texture = RE_ProcessTextTexture(text,width);
		texture->width = width;
		texture->text=text;
		texture->hash=0;
		texture->isStatic=TRUE;
		texture->life=100;
		LinkedListOffer(textTextureCache,texture);
	}
	RE_BindTexture(&(texture->texture));
	RE_DrawRectWithTexture(x,y,texture->texture.width/(float)windowWidth,texture->texture.height/(float)windowHeight,0,0,1,1);
}

void RE_DrawTextVolatile( char* text,float x,float y,float width )
{
	LinkedListIterator *iterator;
	TextTexture *texture = NULL;
	Hash hash = HashCode(text);
	for(iterator=LinkedListGetIterator(textTextureCache);LinkedListIteratorHasNext(iterator);)
	{
		TextTexture *temp = (TextTexture*)LinkedListIteratorGetNext(iterator);
		if(hash==temp->hash && MathFloatEqual(temp->width,width) && strcmp(text,temp->text)==0)
		{
			LinkedListIteratorPullUpCurrent(iterator);
			temp->life=100;
			texture=temp;
			break;
		}
	}
	if(texture==NULL)
	{
		int length = strlen(text)+1;
		texture = RE_ProcessTextTexture(text,width);
		texture->width = width;
		texture->text=(char*)malloc_s(length*sizeof(char));
		texture->hash=hash;
		memcpy(texture->text,text,length);
		texture->isStatic=FALSE;
		texture->life=100;
		LinkedListOffer(textTextureCache,texture);
	}
	RE_BindTexture(&(texture->texture));
	RE_DrawRectWithTexture(x,y,texture->texture.width/(float)windowWidth,texture->texture.height/(float)windowHeight,0,0,1,1);
}*/

/*

TextTexture* RE_ProcessTextTexture( char* utf8Text,float maxWidth )
{
	GLubyte *bytes,*revBytes;
	FT_ULong *unicodeText = NULL;
	GLuint textureID;
	FT_GlyphSlot slot = face->glyph;
	FT_UInt glyph_index; 
	int i,j,w,h,k; //5个路人甲
	int headX=0,headY=0;
	TextTexture* texture;
	int count=0;
	int lineHeight;
	int maxX,maxY,usedLine=1;
	unicodeText = UTF8ToUTF32(utf8Text);
	lineHeight = (face->size->metrics.y_ppem>>6)+(face->size->metrics.descender>>6)+(face->size->metrics.ascender>>6)+4;
	//lineHeight /= 2;
	maxX = MathNextMultiple8((unsigned int)(windowWidth*maxWidth));
	//maxX = MathNextPower2((unsigned int)(windowWidth*maxWidth));
	maxY = MathNextPower2(lineHeight*10);
	//maxX = MathNextPower2(100);
	//maxY = MathNextPower2(lineHeight);
	bytes = (GLubyte *)malloc_s(maxX*maxY*sizeof(GLubyte));
	memset(bytes,0,maxX*lineHeight*sizeof(GLubyte));
	while(*(unicodeText+count)!=0)
	{
		glyph_index = FT_Get_Char_Index( face, *(unicodeText+count) ); 
		FT_Load_Glyph(face,glyph_index, FT_LOAD_NO_BITMAP  );
		FT_Render_Glyph( slot,FT_RENDER_MODE_NORMAL);
		w = slot->bitmap.width;
		h = slot->bitmap.rows;
		if(*(unicodeText+count)=='\n'||headX+w>=maxX)
		{
			headX=0;
			headY+=lineHeight;
			memset(bytes+lineHeight*usedLine*maxX,0,maxX*lineHeight*sizeof(GLubyte));
			usedLine++;
		}
		k=0;
		headX+= slot->bitmap_left>0?slot->bitmap_left:0;
		headY+=lineHeight - slot->bitmap_top-3;
		for(j=0; j <h;j++) {
			for(i=0; i < w; i++){
				bytes[headX+i+(headY+j)*maxX]= i>=slot->bitmap.width||j>=slot->bitmap.rows?0:slot->bitmap.buffer[k++];
			}
		}
		headX-= slot->bitmap_left>0?slot->bitmap_left:0;
		headY-=lineHeight-slot->bitmap_top-3;
		headX+=slot->bitmap.width+2;
		count++;
		//break;
	}
	maxY=lineHeight*usedLine;
	if(usedLine==1)
	{
		int temp = MathNextMultiple8(headX);
		temp=temp>maxX?maxX:temp;
		revBytes = (GLubyte *)malloc_s(temp*maxY*sizeof(GLubyte));
		for(j=0;j<maxY;j++)
		{
			memcpy(revBytes+((maxY-j-1)*temp),bytes+(j*maxX),temp*sizeof(GLubyte));
		}
		maxX = temp;
	}
	else
	{
		revBytes = (GLubyte *)malloc_s(maxX*maxY*sizeof(GLubyte));
		for(j=0;j<maxY;j++)
		{
			memcpy(revBytes+((maxY-j-1)*maxX),bytes+(j*maxX),maxX*sizeof(GLubyte));
		}
	}
	
	free_s(bytes);
	texture = (TextTexture*)malloc_s(sizeof(TextTexture));
	//生成文字纹理
	RE_CheckGLError(RE_STAGE_BEFORE_PROCESS_TEXTURE);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA8,maxX,maxY,0,GL_ALPHA,GL_UNSIGNED_BYTE, revBytes);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	RE_CheckGLError(RE_STAGE_AFTER_PROCESS_TEXTURE);
	texture->texture.id = textureID;
	texture->texture.width=maxX;
	texture->texture.height=maxY;
	free_s(revBytes);
	free_s(unicodeText);
	return texture;
}

void RE_UpdateTextTextureCache()
{
	LinkedListIterator *iterator;
	TextTexture *texture = NULL;
	for(iterator=LinkedListGetIterator(textTextureCache);LinkedListIteratorHasNext(iterator);)
	{
		texture = (TextTexture*)LinkedListIteratorGetNext(iterator);
		texture->life--;
		if(texture->life==0)
		{
			RE_DestroyTextTexture(texture);
			LinkedListIteratorDeleteCurrent(iterator);
		}
	}
}
*/