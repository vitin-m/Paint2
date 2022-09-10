#pragma once

#include "LinAlg.h"

#include <list>
#include <vector>

namespace Rasterizer {
	auto reduceLineTo1stOct(Point, Point);

	std::list<Point> rasterLineImmediate(Point, Point);
	std::list<Point> rasterLineBreseham(Point, Point);
	std::list<Point> rasterCircBreseham(Point, int);

	std::list<Point> circleFill(Point, int);
	std::list<Point> polygonFill(std::list<Point>);
	std::list<Point> floodFill(std::vector< std::vector<const float*> >&, int, int, const float*);
}