
#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>

#ifndef COMMON_H
#define COMMON_H

namespace lyh_cg {
    using std::sqrt;
    using std::fabs;
    class vec3;
    inline vec3 operator+ (const vec3& v1, const vec3& v2);
    inline vec3 operator- (const vec3& v1, const vec3& v2);
    inline vec3 operator* (const vec3& v1, const vec3& v2);
    inline vec3 operator/ (const vec3& v1, const vec3& v2);
    inline vec3 operator* (const vec3& v1, const double& t);
    inline vec3 operator/ (const vec3& v1, const double& t);
    inline vec3 operator* (const double& t, const vec3& v1);
    inline vec3 operator/ (const double& t, const vec3& v1);
    class vec3 {

    public:
        double e[3];
        bool isUnit;
        vec3() : e{ 0,0,0 }, isUnit(false) {};
        vec3(double e0, double e1, double e2) : e{ e0, e1, e2 }, isUnit(false)  {};
        vec3(const vec3& v2) {
            e[0] = v2.e[0]; e[1] = v2.e[1]; e[2] = v2.e[2];
        }
        inline double x() const { return e[0]; }
        inline double y() const { return e[1]; }
        inline double z() const { return e[2]; }
        inline double r() const { return e[0]; }
        inline double g() const { return e[1]; }
        inline double b() const { return e[2]; }
        inline const vec3& operator+()const { return *this; }
        //nline  vec3& operator+(){return *this;}
        inline  vec3 operator-()const { return  vec3(-e[0], -e[1], -e[2]); }
        inline  double operator[](int i)const { return e[i]; }
        inline  double& operator[](int i) { return e[i]; }
        inline  void operator=(const vec3& v2) {
            e[0] = v2.e[0]; e[1] = v2.e[1]; e[2] = v2.e[2];
        }
        inline vec3& operator+=(const vec3& v2) {
            *this = *this + v2; return *this;
        };
        inline vec3& operator-=(const vec3& v2) {
            *this = *this - v2; return *this;
        };
        inline vec3& operator*=(const vec3& v2) {
            *this = *this * v2; return *this;
        };
        inline vec3& operator/=(const vec3& v2) {
            *this = *this / v2; return *this;
        };

        inline vec3& operator*=(const double t) {
            e[0] *= t; e[1] *= t; e[2] *= t;
            return *this;
        };
        inline vec3& operator/=(const double t) {
            e[0] /= t; e[1] /= t; e[2] /= t;
            return *this;
        };
        inline double length()const {
            return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
        }
        inline double squared_length()const {
            return   e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }
        inline void make_unit_vector() {
            if (isUnit)return;
            (*this) /= length();
            isUnit = true;
        }

    };

    inline double dot(const vec3& v1, const vec3& v2) {
        return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
    }
    inline vec3 cross(const vec3& a, const vec3& b) {
        return vec3(
            a.y() * b.z() - a.z() * b.y(),
            a.z() * b.x() - a.x() * b.z(),
            a.x() * b.y() - a.y() * b.x()
        );
    }
    inline vec3 unit_vector(const vec3& v) {
        return v / v.length();
    }

    inline vec3 operator+ (const vec3& v1, const vec3& v2) {
        return vec3(
            v1.x() + v2.x(),
            v1.y() + v2.y(),
            v1.z() + v2.z()
        );
    };
    inline vec3 operator- (const vec3& v1, const vec3& v2) {
        return vec3(
            v1.x() - v2.x(),
            v1.y() - v2.y(),
            v1.z() - v2.z()
        );
    };
    inline vec3 operator* (const vec3& v1, const vec3& v2) {
        return vec3(
            v1.x() * v2.x(),
            v1.y() * v2.y(),
            v1.z() * v2.z()
        );

    };
    inline vec3 operator/ (const vec3& v1, const vec3& v2) {
        return vec3(
            v1.x() / v2.x(),
            v1.y() / v2.y(),
            v1.z() / v2.z()
        );
    };
    inline vec3 operator* (const vec3& v1, const double& t) {
        return vec3(
            v1.x() * t,
            v1.y() * t,
            v1.z() * t
        );
    };
    inline vec3 operator/ (const vec3& v1, const double& t) {
        double f = 1.0 / t;
        return vec3(
            v1.x() * f,
            v1.y() * f,
            v1.z() * f
        );
    };
    inline vec3 operator* (const double& t, const vec3& v1) {
        return v1 * t;
    };
    inline vec3 operator/ (const double& t, const vec3& v1) {
        return v1 / t;
    };

    inline vec3 reflect(const vec3& v, const vec3& n) {
        vec3 nn{ n }; nn.make_unit_vector();
        return v - 2.0 * dot(v, nn) * nn;
    }

    bool refract(const vec3& v, const vec3& n, double n_r, vec3& refracted) {
        //default v is pointing into surface
        vec3 uv = unit_vector(v) * -1.0;
        double dt = fmin(dot(uv, n), 1.0);
        double D = 1.0 - n_r * n_r * (1.0 - dt * dt);
        if (D > 0.0) {
            refracted = n_r * (n * dt - uv) - n * sqrt(D);
            return true;
        }
        else return false;
    }
    //alias name
    using point3 = vec3;
    using color = vec3;
    inline double area2(const vec3& p, const vec3& q, const vec3& s) {
       
        return p.x() * q.y() - p.y() * q.x()
               + q.x() * s.y() - q.y() * s.x()
               + s.x() * p.y() - s.y() * p.x();
    }
    inline  bool to_left(const vec3& p, const vec3& q, const vec3& s) {
     
        return area2(p, q, s) > 0.0;
    }
    inline bool in_triangle(const vec3* tri, const vec3& s) {
        bool t1 = to_left( tri[0],tri[1], s);
        bool t2 = to_left(tri[1],tri[2] , s);
        bool t3 = to_left( tri[2],tri[0] ,s);
        return t1 == t2 && t2 == t3;
    }
}


#endif
