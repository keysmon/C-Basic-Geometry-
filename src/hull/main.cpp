////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>
////////////////////////////////////////////////////////////////////////////////
using namespace std;
typedef std::complex<double> Point;
typedef std::vector<Point> Polygon;

// find distance between two points
double inline distance(const Point &u, const Point &v) {
    return pow((real(u)-real(v)),2) + pow((imag(u)-imag(v)),2);
}

struct Compare {
	Point p0; // Leftmost point of the poly
	bool operator ()(const Point &p1, const Point &p2) {
        double val = (imag(p1)-imag(p0))*(real(p2)-real(p1)) - (real(p1)-real(p0))*(imag(p2)-imag(p1));
        if (val==0){
            return distance(p0,p1) < distance(p0,p2);
        }
        return val < 0;
	}
};

// find the angle from a->b->c
bool inline salientAngle(Point &a, Point &b, Point &c) {
    double val = (imag(b)-imag(a))*(real(c)-real(b)) - (real(b)-real(a))*(imag(c)-imag(b));
    return val < 0 ;
}

////////////////////////////////////////////////////////////////////////////////

// convex hull builder
Polygon convex_hull(std::vector<Point> &points) {
	Compare order;
    // locate the bottom-most point
    Point *ymin = &points[0];
    for (auto & point: points){
        if (imag(*ymin) > imag(point)){
            ymin = &point;
        }
    }
    
    // sort points based on polar anger with origin as ymin
	order.p0 = *ymin;
    
	std::sort(points.begin(), points.end(), order);
	Polygon hull;
    hull.push_back(points.at(0));
    hull.push_back(points.at(1));
    hull.push_back(points.at(2));
    int n = points.size();
    // go through the sorted list to identify the polygon
    for (int i = 3; i < n;i++){
        int hull_size = hull.size();
        while (!salientAngle(hull[hull.size()-2],hull[hull.size()-1],points[i])){
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }
	return hull;
}

std::vector<Point> load_xyz(const std::string &filename) {
	std::vector<Point> points;
	std::ifstream infile(filename);
    if (infile.fail()){
        cerr << "Error opening the file!" << endl;
        infile.close();
        exit(1);
    }
    int size = -1;
    double a,b;
    string line;
    const char delim = ' ';
    
    while(getline(infile,line)){
        // extract the total number of points
        if (size == -1){
            size = stoi(line);

        }
        // extract all points
        else{
            std::istringstream is(line);
            is >> a >> b;
            Point point (a,b);
            points.push_back(point);
        }
    }
    infile.close();
	return points;
    
}

void save_obj(const std::string &filename, Polygon &poly) {
	std::ofstream out(filename);
	if (!out.is_open()) {
		throw std::runtime_error("failed to open file " + filename);
	}
	out << std::fixed;
	for (const auto &v : poly) {
		out << "v " << v.real() << ' ' << v.imag() << " 0\n";
	}
	for (size_t i = 0; i < poly.size(); ++i) {
		out << "l " << i+1 << ' ' << 1+(i+1)%poly.size() << "\n";
	}
	out << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[]) {
	if (argc <= 2) {
		std::cerr << "Usage: " << argv[0] << " points.xyz output.obj" << std::endl;
	}
	std::vector<Point> points = load_xyz(argv[1]);
	Polygon hull = convex_hull(points);
	save_obj(argv[2], hull);
	return 0;
}



