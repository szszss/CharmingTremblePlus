#pragma once
#include "game.h"
#include "world.h"
#include "collection.h"
#include <wchar.h>

class GuiScreen
{
public:
	LinkedList *controlList;
	GuiScreen *parent;
	virtual void Update(World* world) {};
	virtual void Render(World* world) {};
	virtual void Close() {};
	void AddControl(GuiControl* control);
protected:
	GuiScreen() {}
};

class GuiControl
{
public:
	int posX;
	int posY;
	int width;
	int height;
	int flags;
	virtual void Update(World* world) {};
	virtual void Render(World* world) {};
	virtual int Press(World* world) { return 0; };
protected:
	GuiControl(int x, int y) { posX = x; posY = y; width = 0; height = 0; flags = 0; }
};

class GuiButton : public GuiControl
{
public:
	wchar_t *text;
	void Render(World* world);
protected:
	GuiButton(int x, int y, int w, int h) : GuiControl(x, y)
	{
		width = w; height = h; this->text = L"";
	};
};

class GuiScreenGame : public GuiScreen
{
public:
	GuiScreenGame();
	void Update(World* world);
	void Render(World* world);
	void Close();
};

class GuiScreenName : public GuiScreen
{
public:
	GuiScreenName();
	void Update(World* world);
	void Render(World* world);
	void Close();
};

class GuiButtonStart : public GuiButton
{
public:
	GuiButtonStart(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"开始"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonPause : public GuiButton
{
public:
	GuiButtonPause(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"暂停"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonStop : public GuiButton
{
public:
	GuiButtonStop(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"结束"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonRestart : public GuiButton
{
public:
	GuiButtonRestart(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"重开"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonName : public GuiButton
{
public:
	GuiButtonName(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"玩家名"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonModel : public GuiButton
{
public:
	GuiButtonModel(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"模型"; }
	void Update(World* world);
	int Press(World* world);
};

class GuiButtonEnter : public GuiButton
{
public:
	GuiButtonEnter(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"确定"; }
	int Press(World* world);
};

class GuiButtonCancel : public GuiButton
{
public:
	GuiButtonCancel(int x, int y, int w, int h) : GuiButton(x, y, w, h) { text = L"取消"; }
	int Press(World* world);
};


void Gui_Update(World* world);
void Gui_Render(World* world);
void Gui_Destroy();
void Gui_Open(GuiScreen* newScreen);
void Gui_MouseDown(long x,long y);
//GuiButton* Gui_InitButton(int x, int y, int w, int h, wchar_t* text, int flags, int(*press)(GuiButton*, GuiScreen*, World*), void(*update)(GuiButton*, GuiScreen*, World*));
BOOL Gui_Close();

/*GuiScreen* GuiScreenGame();
void GuiScreenGameUpdate(GuiScreen* screen,World* world);
void GuiScreenGameRender(GuiScreen* screen,World* world);
void GuiScreenGameClose(GuiScreen* screen);
GuiScreen* GuiScreenName();
void GuiScreenNameUpdate(GuiScreen* screen,World* world);
void GuiScreenNameRender(GuiScreen* screen,World* world);
void GuiScreenNameClose(GuiScreen* screen);

int GuiButtonStartPress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonPausePress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonStopPress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonRestartPress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonNamePress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonModelPress(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonStartUpdate(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonPauseUpdate(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonStopUpdate(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonRestartUpdate(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonNameUpdate(GuiButton* button,GuiScreen* screen,World* world);
void GuiButtonModelUpdate(GuiButton* button,GuiScreen* screen,World* world);

int GuiButtonEnterPress(GuiButton* button,GuiScreen* screen,World* world);
int GuiButtonCancelPress(GuiButton* button,GuiScreen* screen,World* world);

void GuiButtonDummyUpdate(GuiButton* button,GuiScreen* screen,World* world);*/