#pragma once

#include "LinAlg.h"

#include <iostream>
#include <list>
#include <vector>

// Interface de objeto
class Object {
protected:
	Point centroid;
	Matrix3 transform;
	std::list<Point> originalPoints;
	std::list<Point> points;
	std::list<Point> rasterizedPoints;
	bool fill;
	const float defaultColor[3] = { 255, 255, 255 };
	const float* color = defaultColor;

	void calculateCentroid();
	virtual void raster() = 0;
	friend std::ostream& operator<<(std::ostream&, const Object&);
	Object();

public:

	Object& applyTransform(Matrix3);

	Object& setFill(bool);
	Object& switchFill();
	bool getFill();
	Object& setColor(const float*);
	const float* getColor();

	std::list<Point> getPoints();
	std::list<Point> getRaster();

	virtual void print(std::ostream&) const = 0;
};

class Line : public Object {
public:
	Line(Point, Point);
	Line(std::list<Point>);
	void raster();

	void print(std::ostream&) const;
};

class Triangle : public Object {
public:
	Triangle(Point, Point, Point);
	void raster();

	void print(std::ostream&) const;
};

class Rect : public Object {
public:
	Rect(Point, Point);
	void raster();

	void print(std::ostream&) const;
};

class Poly : public Object {
public:
	static const int snapDist = 8;

	Poly(std::list<Point>);
	void raster();

	void print(std::ostream&) const;
};

class Circle : public Object {
private:
	int radius;

	void calculateCentroid();
public:
	Circle(Point, Point);
	void raster();

	void print(std::ostream&) const;
};

class Blob : public Object {
public:
	Blob(std::vector< std::vector< const float* > >&, Point, const float* color);
	inline void raster() {}
	inline Object& applyTransform(Matrix3 m) { return *this; }

	void print(std::ostream&) const;
};