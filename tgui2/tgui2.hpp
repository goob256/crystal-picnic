#ifndef TGUI_H
#define TGUI_H

#ifdef _WIN32
// Disable warnings about dll-interface. Even Microsoft admits they're superfluous
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif

#ifdef _WIN32
#ifdef TGUI2_STATIC
#define TGUI2_EXPORT
#else
#ifdef TGUI2_BUILD
#define TGUI2_EXPORT __declspec(dllexport)
#else
#define TGUI2_EXPORT __declspec(dllimport)
#endif
#endif
#else
#define TGUI2_EXPORT
#endif

#include <vector>
#include <algorithm>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#ifndef ALLEGRO_WINDOWS
#include <sys/time.h>
#endif

#if !defined MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#if !defined MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace tgui {

// forward declarations
void TGUI2_EXPORT getScreenSize(int *w, int *h);

class TGUI2_EXPORT TGUIWidget {
public:
	friend void TGUI2_EXPORT drawRect(int x1, int y1, int x2, int y2);
	friend void TGUI2_EXPORT handleEvent_pretransformed(void *allegro_event);
	friend void TGUI2_EXPORT handleEvent(void *allegro_event);

	float getX() { return x; }
	float getY() { return y; }
	virtual void setX(float newX) { x = newX; }
	virtual void setY(float newY) { y = newY; }

	int getWidth() { return width; }
	int getHeight() { return height; }
	virtual void setWidth(int w) { width = w; }
	virtual void setHeight(int h) { height = h; }

	TGUIWidget *getParent() { return parent; }
	void setParent(TGUIWidget *p) { parent = p; }

	TGUIWidget *getChild() { return child; }
	void setChild(TGUIWidget *c) { child = c; }
	virtual bool getAbsoluteChildPosition(TGUIWidget *child, int *x, int *y) { return false; }

	virtual void draw(int abs_x, int abs_y) {}
	// -- only called if registered
	virtual void preDraw(int abs_x, int abs_y) {}
	virtual void postDraw(int abs_x, int abs_y) {}
	// --

	virtual TGUIWidget *update() {
		TGUIWidget *w;
		if (child) {
			w = child->update();
			if (w) return w;
		}
		return NULL;
	}
	virtual void resize() {
		resize_self();
		resize_child();
	}
	virtual void translate(int xx, int yy) {
		if (child) {
			child->translate(xx, yy);
		}
	}

	virtual void raise();
	virtual void lower();

	// give relative and absolute coordinates. rel_x/y can be -1 if not
	// over widget
	// keyChar and joyAxis should return true if a directional event was used
	// (ie left/right/up/down arrows/axis)
	virtual void mouseMove(int rel_x, int rel_y, int abs_x, int abs_y) {}
	virtual void mouseScroll(int z, int w, int dz = 0, int dw = 0) {}
	virtual void mouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb) {}
	virtual void mouseUp(int rel_x, int rel_y, int abs_x, int abs_y, int b) {}
	virtual void keyDown(int keycode) {}
	virtual void keyUp(int keycode) {}
	virtual bool keyChar(int keycode, int unichar) { return false; }
	virtual void joyButtonDown(int button) {}
	virtual void joyButtonDownRepeat(int button) {}
	virtual void joyButtonUp(int button) {}
	virtual void joyAxis(int stick, int axis, float value) {}
	virtual bool joyAxisRepeat(int stick, int axis, float value) { return false; }

	virtual void mouseMoveAll(TGUIWidget *leftOut, int abs_x, int abs_y)
	{
		if (this != leftOut) {
			mouseMove(-1, -1, abs_x, abs_y);
		}
		if (child) {
			child->mouseMoveAll(leftOut, abs_x, abs_y);
		}
	}
	virtual void mouseDownAll(TGUIWidget *leftOut, int abs_x, int abs_y, int mb)
	{
		if (this != leftOut) {
			mouseDown(-1, -1, abs_x, abs_y, mb);
		}
		if (child) {
			child->mouseDownAll(leftOut, abs_x, abs_y, mb);
		}
	}
	virtual void mouseUpAll(TGUIWidget *leftOut, int abs_x, int abs_y, int mb)
	{
		if (this != leftOut) {
			mouseUp(-1, -1, abs_x, abs_y, mb);
		}
		if (child) {
			child->mouseUpAll(leftOut, abs_x, abs_y, mb);
		}
	}

	virtual void remove();
	
	virtual bool acceptsFocus() { return false; }


	virtual TGUIWidget *chainMouseMove(int rel_x, int rel_y, int abs_x, int abs_y, int z, int w, int dz = 0, int dw = 0);
	virtual TGUIWidget *chainMouseDown(int rel_x, int rel_y, int abs_x, int abs_y, int mb);
	virtual TGUIWidget *chainMouseUp(int rel_x, int rel_y, int abs_x, int abs_y, int b);
	virtual void chainKeyDown(int keycode);
	virtual void chainKeyUp(int keycode);
	virtual bool chainKeyChar(int keycode, int unichar);
	virtual void chainJoyButtonDown(int button);
	virtual void chainJoyButtonDownRepeat(int button);
	virtual void chainJoyButtonUp(int button);
	virtual void chainJoyAxis(int stick, int axis, float value);
	virtual bool chainJoyAxisRepeat(int stick, int axis, float value);
	virtual void chainDraw();

	virtual void losingFocus() {}
	virtual void gainingFocus() {}

	virtual void addCollidingChildrenToVector(std::vector<tgui::TGUIWidget *> &v, tgui::TGUIWidget *exception, int x1, int y1, int x2, int y2) {}

	void setFocusGroup(int focusGroup, int numberInFocusGroup) {
		this->focusGroup = focusGroup;
		this->numberInFocusGroup = numberInFocusGroup;
	}
	bool getDrawFocus() { return drawFocus; }
	void setDrawFocus(bool draw) { drawFocus = draw; }

	TGUIWidget() :
		parent(NULL),
		child(NULL),
		focusGroup(0),
		drawFocus(true)
	{
	}

	virtual ~TGUIWidget() {}

protected:

	void resize_self() {
		if (parent) {
			width = parent->getWidth();
			height = parent->getHeight();
		}
		else {
			int w, h;
			tgui::getScreenSize(&w, &h);
			width = w;
			height = h;
		}
	}

	void resize_child() {
		if (child)
			child->resize();
	}

	float x;
	float y;
	float width;
	float height;
	TGUIWidget *parent;
	TGUIWidget *child;

	int focusGroup;
	int numberInFocusGroup;
	bool drawFocus;
};

long TGUI2_EXPORT currentTimeMillis();
void TGUI2_EXPORT init(ALLEGRO_DISPLAY *display);
void TGUI2_EXPORT shutdown();
void TGUI2_EXPORT setFocus(TGUIWidget *widget);
TGUIWidget TGUI2_EXPORT *getFocussedWidget();
void TGUI2_EXPORT focusPrevious();
void TGUI2_EXPORT focusNext();
void TGUI2_EXPORT translateAll(int x, int y);
void TGUI2_EXPORT addWidget(TGUIWidget *widget);
TGUIWidget TGUI2_EXPORT *update();
std::vector<TGUIWidget *> TGUI2_EXPORT updateAll();
void TGUI2_EXPORT draw();
void TGUI2_EXPORT drawRect(int x1, int y1, int x2, int y2);
void TGUI2_EXPORT push();
bool TGUI2_EXPORT pop();
void TGUI2_EXPORT setNewWidgetParent(TGUIWidget *parent);
TGUIWidget TGUI2_EXPORT *getNewWidgetParent();
void TGUI2_EXPORT centerWidget(TGUIWidget *widget, int x, int y);
bool TGUI2_EXPORT widgetIsChildOf(TGUIWidget *widget, TGUIWidget *parent);
void TGUI2_EXPORT setScale(float x_scale, float y_scale);
void TGUI2_EXPORT setOffset(float x_offset, float y_offset);
void TGUI2_EXPORT ignore(int type);
void TGUI2_EXPORT convertMousePosition(int *x, int *y);
void TGUI2_EXPORT bufferToScreenPos(int *x, int *y, int bw, int bh);
void TGUI2_EXPORT handleEvent_pretransformed(void *allegro_event);
void TGUI2_EXPORT handleEvent(void *allegro_event);
TGUIWidget TGUI2_EXPORT *getTopLevelParent(TGUIWidget *widget);
ALLEGRO_FONT TGUI2_EXPORT *getFont();
void TGUI2_EXPORT setFont(ALLEGRO_FONT *font);
void TGUI2_EXPORT determineAbsolutePosition(TGUIWidget *widget, int *x, int *y);
TGUIWidget TGUI2_EXPORT *determineTopLevelOwner(int x, int y);
bool TGUI2_EXPORT pointOnWidget(TGUIWidget *widget, int x, int y);
void TGUI2_EXPORT resize(TGUIWidget *parent);
void TGUI2_EXPORT clearClip();
void TGUI2_EXPORT setClippedClip(int x, int y, int width, int height);
void TGUI2_EXPORT setClip(int x, int y, int width, int height);
bool TGUI2_EXPORT isClipSet();
void TGUI2_EXPORT getClip(int *x, int *y, int *w, int *h);
void TGUI2_EXPORT raiseWidget(TGUIWidget *widget);
void TGUI2_EXPORT lowerWidget(TGUIWidget *widget);
bool TGUI2_EXPORT isDeepChild(TGUIWidget *parent, TGUIWidget *widget);
std::vector<TGUIWidget *> TGUI2_EXPORT removeChildren(TGUIWidget *widget);
void TGUI2_EXPORT addPreDrawWidget(TGUIWidget *widget);
void TGUI2_EXPORT addPostDrawWidget(TGUIWidget *widget);
bool TGUI2_EXPORT isKeyDown(int keycode);
void TGUI2_EXPORT clearKeyState();
void TGUI2_EXPORT setScreenSize(int w, int h);
ALLEGRO_DISPLAY TGUI2_EXPORT *getDisplay();
void TGUI2_EXPORT drawFocusRectangle(int x, int y, int w, int h);
bool TGUI2_EXPORT checkBoxCollision(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void TGUI2_EXPORT hide();
void TGUI2_EXPORT unhide();
void TGUI2_EXPORT releaseKeysAndButtons();

} // End namespace tgui

#endif
