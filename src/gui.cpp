#include "gui.h"
#include "memory.h"
#include "renderengine.h"
#include "resourcemanager.h"
#include "collection.h"
#include "input.h"
#include "SDL_opengl.h"
#include <stdlib.h> //可去除
#include "pmd.h"
#include <time.h>
#include <wchar.h>

static GuiScreen *currentScreen = NULL;
extern World* theWorld;
extern wchar_t* defPlayerName;

void Gui_Update( World* world )
{
	if(currentScreen==NULL)
		return;
	currentScreen->Update(world);
}

void Gui_Render( World* world )
{
	if(currentScreen==NULL)
		return;
	currentScreen->Render(world);
}

void Gui_Destroy()
{
	while(Gui_Close()){};
}

void Gui_Open(GuiScreen* newScreen)
{
	GuiScreen *screen = newScreen;
	screen->parent=currentScreen;
	currentScreen=screen;
}

BOOL Gui_Close()
{
	GuiScreen *screen;
	if(currentScreen==NULL)
		return FALSE;
	screen = currentScreen->parent;
	currentScreen->Close();
	delete(currentScreen);
	currentScreen=screen;
	return TRUE;
}

void Gui_MouseDown(long x,long y)
{
	LinkedListIterator *iterator;
	if(currentScreen==NULL)
		return;
	for(iterator = LinkedListGetIterator(currentScreen->controlList);LinkedListIteratorHasNext(iterator);)
	{
		GuiControl *button = (GuiControl*)LinkedListIteratorGetNext(iterator);
		if(x>=button->posX&&x<=(button->posX+button->width)&&y>=button->posY&&y<=(button->posY+button->height)&&button->flags!=2)
		{
			button->flags=1;
		}
	}
}

/*GuiButton* Gui_InitButton(int x, int y,int w,int h,wchar_t *text,int flags,int (*press)(GuiButton*,GuiScreen*,World*),void (*update)(GuiButton*,GuiScreen*,World*))
{
	GuiButton* button = (GuiButton*)malloc_s(sizeof(GuiButton));
	button->posX=x;
	button->posY=y;
	button->width=w;
	button->height=h;
	button->text=text;
	button->press=press;
	button->update=update;
	button->flags=flags;
	return button;
}*/

/*void Gui_AddButton(GuiScreen* screen,GuiButton* button)
{
	LinkedListAdd(screen->controlList,button);
}*/

void GuiScreen::AddControl(GuiControl* control)
{
	LinkedListAdd(controlList, control);
}

void GuiButton::Render(World* world)
{
	//float x = button->flags==1?button->posX+0.01f:button->posX;
	//float y = button->flags==1?button->posY+0.01f:button->posY;
	RE_BindTexture(NULL);
	glColor3f(0.2,0.2,0.2);
	RE_DrawRectWithTexture(posX/WINDOW_WIDTH_FLOAT,posY/WINDOW_HEIGHT_FLOAT,
		width/WINDOW_WIDTH_FLOAT,height/WINDOW_HEIGHT_FLOAT,0,0,1,1);
	if(flags==2)
		glColor3f(0.3,0.3,0.3);
	else
		glColor3f(1,1,1);
	RE_DrawRectWithTexture(posX/WINDOW_WIDTH_FLOAT+0.003f,posY/WINDOW_HEIGHT_FLOAT+0.003f,
		width/WINDOW_WIDTH_FLOAT-0.006f,height/WINDOW_HEIGHT_FLOAT-0.006f,0,0,1,1);
	glColor3f(0,0,0);
	RE_DrawText(text, posX/WINDOW_WIDTH_FLOAT+0.02f, posY/WINDOW_HEIGHT_FLOAT+0.02f, width/WINDOW_WIDTH_FLOAT);
	glColor3f(1,1,1);
}

GuiScreenGame::GuiScreenGame()
{
	GuiButton *btnStart = new GuiButtonStart(500,350,70,40);
	GuiButton *btnStop = new GuiButtonStop(600,350,70,40);
	GuiButton *btnRestart = new GuiButtonRestart(600,400,70,40);
	GuiButton *btnPause = new GuiButtonPause(500,400,70,40);
	GuiButton *btnName = new GuiButtonName(500,450,85,40);
	GuiButton *btnModel = new GuiButtonModel(600,450,70,40);
	controlList=LinkedListCreate();
	AddControl(btnStart);
	AddControl(btnStop);
	AddControl(btnRestart);
	AddControl(btnPause);
	AddControl(btnName);
	AddControl(btnModel);
}

void GuiScreenGame::Update(World* world)
{
	LinkedListIterator *iterator;
	for(iterator = LinkedListGetIterator(controlList);LinkedListIteratorHasNext(iterator);)
	{
		GuiControl *button = (GuiControl*)LinkedListIteratorGetNext(iterator);
		if(button->flags==1)
		{
			button->flags=0;
			button->Press(world);
		}
		button->Update(world);
	}
}

void GuiScreenGame::Render(World* world)
{
	static wchar_t life[8];
	static wchar_t score[24];
	static wchar_t maxs[24];
	static wchar_t level[24];
	long long maxScore = GameGetMaxScore();
	LinkedListIterator *iterator;
	static Texture* texture = NULL;
	if(texture==NULL)
		texture=RM_GetTexture("image/bgGame.png");
	glColor3f(1,1,1);
	RE_BindTexture(texture);
	//RE_BindTexture(NULL);
	RE_DrawRectWithTexture(0,0,1,1,0,0,800.0/1024.0,600.0/1024.0);
	RE_BindTexture(NULL);
	if(world!=NULL)
	{
		wsprintf(life,L"%d",world->players[0]->life);
		wsprintf(score,L"%d",world->players[0]->score);
		wsprintf(maxs,L"%d",maxScore);
		wsprintf(level,L"%d",world->players[0]->maxDepthLevel);
		glColor3f(1,0,0);
		RE_DrawText(L"Name:", 0.65f, 0.02f, 1.0f);
		RE_DrawText(defPlayerName, 0.65f, 0.06f, 1.0f);
		RE_DrawText(L"Life:", 0.65f, 0.1f, 1.0f);
		RE_DrawText(life, 0.65f, 0.14f, 1.0f);
		RE_DrawText(L"Score:", 0.65f, 0.18f, 1.0f);
		RE_DrawText(score, 0.65f, 0.22f, 1.0f);
		RE_DrawText(L"MaxScore:", 0.85f, 0.18f, 1.0f);
		RE_DrawText(maxs, 0.85f, 0.22f, 1.0f);
		RE_DrawText(L"Level:", 0.65f, 0.26f, 1.0f);
		RE_DrawText(level, 0.65f, 0.3f, 1.0f);
		if(world->state==WSTATE_GAMEOVERED)
		{
			RE_DrawText(L"很遗憾,你♂死♂了",0.2,0.5,1);
		}
	}
	
	glColor3f(1,1,1);
	for(iterator = LinkedListGetIterator(controlList);LinkedListIteratorHasNext(iterator);)
	{
		GuiControl *button = (GuiControl*)LinkedListIteratorGetNext(iterator);
		button->Render(world);
	}
}

int CallbackGuiButtonDestroy(void *button)
{
	delete(button);
	return 0;
}

void GuiScreenGame::Close()
{
	LinkedListDestory(controlList,CallbackGuiButtonDestroy);
}

GuiScreenName::GuiScreenName()
{
	GuiButton *btnEnter = new GuiButtonEnter(320,340,70,40);
	GuiButton *btnCancel = new GuiButtonCancel(420,340,70,40);
	controlList=LinkedListCreate();
	AddControl(btnEnter);
	AddControl(btnCancel);
	IN_TextInputEnable(300,200,200,200);
}

void GuiScreenName::Update(World* world)
{
	LinkedListIterator *iterator;
	for(iterator = LinkedListGetIterator(controlList);LinkedListIteratorHasNext(iterator);)
	{
		int flag = 0;
		GuiControl *button = (GuiControl*)LinkedListIteratorGetNext(iterator);
		if(button->flags==1)
		{
			button->flags=0;
			flag = button->Press(world);
		}
		button->Update(world);
		if(flag==1)
		{
			Gui_Close();
			break;
		}
	}
}

void GuiScreenName::Render(World* world)
{
	LinkedListIterator *iterator;
	static wchar_t str[256];
	parent->Render(world);
	RE_BindTexture(NULL);
	glColor3f(0.9,0.9,0.9);
	RE_DrawRectWithTexture(300/WINDOW_WIDTH_FLOAT,200/WINDOW_HEIGHT_FLOAT,
		200/WINDOW_WIDTH_FLOAT,200/WINDOW_HEIGHT_FLOAT,0,0,1,1);
	glColor3f(1,1,1);
	RE_DrawRectWithTexture(305/WINDOW_WIDTH_FLOAT,205/WINDOW_HEIGHT_FLOAT,
		190/WINDOW_WIDTH_FLOAT,190/WINDOW_HEIGHT_FLOAT,0,0,1,1);
	IN_TextInputGet(str);
	glColor3f(0,0,0);
	RE_DrawText(str,320/WINDOW_WIDTH_FLOAT,220/WINDOW_HEIGHT_FLOAT,160/WINDOW_WIDTH_FLOAT);
	for(iterator = LinkedListGetIterator(controlList);LinkedListIteratorHasNext(iterator);)
	{
		GuiControl *button = (GuiControl*)LinkedListIteratorGetNext(iterator);
		button->Render(world);
	}
}

void GuiScreenName::Close()
{
	LinkedListDestory(controlList,CallbackGuiButtonDestroy);
}

int GuiButtonStart::Press(World* world)
{
	#ifdef DEBUG
	theWorld = new World(defPlayerName, 9369319, TYPE_NORMAL, DIFF_NORMAL);
	#else
	theWorld = new World(defPlayerName,time(0),TYPE_NORMAL,DIFF_NORMAL);
	#endif
	theWorld->Start();
	return 0;
}

int GuiButtonPause::Press(World* world)
{
	GameSetPause(!GameGetPause());
	return 0;
}

int GuiButtonStop::Press(World* world)
{
	//GameSafelyKillWorld();
	theWorld->End();
	theWorld->Destory();
	delete(theWorld);
	theWorld=NULL;
	return 0;
}

int GuiButtonRestart::Press(World* world)
{
	theWorld->End();
	theWorld->Start();
	GameSetPause(FALSE);
	return 0;
}

int GuiButtonName::Press(World* world)
{
	Gui_Open(new GuiScreenName());
	return 0;
}

int GuiButtonModel::Press(World* world)
{
	if(world->players[0]->modelInstance == NULL)
	{
		world->players[0]->modelInstance = PMD_ModelInstanceCreate(PMD_LoadModel("model/koishi","koishi.pmd"));
	}
	else
	{
		world->players[0]->modelInstance = NULL;
	}
	return 0;
}

void GuiButtonStart::Update(World* world)
{
	if(theWorld!=NULL)
		flags=2;
	else
		flags=0;
}

void GuiButtonPause::Update(World* world)
{
	if(theWorld==NULL)
		flags=2;
	else
		flags=0;
}

void GuiButtonStop::Update(World* world)
{
	if(theWorld==NULL)
		flags=2;
	else
		flags=0;
}

void GuiButtonRestart::Update(World* world)
{
	if(theWorld==NULL)
		flags=2;
	else
		flags=0;
}

void GuiButtonName::Update(World* world)
{
	if(theWorld!=NULL)
		flags=2;
	else
		flags=0;
}

void GuiButtonModel::Update(World* world)
{
	if(theWorld==NULL)
		flags=2;
	else
		flags=0;
}

int GuiButtonEnter::Press(World* world)
{
	wchar_t temp[256];
	IN_TextInputGet(temp);
	defPlayerName = (wchar_t*)malloc_s((256)*sizeof(wchar_t));
	memcpy(defPlayerName, temp, 256 * sizeof(wchar_t));
	IN_TextInputDisable();
	return 1;
}

int GuiButtonCancel::Press(World* world)
{
	IN_TextInputDisable();
	return 1;
}






