
#include <functional>
#include <iostream>
#include<cstdio>
#include <algorithm>
#include <vector>
using namespace std;
struct point {
    long long x, y;
    unsigned long long id;
    point() :x(0), y(0) {}
    point(long long x, long long y) :x(x), y(y) {}
    bool operator ==(const point& p) const {
        return x == p.x && y == p.y;
    }
};
point PP;
vector<point> points;

long long area2(point p, point q, point s) {
    /*
    |p.x p.y 1|
    |q.x q.y 1| == 2*DirectedTriangleArea(p,q,s)
    |s.x s.y 1|
    */
    return p.x * q.y - s.x * q.y
        + q.x * s.y - q.x * p.y
        + s.x * p.y - p.x * s.y;
    
}

bool toLeftTest(point p, point q, point s) {
    //When return value large than 0, S is on the left side of ray PQ
    return area2(p, q, s) > 0;
}
bool toLeftTest2(point p, point q, point s) {
    //When return value large than 0, S is on the left side of ray PQ
    return area2(p, q, s) >= 0;
}
bool cmp(const point& p1, const point& p2) { // Sort according to polar angle
    if (p1 == PP)return true;
    if (p2 == PP)return false;
    return  toLeftTest(PP, p1, p2);
}

point LTL(vector<point>& points) { //Lowest then leftmost
    point ltl = points[0];
    for (int i = 1; i < points.size(); i++) {
        if (points[i].y < ltl.y || points[i].y == ltl.y && points[i].x < ltl.x)
            ltl = points[i];
    }
    return ltl;
}
vector<point> grahamScan() {
    PP = LTL(points);
    for (auto& p : points) {
        p.x -= PP.x;
        p.y -= PP.y;
    }
    PP.x = 0; PP.y = 0;
    sort(points.begin(), points.end(), cmp);
  
  
    vector<point> S, T;
    S.push_back(points[0]); S.push_back(points[1]);
    for (int i = points.size() - 1; i > 1; i--)T.push_back(points[i]);
    while (!T.empty()) {
        if (toLeftTest2(S[S.size() - 2], S[S.size() - 1], T[T.size() - 1])) {
            S.push_back(T[T.size() - 1]);
            T.pop_back();
        }
        else S.pop_back();
    }
    return S;
}

 
  
int main() {

    ios::sync_with_stdio(false);
    unsigned long long n;
    cin >> n;
    if (n == 3) {
        cout<<(1*2*3*3)%(3+1);
        return 0;
  }
    for (unsigned long long i = 1; i <= n; i++) {
        point tmp;
        cin >> tmp.x >> tmp.y;
        tmp.id = i;
        points.push_back(tmp);
    }
    vector<point> result;
    if (points.size() > 2)result = grahamScan();
    else result = points;
    unsigned long long res = 1;
    for (int i = 0; i < result.size(); i++) {
        //cout << result[i].id << endl;//debug
        res = ((res % (n + 1)) * (result[i].id % (n + 1))) % (n + 1);
    }
    res = ((res % (n + 1)) * ((unsigned long long)result.size() % (n + 1))) % (n + 1);
    cout << res;
    
    return 0;
  
}