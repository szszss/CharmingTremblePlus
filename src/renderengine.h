//#define RE_INIT_SUCCESS_BUT_LOW_VERSION 1
//#define RE_INIT_GL_NO_SUPPORT 10
//#define RE_INIT_JUST_ERROR 255
#pragma once
#include "game.h"
#include <wchar.h>

#define RE_STAGE_BEFORE_DRAW_3D "Before drawing 3D"
#define RE_STAGE_AFTER_DRAW_3D "After drawn 3D"
#define RE_STAGE_FLUSH_3D "Flushing 3D"
#define RE_STAGE_BEFORE_DRAW_2D "Before drawing 2D"
#define RE_STAGE_AFTER_DRAW_2D "After drawn 2D"
#define RE_STAGE_FLUSH_2D "Flushing 2D"
#define RE_STAGE_FINISH "Rendering"

#define RE_STAGE_BEFORE_PROCESS_TEXTURE "Before processing texture"
#define RE_STAGE_AFTER_PROCESS_TEXTURE "Processing texture"

#define RE_CLEAR_COLOR 0.345, 0.675, 1.0, 1.0

struct implTexture{
	unsigned long width;
	unsigned long height;
	unsigned int id;
};

struct implTextTexture{
	Texture texture;
	Hash hash;
	char *text;
	float width;
	BOOL isStatic;
	int life;
};

/*初始化窗口,OPENGL和图形资源.*/
int RE_InitWindow(int width,int height);
/*销毁窗口和OpenGL,并释放图形资源*/
void RE_DestroyWindow();
/*调整窗口*/
void RE_Reshape(int width,int height);
/*渲~染~!*/
int RE_Render();
/*快速渲染一组砖块.*/
void RE_RenderCubeQuick(int count);
/*渲染一个lx,ly,lz为左上顶点,rx,ry,rz为右下顶点的砖块.*/
void RE_RenderCube(float lx,float ly,float lz,float rx,float ry,float rz);
/*绘制一个带有纹理的矩形.
x,y为矩形左上角坐标
width,height为矩形宽高
u,v为纹理的uv
uw,vh为纹理中截取的面积*/
void RE_DrawRectWithTexture(float x,float y,float width,float height,float u,float v,float uw,float vh);
/*绑定一个纹理,若为NULL,则为取消绑定.返回值为被绑定的纹理的纹理对象ID (参数为NULL则为0)*/
int RE_BindTexture(Texture* texture);
/*设置材质,NULL的项会采用默认值*/
void RE_SetMaterial(float* diffuse,float* ambient,float* specular,float* shininess);
/*使用默认材质*/
#define RE_ClearMaterial() RE_SetMaterial(0,0,0,0);
/*处理一个正在载入的纹理*/
unsigned int RE_ProcessRawTexture(byte* rawData,int color,int format,unsigned long width,unsigned long height);
/*卸载一个纹理对象,注意它只负责卸载OpenGL内部的纹理对象,纹理(Texture)的释放仍需要手动完成*/
void RE_UnloadTexture(unsigned int texture);
/*检查OpenGL内部错误,注意只有在Debug模式下才会检查每一个错误,Release模式下只有Rendering阶段才会进行检查.这是因为glGetError的开销很大.*/
int RE_CheckGLError(char* stage);
void RE_DrawTextStatic(char* text,float x,float y,float width);
void RE_DrawTextVolatile(char* text,float x,float y,float width);