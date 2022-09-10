#include "Objects.h"
#include "Rasterizer.h"
#include <iostream>

Object::Object() {
	fill = false;
}

void Object::calculateCentroid() {
	centroid = Point();
	for (const auto& p : points)
		centroid = centroid + p;
	centroid = centroid / points.size();
}

Object& Object::setFill(bool fill) {
	if (this->fill == fill) return *this;
	this->fill = fill;
	raster();

	return *this;
}

Object& Object::switchFill() {
	fill = !fill;
	raster();

	return *this;
}

bool Object::getFill() {
	return fill;
}

Object& Object::setColor(const float* newColor) {
	color = newColor;

	return *this;
}

const float* Object::getColor() {
	return color;
}

std::list<Point> Object::getRaster() {
	return rasterizedPoints;
}

std::list<Point> Object::getPoints() {
	return points;
}

Object& Object::applyTransform(Matrix3 m) {
	transform = transform * m;

	const auto opCentroid = centroid;
	const auto centroidTransform =
		Matrix3::get2DTranslationMatrix(-opCentroid.x, -opCentroid.y)
		* transform
		* Matrix3::get2DTranslationMatrix(opCentroid.x, opCentroid.y);

	std::list<Point> newPoints;
	for (const auto& p : originalPoints) newPoints.push_back(p * centroidTransform);
	points = newPoints;

	rasterizedPoints.clear();
	raster();

	return *this;
}

Line::Line(Point p1, Point p2) : Object() {
	points.push_back(p1);
	points.push_back(p2);
	originalPoints = points;

	calculateCentroid();
	raster();
}

Line::Line(std::list<Point> lsp) : Line(lsp.front(), lsp.back()) {}

void Line::raster() {
	rasterizedPoints = Rasterizer::rasterLineBreseham(points.front(), points.back());
}

Triangle::Triangle(Point p1, Point p2, Point p3) : Object() {
	points.push_back(p1);
	points.push_back(p2);
	points.push_back(p3);
	originalPoints = points;

	calculateCentroid();
	raster();
}

void Triangle::raster() {
	rasterizedPoints = Rasterizer::rasterLineBreseham(points.front(), points.back());

	auto pnext = points.begin();
	pnext++;
	for (auto p = points.begin(); pnext != points.end(); p++, pnext++)
		rasterizedPoints.splice(rasterizedPoints.end(),
			Rasterizer::rasterLineBreseham(*p, *pnext));

	if (fill) rasterizedPoints.splice(rasterizedPoints.end(),
		Rasterizer::polygonFill(points));

}

Rect::Rect(Point p1, Point p2) : Object() {
	points.push_back(p1);
	points.push_back(Point(p2.x, p1.y));
	points.push_back(p2);
	points.push_back(Point(p1.x, p2.y));
	originalPoints = points;

	calculateCentroid();
	raster();
}

void Rect::raster() {
	rasterizedPoints = Rasterizer::rasterLineBreseham(points.front(), points.back());

	auto pnext = points.begin();
	pnext++;
	for (auto p = points.begin(); pnext != points.end(); p++, pnext++)
		rasterizedPoints.splice(rasterizedPoints.end(),
			Rasterizer::rasterLineBreseham(*p, *pnext));

	if (fill) rasterizedPoints.splice(rasterizedPoints.end(),
		Rasterizer::polygonFill(points));

}

Poly::Poly(std::list<Point> lsp) : Object() {
	points = lsp;
	originalPoints = points;

	calculateCentroid();
	raster();
}

void Poly::raster() {
	rasterizedPoints = Rasterizer::rasterLineBreseham(points.front(), points.back());

	auto pnext = points.begin();
	pnext++;
	for (auto p = points.begin(); pnext != points.end(); p++, pnext++)
		rasterizedPoints.splice(rasterizedPoints.end(),
			Rasterizer::rasterLineBreseham(*p, *pnext));

	if (fill) rasterizedPoints.splice(rasterizedPoints.end(),
		Rasterizer::polygonFill(points));
}

Circle::Circle(Point c, Point border) : Object() {
	points.push_back(c);
	points.push_back(border);
	originalPoints = points;

	calculateCentroid();
	raster();
}

void Circle::calculateCentroid() {
	centroid = points.front();
}

void Circle::raster() {
	radius = points.front().euclideanDistance(points.back());
	rasterizedPoints = Rasterizer::rasterCircBreseham(points.front(), radius);

	if (fill) rasterizedPoints.splice(rasterizedPoints.end(),
		Rasterizer::circleFill(points.front(), radius));
}

Blob::Blob(std::vector< std::vector< const float* > >& screen, Point start, const float* color) {
	color = color;
	points = rasterizedPoints = originalPoints = Rasterizer::floodFill(screen, start.x, start.y, color);
}

// Operadores

std::ostream& operator<<(std::ostream& out, const Object& obj) {
	obj.print(out);
	return out;
}

void Line::print(std::ostream& out) const {
	out << "Line { " << points.front() << ", " << points.back() << " }";
}

void Triangle::print(std::ostream& out) const {
	out << "Triangle { ";
	for (auto ptr = points.begin(); ++ptr != points.end();) {
		out << *--ptr << ", ";
		ptr++;
	}
	out << points.back() << " }";
}

void Rect::print(std::ostream& out) const {
	out << "Rectangle { ";
	for (auto ptr = points.begin(); ++ptr != points.end();) {
		out << *--ptr << ", ";
		ptr++;
	}
	out << points.back() << " }";
}

void Poly::print(std::ostream& out) const {
	out << "Polygon { ";
	for (auto ptr = points.begin(); ++ptr != points.end();) {
		out << *--ptr << ", ";
		ptr++;
	}
	out << points.back() << " }";
}

void Circle::print(std::ostream& out) const {
	out << "Circle { Center = " << points.front() << ", Radius = " << radius << " }";
}

void Blob::print(std::ostream& out) const {
	out << "Blob (Generated by flood fill) { ";
	if (!points.empty()) {
		auto ptr = points.begin();
		if (ptr != points.end()) out << *++ptr << ", ";
		if (ptr != points.end()) out << *++ptr << ", ";
		//if () out << "..., " << points.back();
	}
	out << points.back() << " }";
}

//int main() {
//	Point p1(0, 0), p2(5, 5);
//	Rect r(p1, p2);
//
//	for (auto const& p : r.getRaster()) std::cout << "(" << p.x << "," << p.y << ")\n";
//	
//	r.applyTransform(Matrix3::get2DTranslationMatrix(1, 2));
//	std::cout << "Transladado em Tx = 1, Ty = 2:\n";
//	for (auto const& p : r.getRaster()) std::cout << "(" << p.x << "," << p.y << ")\n";
//}