#pragma once

#include "Objects.h"

#include <list>
#include <vector>
#include <map>
#include <memory>

// Gerenciador da janela (Objetos, renderização, estado)
class WindowMgr {
private:
	static int W, H;
	static bool running;
public:
	static const int Wmax = 2560, Hmax = 1080;

	static bool isRunning();
	static void setRunning();

	static int getWidth();
	static int getHeight();

private:
	std::list< std::unique_ptr<Object> > objList;
	std::vector< std::vector<const float*> > screen;
	static int maxHist;
	void render();

	WindowMgr();
	WindowMgr(const WindowMgr&) = delete;
	WindowMgr& operator=(const WindowMgr&) = delete;
	WindowMgr(WindowMgr&&) = delete;
	WindowMgr& operator=(WindowMgr&&) = delete;
public:
	static WindowMgr& getInstance();

	void reshapeCB(int w, int h);
	void displayCB();
	void add(Line& line);
	void add(Triangle& triangle);
	void add(Rect& rectangle);
	void add(Poly& line);
	void add(Circle& line);
	void add(Blob& blob);

	void transformLast(Matrix3 m);
	std::vector< std::vector<const float*> >& getScreen();
};

// Gerenciador de input
class InputMgr {
public:
	enum {
		SELECT = 0,
		LINE, TRIANGLE, RECTANGLE, POLYGON, CIRCLE, PAINT,
		TRANSLATE, SCALE, SHEAR, REFLECT, ROTATE,
		_LAST_MODE
	};
private:
	std::unique_ptr< Object > objPreview;
	std::list<Point> promisePoints;
	const int expectedVals[_LAST_MODE] = { 0,
										   2, 3, 2, -1, 2, 1,
										   0, 0, 0, 0, 0 };
	int clicksExpected, clicksGot;
	const float* color;
	bool fill;

	int modeCurr;
	const char* modeNames[_LAST_MODE] = {
		"Select",
		"Line", "Triangle", "Rectangle", "Polygon", "Circle", "Paint",
		"Translate", "Scale", "Shear", "Reflect", "Rotate"
	};

	inline bool isModeDrawTools() {
		return modeCurr >= LINE && modeCurr <= PAINT;
	}

	inline bool isModeTransformTools() {
		return modeCurr >= TRANSLATE && modeCurr <= ROTATE;
	}

	void flushObj();

	InputMgr();
	InputMgr(const InputMgr&) = delete;
	InputMgr& operator=(const InputMgr&) = delete;
	InputMgr(InputMgr&&) = delete;
	InputMgr& operator=(InputMgr&&) = delete;
public:

	static InputMgr& getInstance();

	std::unique_ptr<Object>& getPreviewObj();
	void keyboardCB(unsigned char key, int x, int y);
	void specialKeyboardCB(int key, int x, int y);
	void mouseCB(int button, int state, int x, int y);
	void mouseHoverCB(int x, int y);
	InputMgr& setMode(int mode);
	void addPoint(Point p1);
	InputMgr& setColor(const float* newColor);
	const float* getColor();
};

// Função que inicializa o OpenGL e o GLUT
void run(int* argc, char** argv);