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

double inline det(const Point &u, const Point &v) {
    return pow((real(u)-real(v)),2) + pow((imag(u)-imag(v)),2);
}

// Return true iff [a,b] intersects [c,d], and store the intersection in ans
bool intersect_segment(const Point &a, const Point &b, const Point &c, const Point &d, Point &ans) {
    
    
    double s1_x = real(b) - real(a);
    double s1_y = imag(b) - imag(a);
    double s2_x = real(d) - real(c);
    double s2_y = imag(d) - imag(c);
    
    double s, t;
    s = (-s1_y * (real(a) - real(c)) + s1_x * (imag(a) - imag(c))) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (imag(a) - imag(c)) - s2_y * (real(a) - real(c))) / (-s2_x * s1_y + s1_x * s2_y);
    
    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // if intersection between two line segments
        ans.real(real(a) + (t * s1_x));
        ans.imag(imag(a) + (t * s1_y));
        return true;
    }
    return false; // no intersection
    
}

////////////////////////////////////////////////////////////////////////////////

bool is_inside(const Polygon &poly, const Point &query) {
	Point outside(0, 0);
    Point intersection;
    vector<Point> intersections;
    vector<Point>::iterator ip;
    bool cross;
    int number_of_intersection = 0;
    
    intersections.clear();
    for(auto it = poly.begin(); it != poly.end(); ++it){

        if (*it == poly.end()[-1]){
            // cross the line connecting to the first point and the last point
            cross = intersect_segment(*it,*poly.begin(),query,outside,intersection);
        }else{
            cross = intersect_segment(*it,*next(it,1),query,outside,intersection);
        }
        if (cross){
            // if line crossed, add intersection to the list
            Point point(real(intersection),imag(intersection));
            intersections.push_back(point);
            
        }
    }
    
    // remove any duplication
    ip = unique(intersections.begin(),intersections.end());
    intersections.resize(distance(intersections.begin(),ip));
    number_of_intersection = intersections.size();
   
    if (number_of_intersection%2 == 1){
        return true;
    }else{
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////

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

Polygon load_obj(const std::string &filename) {
    
	std::ifstream infile(filename);

    Polygon hull;
    if (infile.fail()){
        cerr << "Error opening the file!" << endl;
        infile.close();
        exit(1);
    }
    int size = -1;
    double a,b;
    string line;
    const char delim = ' ';
    char v = 'v';
    while(getline(infile,line)){
        
        // if the line starts with 'v'
        if (line.rfind("v", 0)==0){
            
            std::istringstream is(line);
            is >> v >> a >> b;
            Point point (a,b);
            hull.push_back(point);
        }
    }
    
    infile.close();
	return hull;
}

void save_xyz(const std::string &filename, const std::vector<Point> &points) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("failed to open file " + filename);
    }
    out << std::fixed;
    
    for (const auto &v : points) {
        out << v.real() << ' ' << v.imag() << " 0\n";
    }
    out << std::endl;


}




////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[]) {
    
    
    if (argc <= 3) {
		std::cerr << "Usage: " << argv[0] << " points.xyz poly.obj result.xyz" << std::endl;
	}
	std::vector<Point> points = load_xyz(argv[1]);
    
	Polygon poly = load_obj(argv[2]);
    
    std::vector<Point> result;
	for (size_t i = 0; i < points.size(); ++i) {
        
		if (is_inside(poly, points[i])) {
			result.push_back(points[i]);
        }
	}
	save_xyz(argv[3], result);
	return 0;
}
