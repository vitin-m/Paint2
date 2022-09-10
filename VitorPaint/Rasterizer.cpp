#include "Rasterizer.h"
#include <iostream>

auto Rasterizer::reduceLineTo1stOct(Point p1, Point p2) {
	int dx = p2.x - p1.x, dy = p2.y - p1.y;
	bool S = false, D = false;
	Point _p1(p1), _p2(p2);

	// Check m < 0
	if (dx * dy < 0) {
		_p1.y = -_p1.y;
		_p2.y = -_p2.y;

		S = true;
	}

	// Check m > 1
	if (std::abs(dx) < std::abs(dy)) {
		_p1.swapXY();
		_p2.swapXY();

		D = true;
	}

	// Check x1 > x2
	if (_p1.x > _p2.x) {
		Point tmp(_p1);
		_p1 = _p2;
		_p2 = tmp;
	}

	struct _ { Point p1, p2; bool S, D; };
	return _{ _p1, _p2, S, D };
}

std::list<Point> Rasterizer::rasterLineImmediate(Point p1, Point p2) {
	std::list<Point> raster;
	auto [_p1, _p2, S, D] = Rasterizer::reduceLineTo1stOct(p1, p2);
	float m = (float)(_p2.y - _p1.y) / (_p2.x - _p1.x);

	float y = _p1.y;
	while (_p1.x <= _p2.x) {
		Point p(_p1.x, std::floor(y));
		if (D) p.swapXY();
		if (S) p.y = -p.y;
		raster.push_back(p);

		y += m;
		_p1.x++;
	}

	return raster;
}

std::list<Point> Rasterizer::rasterLineBreseham(Point p1, Point p2) {
	std::list<Point> raster;
	auto [_p1, _p2, S, D] = Rasterizer::reduceLineTo1stOct(p1, p2);

	int dx = _p2.x - _p1.x;
	int dy = _p2.y - _p1.y;

	int d = 2 * dy - dx;
	int incE = 2 * dy;
	int incNE = 2 * (dy - dx);

	while (_p1.x <= _p2.x) {
		Point p(_p1.x, _p1.y);
		if (D) p.swapXY();
		if (S) p.y = -p.y;
		raster.push_back(p);

		if (d > 0) {
			d += incNE;
			_p1.y += 1;
		}
		else d += incE;

		_p1.x += 1;
	}

	return raster;
}

std::list<Point> Rasterizer::rasterCircBreseham(Point C, int R) {
	std::list<Point> raster;
	int d = 1 - R, dE = 3, dSE = -2 * R + 5;

	for (int xi = 0, yi = R; xi <= yi; xi++) {
		// 1o quad
		raster.push_back(Point(xi + C.x, yi + C.y));
		raster.push_back(Point(yi + C.x, xi + C.y));
		// 2o quad
		raster.push_back(Point(-xi + C.x, yi + C.y));
		raster.push_back(Point(yi + C.x, -xi + C.y));
		//3o quad
		raster.push_back(Point(-xi + C.x, -yi + C.y));
		raster.push_back(Point(-yi + C.x, -xi + C.y));
		//4o quad
		raster.push_back(Point(xi + C.x, -yi + C.y));
		raster.push_back(Point(-yi + C.x, xi + C.y));

		if (d < 0) {
			d += dE;
			dE += 2;
			dSE += 2;
		}
		else {
			d += dSE;
			dE += 2;
			dSE += 4;
			yi -= 1;
		}
	}

	return raster;
}

std::list<Point> Rasterizer::circleFill(Point C, int R) {
	auto fillLine = [&](std::list<Point>& raster,
		const int& x0, const int& x1, const int& y) -> void {
			int xmin = x0 < x1 ? x0 : x1;
			int xmax = x0 > x1 ? x0 : x1;

			for (int x = xmin + 1; x < xmax; x++)
				raster.push_back(Point(x, y));
	};

	std::list<Point> raster;
	int d = 1 - R, dE = 3, dSE = -2 * R + 5;

	for (int xi = 0, yi = R; xi <= yi; xi++) {
		fillLine(raster, -xi + C.x, xi + C.x, -yi + C.y);
		fillLine(raster, -xi + C.x, xi + C.x, yi + C.y);
		fillLine(raster, -yi + C.x, yi + C.x, -xi + C.y);
		fillLine(raster, -yi + C.x, yi + C.x, xi + C.y);

		if (d < 0) {
			d += dE;
			dE += 2;
			dSE += 2;
		}
		else {
			d += dSE;
			dE += 2;
			dSE += 4;
			yi -= 1;
		}
	}

	return raster;
}

std::list<Point> Rasterizer::polygonFill(std::list<Point> lsp) {
	if (lsp.size() <= 2) return std::list<Point>();
	else if (lsp.size() <= 6) {
		auto test = lsp;
		test.sort(); test.unique();
		if (test.size() <= 2) return std::list<Point>();
	}

	struct Edge {
		int ymin, ymax;
		float x_ymin, m_inv;

		Edge(int ymin, int ymax, int x_ymin, float m_inv)
			: ymin(ymin), ymax(ymax), x_ymin(x_ymin), m_inv(m_inv) {}

		bool operator<(const Edge e) {
			return x_ymin < e.x_ymin;
		}
	};

	std::list<Edge> GET;

	int min_miny = std::numeric_limits<int>::max(), max_miny = 0;

	auto lspNext = lsp.begin();

	for (auto itr = lsp.begin(); itr != lsp.end(); itr++) {
		if (++lspNext == lsp.end()) lspNext = lsp.begin();
		if (itr->y - lspNext->y == 0) continue;
		if (itr->y <= lspNext->y) {
			if (itr->y < min_miny) min_miny = itr->y;
			if (itr->y > max_miny) max_miny = itr->y;
			GET.push_back(Edge(itr->y, lspNext->y, itr->x,
				(lspNext->x - itr->x) / (float)(lspNext->y - itr->y)));
		}
		else {
			if (lspNext->y < min_miny) min_miny = lspNext->y;
			if (lspNext->y > max_miny) max_miny = lspNext->y;
			GET.push_back(Edge(lspNext->y, itr->y, lspNext->x,
				(lspNext->x - itr->x) / (float)(lspNext->y - itr->y)));
		}
	}

	std::vector< std::list<Edge> > ET(max_miny - min_miny + 1, std::list<Edge>());
	std::list<Edge> AET;
	std::list<Point> raster;

	for (auto itr = GET.begin(); itr != GET.end(); itr++)
		ET[itr->ymin - min_miny].push_back(*itr);
	int y = 0;
	while (!AET.empty() || y <= max_miny - min_miny) {
		if (y < ET.size() && !ET[y].empty()) AET.splice(AET.end(), ET[y]);
		AET.sort();

		auto first = AET.begin(), sec = AET.begin();
		if (sec != AET.end()) sec++;
		while (sec != AET.end()) {
			for (int x = std::ceil(first->x_ymin); x <= std::floor(sec->x_ymin); x++)
				raster.push_back(Point(x, y + min_miny));
			first++; sec++;
			if (sec == AET.end()) break;
			first++; sec++;
		}

		y += 1;

		for (auto itr = AET.begin(); itr != AET.end();) {
			if (y >= itr->ymax - min_miny) AET.erase(itr++);
			else {
				itr->x_ymin += itr->m_inv;
				itr++;
			}
		}
	}

	return raster;
}

std::list<Point> Rasterizer::floodFill(std::vector< std::vector<const float*> >& screen, int x, int y, const float* color) {
	const float* old = screen[x][y];
	if (color == old) return std::list<Point>();

	auto isValid = [&](const int& px, const int& py) -> bool {
		if (px < 0 || px >= screen.size() || py < 0 || py >= screen[0].size()) return false;
		const float* thisColor = screen[px][py];
		if (thisColor == old) return true;
		return false;
	};

	std::list<Point> q, raster;
	q.push_back(Point(x, y));
	while (!q.empty()) {
		Point p = q.front();
		q.pop_front();

		if (isValid(p.x + 1, p.y)) {
			screen[p.x + 1][p.y] = color;
			q.push_back(Point(p.x + 1, p.y));
			raster.push_back(Point(p.x + 1, p.y));
		}
		if (isValid(p.x - 1, p.y)) {
			screen[p.x - 1][p.y] = color;
			q.push_back(Point(p.x - 1, p.y));
			raster.push_back(Point(p.x - 1, p.y));
		}
		if (isValid(p.x, p.y + 1)) {
			screen[p.x][p.y + 1] = color;
			q.push_back(Point(p.x, p.y + 1));
			raster.push_back(Point(p.x, p.y + 1));
		}
		if (isValid(p.x, p.y - 1)) {
			screen[p.x][p.y - 1] = color;
			q.push_back(Point(p.x, p.y - 1));
			raster.push_back(Point(p.x, p.y - 1));
		}
	}

	return raster;
}

//int main() {
//	Point p1(0, 0), p2(1, 5);
//	std::list<Point> pn({ Point(2,3), Point(7,1), Point(13,5), Point(13,11),
//		Point(7,7), Point(2,9)});
//	for (const auto& p : Rasterizer::polygonFill(pn))
//		std::cout << p << "\n";
//}