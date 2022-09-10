#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>

struct Point;
class Matrix3;

namespace _math {
	const double pi = std::acos(-1);
	inline double cvtDEG2RAD(const double& deg) {
		return (deg / 180) * pi;
	}
}

struct Point {
	int x;
	int y;

	Point();
	Point(int x, int y);

	void swapXY();
	int manhattanDistance(Point p);
	double euclideanDistance(Point p);

	// Operadores
	Point operator-(const Point& p);
	Point operator-(const Point& p) const;
	Point operator+(const Point& p);
	Point operator+(const Point& p) const;
	Point operator*(const Matrix3& m);
	Point operator*(const Matrix3& m) const;
	Point operator/(const float div);
	Point operator/(const float div) const;
	bool operator<(const Point p);
	bool operator<(const Point p) const;
	friend bool operator==(const Point& lhs, const Point& rhs);
	friend std::ostream& operator<<(std::ostream& out, const Point& p);
};

class Matrix3 {
	double elements[9];

public:
	// Construtores
	Matrix3(
		double a0, double a1, double a2,
		double a3, double a4, double a5,
		double a6, double a7, double a8
	);

	Matrix3(double* elems);

	Matrix3();

	// Usos
	static Matrix3 getZerosMatrix() {
		return Matrix3(
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
		);
	}

	static Matrix3 getIdentityMatrix() {
		return Matrix3(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		);
	}

	static Matrix3 get2DTranslationMatrix(int Tx = 0, int Ty = 0) {
		return Matrix3(
			1, 0, 0,
			0, 1, 0,
			Tx, Ty, 1
		);
	}

	static Matrix3 get2DScaleMatrix(float Sx = 1, float Sy = 1) {
		return Matrix3(
			Sx, 0, 0,
			0, Sy, 0,
			0, 0, 1
		);
	}

	static Matrix3 get2DShearMatrix(float Cx = 1, float Cy = 1) {
		return Matrix3(
			1, Cy, 0,
			Cx, 1, 0,
			0, 0, 1
		);
	}

	static Matrix3 get2DReflectionMatrix(bool Rx = false, bool Ry = false) {
		Matrix3 m = getIdentityMatrix();
		if (Rx) m[0] = -1;
		if (Ry) m[4] = -1;
		return m;
	}

	static Matrix3 get2DRotationMatrix(double theta = 0, bool cw = false) {
		theta = std::fmod(theta, 360);
		if (cw) theta = 360 - theta;
		theta = _math::cvtDEG2RAD(theta);
		double s = std::sin(theta), c = std::cos(theta);
		return Matrix3(
			c, s, 0,
			-s, c, 0,
			0, 0, 1
		);
	}

	// Operadores
	double& operator[](int index);
	double operator[](int index) const;
	Point operator*(const Point p);
	Point operator*(const Point p) const;
	Matrix3 operator*(const Matrix3 m);
	Matrix3 operator*(const Matrix3 m) const;

	friend std::ostream& operator<<(std::ostream& out, const Matrix3& m);
};