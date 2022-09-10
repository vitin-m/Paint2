#include "LinAlg.h"

Point::Point() {
	x = 0;
	y = 0;
}

Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

void Point::swapXY() {
	int tmp = x;
	x = y;
	y = tmp;
}

int Point::manhattanDistance(Point p) {
	return std::abs(p.x - x) + std::abs(p.y - y);
}

double Point::euclideanDistance(Point p) {
	return std::sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
}

Matrix3::Matrix3(
	double a0, double a1, double a2,
	double a3, double a4, double a5,
	double a6, double a7, double a8
) {
	elements[0] = a0;
	elements[1] = a1;
	elements[2] = a2;

	elements[3] = a3;
	elements[4] = a4;
	elements[5] = a5;

	elements[6] = a6;
	elements[7] = a7;
	elements[8] = a8;
}

Matrix3::Matrix3(double* elems) {
	elements[0] = elems[0];
	elements[1] = elems[1];
	elements[2] = elems[2];

	elements[3] = elems[3];
	elements[4] = elems[4];
	elements[5] = elems[5];

	elements[6] = elems[6];
	elements[7] = elems[7];
	elements[8] = elems[8];
}

Matrix3::Matrix3() : Matrix3(Matrix3::getIdentityMatrix()) {}

// Definição dos operadores

std::ostream& operator<<(std::ostream& out, const Point& p) {
	out << "(" << p.x << "," << p.y << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Matrix3& m) {
	for (int i = 0; i < 9; i += 3)
		out << "|" << m.elements[i] << "  " << m.elements[i + 1] << "  " << m.elements[i + 2] << "|\n";
	return out;
}

bool operator==(const Point& lhs, const Point& rhs) {
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}


double& Matrix3::operator[](int index) {
	if (index >= 9) exit(0);

	return elements[index];
}

double Matrix3::operator[](int index) const {
	if (index >= 9) exit(0);

	return elements[index];
}

Point Matrix3::operator*(const Point p) {
	double M = p.x * elements[6] + p.y * elements[7] + elements[8];

	return Point(
		(int)((p.x * elements[0] + p.y * elements[1] + elements[2]) / M),
		(int)((p.x * elements[3] + p.y * elements[4] + elements[5]) / M)
	);
}

Point Matrix3::operator*(const Point p) const {
	double M = p.x * elements[6] + p.y * elements[7] + elements[8];

	return Point(
		(int)((p.x * elements[0] + p.y * elements[1] + elements[2]) / M),
		(int)((p.x * elements[3] + p.y * elements[4] + elements[5]) / M)
	);
}

Matrix3 Matrix3::operator*(const Matrix3 m) {
	double res[9];
	for (int idx = 0, i1 = 0, i2 = 0; idx < 9; idx += 3, i1 += 3) {
		res[idx + 0] = elements[i1] * m[i2 + 0] + elements[i1 + 1] * m[i2 + 3] + elements[i1 + 2] * m[i2 + 6];
		res[idx + 1] = elements[i1] * m[i2 + 1] + elements[i1 + 1] * m[i2 + 4] + elements[i1 + 2] * m[i2 + 7];
		res[idx + 2] = elements[i1] * m[i2 + 2] + elements[i1 + 1] * m[i2 + 5] + elements[i1 + 2] * m[i2 + 8];
	}

	return Matrix3(res);
}

Matrix3 Matrix3::operator*(const Matrix3 m) const {
	double res[9];
	for (int idx = 0, i1 = 0, i2 = 0; idx < 9; idx += 3, i1 += 3) {
		res[idx + 0] = elements[i1] * m[i2 + 0] + elements[i1 + 1] * m[i2 + 3] + elements[i1 + 2] * m[i2 + 6];
		res[idx + 1] = elements[i1] * m[i2 + 1] + elements[i1 + 1] * m[i2 + 4] + elements[i1 + 2] * m[i2 + 7];
		res[idx + 2] = elements[i1] * m[i2 + 2] + elements[i1 + 1] * m[i2 + 5] + elements[i1 + 2] * m[i2 + 8];
	}

	return Matrix3(res);
}

Point Point::operator-(const Point& p) {
	return Point(x - p.x, y - p.y);
}

Point Point::operator-(const Point& p) const {
	return Point(x - p.x, y - p.y);
}

Point Point::operator+(const Point& p) {
	return Point(x + p.x, y + p.y);
}

Point Point::operator+(const Point& p) const {
	return Point(x + p.x, y + p.y);
}

Point Point::operator*(const Matrix3& m) {
	double M = x * m[2] + y * m[5] + m[8];

	return Point(
		(int)((x * m[0] + y * m[3] + m[6]) / M),
		(int)((x * m[1] + y * m[4] + m[7]) / M)
	);
}

Point Point::operator*(const Matrix3& m) const {
	double M = x * m[2] + y * m[5] + m[8];

	return Point(
		(int)((x * m[0] + y * m[3] + m[6]) / M),
		(int)((x * m[1] + y * m[4] + m[7]) / M)
	);
}

Point Point::operator/(const float div) {
	return Point(x / div, y / div);
}

Point Point::operator/(const float div) const {
	return Point(x / div, y / div);
}

bool Point::operator<(const Point p) {
	return x + y < p.x + p.y;
}

bool Point::operator<(const Point p) const {
	return x + y < p.x + p.y;
}