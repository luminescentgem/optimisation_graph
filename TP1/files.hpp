#ifndef FILES_HPP
#define FILES_HPP

#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "graph.hpp"

rapidjson::Document read_json(std::string filename)
{
  std::ifstream in(filename, std::ifstream::in | std::ifstream::binary);
  if (!in.is_open()) {
    std::cerr << "Error reading " << filename << std::endl;
    assert(false);
    exit(EXIT_FAILURE);
  }

  rapidjson::IStreamWrapper isw {in};
  rapidjson::Document doc {};
  doc.ParseStream(isw);

  if (doc.HasParseError()) {
    std::cerr << "Error  : " << doc.GetParseError()  << std::endl;
    std::cerr << "Offset : " << doc.GetErrorOffset() << std::endl;
    exit(EXIT_FAILURE);
  }
  return doc;
}

std::vector<int> json_int_vec(rapidjson::Value &values) {
  std::vector<int> v;
  for (auto &x : values.GetArray())
    v.push_back((int)x.GetDouble());
  return v;
}

using i64 = long long int;

class Point {
public:
  i64 x,y;

  Point() {
  }

  Point(i64 _x, i64 _y) : x(_x), y(_y) {
  }

  friend bool operator==(const Point &p, const Point &q) {
    return p.x == q.x && p.y == q.y;
  }

  friend bool operator!=(const Point &p, const Point &q) {
    return !(p == q);
  }

  friend bool operator<(const Point &p, const Point &q) {
    return p.x < q.x || (p.x == q.x && p.y < q.y);
  }

  Point operator-(const Point &p) const {
    return Point(x-p.x, y-p.y);
  }

  Point operator+(const Point &p) const {
    return Point(x+p.x, y+p.y);
  }

  // Dot product
  i64 operator*(const Point &p) const {
    return x*p.x + y*p.y;
  }

  std::string toString() const {
    std::string s = "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    return s;
  }

  bool inside(const Point &p, const Point &q) const {
    i64 minx = std::min(p.x, q.x);
    i64 miny = std::min(p.y, q.y);
    i64 maxx = std::max(p.x, q.x);
    i64 maxy = std::max(p.y, q.y);
    return x >= minx && x <= maxx && y >= miny && y <= maxy;
  }

  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    os << p.toString();
    return os;
  }
};


class Segment {
  Point p,q;
public:
  Segment() {
  }

  Segment(const Point  &_p, const Point  &_q) {
    if (_p < _q) {
      p = _p;
      q = _q;
    }
    else {
      p = _q;
      q = _p;
    }
  }

  const Point &get_p() const {
    return p;
  }

  const Point &get_q() const{
    return q;
  }

  friend bool operator==(const Segment &s, const Segment &t) {
    return s.p == t.p && s.q == t.q;
  }

  int orientation(const Point &r) const {
    i64 d1 = (q.y - p.y);
    i64 d2 = (r.x - q.x);
    i64 d3 = (q.x - p.x);
    i64 d4 = (r.y - q.y);
    i64 val = d1*d2 - d3*d4;

    return (val > 0) - (val < 0);
  }

  bool cross(const Segment &s) const {
    int o1 = orientation(s.p);
    int o2 = orientation(s.q);
    int o3 = s.orientation(p);
    int o4 = s.orientation(q);

    // No 3 colinear points
    if (o1 != 0 && o2 != 0 && o3 != 0 && o4 != 0) {
      return o1 != o2 && o3 != o4;
    }

    // Colinear but 4 distinct points
    if (s.p != p && s.q != q && s.p != q && s.q != p)
      return s.p.inside(p, q) || s.q.inside(p, q) || p.inside(s.p, s.q) || q.inside(s.p, s.q);

    // Same segment twice, return false for convinience
    if (*this == s)
      return false;

    // 3 points among 4 vertices, not all colinear
    if (o1 != 0 || o2 != 0 || o3 != 0 || o4 != 0)
      return false;

    // 3 points among 4 vertices, all colinear
    if (s.p == p)
      return q.inside(s.p, s.q) || s.q.inside(p, q);
    if (s.q == q)
      return p.inside(s.p, s.q) || s.p.inside(p, q);
    if (s.p == q)
      return p.inside(s.p, s.q) || s.q.inside(p, q);
    //    if (s.q == p)
    return q.inside(s.p, s.q) || s.p.inside(p, q);
  }

  std::string toString() const {
    std::string s = p.toString() + "-" + q.toString();
    return s;
  }

  friend std::ostream& operator<<(std::ostream& os, const Segment& s) {
    os << s.toString();
    return os;
  }
};

Graph<int> readGraph(std::string fn) {
  rapidjson::Document doc = read_json(fn);
  Graph<int> g;

  const std::vector<int> x_vec = json_int_vec(doc["x"]);
  const std::vector<int> y_vec = json_int_vec(doc["y"]);
  const std::vector<int> i_vec = json_int_vec(doc["edge_i"]);
  const std::vector<int> j_vec = json_int_vec(doc["edge_j"]);
  std::vector<Segment> segments;
  for (size_t k = 0; k < i_vec.size(); k++) {
      Point p(x_vec[i_vec[k]], y_vec[i_vec[k]]);
      Point q(x_vec[j_vec[k]], y_vec[j_vec[k]]);
      segments.push_back(Segment(p, q));
  }

  int n = segments.size();

  for(int i = 0; i < n; i++)
    g.addVertex(i);

  for(int i = 0; i < segments.size(); i++)
    for(int j = 0; j < segments.size(); j++)
      if(segments[i].cross(segments[j]))
        g.addEdge(i,j);

  return g;
}

#endif
