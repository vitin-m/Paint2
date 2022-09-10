#include <GL/glut.h>
#include "Managers.h"
#include <iostream>

namespace _Colors {
	constexpr float WHITE[3] = { 1.f, 1.f, 1.f };
	constexpr float BLACK[3] = { 0.f, 0.f, 0.f };

	constexpr float RED[3] = { 0.906, 0.510, 0.518 };
	constexpr float GREEN[3] = { 0.651, 0.820, 0.537 };
	constexpr float BLUE[3] = { 0.522, 0.757, 0.863 };

	constexpr float YELLOW[3] = { 0.898, 0.784, 0.565 };
	constexpr float PINK[3] = { 0.957, 0.722, 0.894 };
}

int WindowMgr::W = 800, WindowMgr::H = 600, WindowMgr::maxHist = 20;
bool WindowMgr::running = false;

WindowMgr::WindowMgr() :
	screen(Wmax, std::vector< const float* >(Hmax, _Colors::WHITE)) {}

bool WindowMgr::isRunning() {
	return running;
}

void WindowMgr::setRunning() {
	running = true;
}

int WindowMgr::getWidth() {
	return W;
}

int WindowMgr::getHeight() {
	return H;
}

void WindowMgr::render() {
	for (const auto& obj : objList) {
		for (const auto& p : obj->getRaster())
			if (p.x >= 0 && p.x <= Wmax && p.y >= 0 && p.y <= Hmax)
				screen[p.x][p.y] = obj->getColor();
	}
}

WindowMgr& WindowMgr::getInstance() {
	static WindowMgr instance;
	return instance;
}

void WindowMgr::add(Line& line) {
	objList.push_back(std::make_unique<Line>(line));
}

void WindowMgr::add(Triangle& triangle) {
	objList.push_back(std::make_unique<Triangle>(triangle));
}

void WindowMgr::add(Rect& rectangle) {
	objList.push_back(std::make_unique<Rect>(rectangle));
}

void WindowMgr::add(Poly& polygon) {
	objList.push_back(std::make_unique<Poly>(polygon));
}

void WindowMgr::add(Circle& circle) {
	objList.push_back(std::make_unique<Circle>(circle));
}

void WindowMgr::add(Blob& blob) {
	objList.push_back(std::make_unique<Blob>(blob));
}

void WindowMgr::transformLast(Matrix3 m) {
	if (objList.empty()) return;
	objList.back()->applyTransform(m);
}

std::vector< std::vector<const float*> >& WindowMgr::getScreen() {
	render();
	return screen;
}

InputMgr::InputMgr() {
	setMode(SELECT);
	color = _Colors::BLACK;
}

InputMgr& InputMgr::getInstance() {
	static InputMgr instance;
	return instance;
}

void InputMgr::flushObj() {
	std::cout << "New object";
	if (fill && isModeDrawTools()) std::cout << " (filled): ";
	else std::cout << ": ";

	switch (modeCurr) {
	case LINE:
	{
		Line l(promisePoints.front(), promisePoints.back());
		l.setFill(fill).setColor(color);
		WindowMgr::getInstance().add(l);
		std::cout << l;
		break;
	}
	case TRIANGLE:
	{
		auto ptr = promisePoints.begin();
		Triangle t(*ptr, *(ptr++), *((ptr++)++));
		t.setFill(fill).setColor(color);
		WindowMgr::getInstance().add(t);
		std::cout << t;
		break;
	}
	case RECTANGLE:
	{
		Rect r(promisePoints.front(), promisePoints.back());
		r.setFill(fill).setColor(color);
		WindowMgr::getInstance().add(r);
		std::cout << r;
		break;
	}
	case POLYGON:
	{
		Poly p(promisePoints);
		p.setFill(fill).setColor(color);
		WindowMgr::getInstance().add(p);
		std::cout << p;
		break;
	}
	case CIRCLE:
	{
		Circle c(promisePoints.front(), promisePoints.back());
		c.setFill(fill).setColor(color);
		WindowMgr::getInstance().add(c);
		std::cout << c;
		break;
	}
	case PAINT:
		Blob b(WindowMgr::getInstance().getScreen(), promisePoints.front(), color);
		b.setColor(color);
		WindowMgr::getInstance().add(b);
		std::cout << b;
		break;
	}

	std::cout << "\n";
	objPreview = nullptr;
	promisePoints.clear();
	clicksGot = 0;
	glutPostRedisplay();
}

InputMgr& InputMgr::setMode(int mode) {
	if (mode < 0 || mode >= _LAST_MODE) return *this;

	if (modeCurr == mode) fill = !fill;
	else fill = false;

	clicksExpected = expectedVals[mode];
	modeCurr = mode;
	promisePoints.clear();
	objPreview = nullptr;
	clicksGot = 0;
	std::cout << "Mode set: " << modeNames[mode];
	if (fill && isModeDrawTools()) std::cout << " (filled)";
	std::cout << " tool.\n";

	return *this;
}

void InputMgr::addPoint(Point p1) {
	if (!isModeDrawTools()) return;

	promisePoints.push_back(p1);

	clicksGot++;

	if (clicksGot == clicksExpected) {
		flushObj();
		return;
	}

	if (clicksGot <= 1) return;
	mouseHoverCB(p1.x, WindowMgr::getHeight() - p1.y);
	if (modeCurr == POLYGON && promisePoints.front().manhattanDistance(p1) <= Poly::snapDist) {
		promisePoints.pop_back();
		flushObj();
	}
}

std::unique_ptr<Object>& InputMgr::getPreviewObj() {
	return objPreview;
}

InputMgr& InputMgr::setColor(const float* newColor) {
	color = newColor;
	return *this;
}

const float* InputMgr::getColor() {
	return color;
}

void WindowMgr::reshapeCB(int w, int h) {
	// Projection matrix definition
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -1, 1);

	std::cout << "Reshape: (" << W << "," << H << ") to (" << w << "," << h << ")\n";

	W = w;
	H = h;

	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void InputMgr::keyboardCB(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;
	case 's':
		setMode(SELECT);
		break;
	case 'l':
		setMode(LINE);
		break;
	case 't':
		setMode(TRIANGLE);
		break;
	case 'r':
		setMode(RECTANGLE);
		break;
	case 'p':
		setMode(POLYGON);
		break;
	case 'P':
		setMode(PAINT);
		break;
	case 'c':
		setMode(CIRCLE);
		break;
	case 'T':
		setMode(TRANSLATE);
		break;
	case 'S':
		setMode(SCALE);
		break;
	case 'C':
		setMode(SHEAR);
		break;
	case 'M':
		setMode(REFLECT);
		break;
	case 'R':
		setMode(ROTATE);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void InputMgr::specialKeyboardCB(int key, int x, int y) {
	int translate = 5, rotateSmall = 2, rotateBig = 6;
	float propDown = 0.9, propUp = 1.1;

	switch (key) {
	case GLUT_KEY_LEFT:
		if (!isModeTransformTools()) break;
		{
			Matrix3 op = Matrix3::getIdentityMatrix();
			switch (modeCurr) {
			case TRANSLATE:
				op = Matrix3::get2DTranslationMatrix(-translate);
				break;
			case SCALE:
				op = Matrix3::get2DScaleMatrix(propDown);
				break;
			case SHEAR:
				op = Matrix3::get2DShearMatrix(propDown);
				break;
			case REFLECT:
				op = Matrix3::get2DReflectionMatrix(true);
				break;
			case ROTATE:
				op = Matrix3::get2DRotationMatrix(rotateSmall);
				break;
			}

			WindowMgr::getInstance().transformLast(op);
		}
		break;
	case GLUT_KEY_RIGHT:
		if (!isModeTransformTools()) break;
		{
			Matrix3 op = Matrix3::getIdentityMatrix();
			switch (modeCurr) {
			case TRANSLATE:
				op = Matrix3::get2DTranslationMatrix(translate);
				break;
			case SCALE:
				op = Matrix3::get2DScaleMatrix(propUp);
				break;
			case SHEAR:
				op = Matrix3::get2DShearMatrix(propUp);
				break;
			case REFLECT:
				op = Matrix3::get2DReflectionMatrix(true);
				break;
			case ROTATE:
				op = Matrix3::get2DRotationMatrix(rotateSmall, true);
				break;
			}

			WindowMgr::getInstance().transformLast(op);
		}
		break;
	case GLUT_KEY_DOWN:
		if (!isModeTransformTools()) break;
		{
			Matrix3 op = Matrix3::getIdentityMatrix();
			switch (modeCurr) {
			case TRANSLATE:
				op = Matrix3::get2DTranslationMatrix(0, -translate);
				break;
			case SCALE:
				op = Matrix3::get2DScaleMatrix(1, propDown);
				break;
			case SHEAR:
				op = Matrix3::get2DShearMatrix(1, propDown);
				break;
			case REFLECT:
				op = Matrix3::get2DReflectionMatrix(false, true);
				break;
			case ROTATE:
				op = Matrix3::get2DRotationMatrix(rotateBig);
				break;
			}

			WindowMgr::getInstance().transformLast(op);
		}
		break;
	case GLUT_KEY_UP:
		if (!isModeTransformTools()) break;
		{
			Matrix3 op = Matrix3::getIdentityMatrix();
			switch (modeCurr) {
			case TRANSLATE:
				op = Matrix3::get2DTranslationMatrix(0, translate);
				break;
			case SCALE:
				op = Matrix3::get2DScaleMatrix(1, propUp);
				break;
			case SHEAR:
				op = Matrix3::get2DShearMatrix(1, propUp);
				break;
			case REFLECT:
				op = Matrix3::get2DReflectionMatrix(false, true);
				break;
			case ROTATE:
				op = Matrix3::get2DRotationMatrix(rotateBig, true);
				break;
			}

			WindowMgr::getInstance().transformLast(op);
		}
		break;
	case GLUT_KEY_F1:
		color = _Colors::BLACK;
		break;
	case GLUT_KEY_F2:
		color = _Colors::RED;
		break;
	case GLUT_KEY_F3:
		color = _Colors::GREEN;
		break;
	case GLUT_KEY_F4:
		color = _Colors::BLUE;
		break;
	case GLUT_KEY_F5:
		color = _Colors::YELLOW;
		break;
	}
	glutPostRedisplay();
}

void InputMgr::mouseCB(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			switch (modeCurr) {
			case SELECT:
				break;
			case TRIANGLE:
			case LINE:
			case RECTANGLE:
			case POLYGON:
			case CIRCLE:
			case PAINT:
				addPoint(Point(x, WindowMgr::getHeight() - y));
				break;
			default:
				break;
			}
		}
	}
}

void InputMgr::mouseHoverCB(int x, int y) {
	if (!isModeDrawTools()) return;
	if (promisePoints.empty()) return;

	promisePoints.push_back(Point(x, WindowMgr::getHeight() - y));

	if (modeCurr == CIRCLE) {
		Circle c(promisePoints.front(), promisePoints.back());
		c.setFill(fill).setColor(color);
		objPreview = std::make_unique< Circle >(c);
	}
	else if (modeCurr == RECTANGLE) {
		Rect r(promisePoints.front(), promisePoints.back());
		r.setFill(fill).setColor(color);
		objPreview = std::make_unique< Rect >(r);
	}
	else {
		Poly p(promisePoints);
		p.setFill(fill).setColor(color);
		objPreview = std::make_unique< Poly >(p);
	}

	promisePoints.pop_back();
	glutPostRedisplay();
}

void WindowMgr::displayCB() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	glColor3fv(_Colors::BLACK);

	glBegin(GL_POINTS);
	for (const auto& obj : objList) {
		glColor3fv(obj->getColor());
		for (const auto& p : obj->getRaster())
			glVertex2i(p.x, p.y);
	}

	if (InputMgr::getInstance().getPreviewObj() != nullptr) {
		glColor3fv(InputMgr::getInstance().getPreviewObj()->getColor());
		for (const auto& p : InputMgr::getInstance().getPreviewObj()->getRaster())
			glVertex2i(p.x, p.y);
	}

	glEnd();
	glutSwapBuffers();
}

void reshapeCB(int w, int h) {
	WindowMgr::getInstance().reshapeCB(w, h);
}

void keyboardCB(unsigned char key, int x, int y) {
	InputMgr::getInstance().keyboardCB(key, x, y);
}

void specialKeyboardCallback(int key, int x, int y) {
	InputMgr::getInstance().specialKeyboardCB(key, x, y);
}

void mouseCallback(int button, int state, int x, int y) {
	InputMgr::getInstance().mouseCB(button, state, x, y);
}

void mouseHoverCallback(int x, int y) {
	InputMgr::getInstance().mouseHoverCB(x, y);
}

void displayCallback() {
	WindowMgr::getInstance().displayCB();
}

void run(int* argc, char** argv) {
	if (WindowMgr::isRunning()) return;
	WindowMgr::setRunning();

	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WindowMgr::getWidth(), WindowMgr::getHeight());
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paint 2 limão");

	glClearColor(1.f, 1.f, 1.f, 1.f);

	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(specialKeyboardCallback);
	glutMouseFunc(mouseCallback);
	glutPassiveMotionFunc(mouseHoverCallback);
	glutDisplayFunc(displayCallback);

	glutMainLoop();
}