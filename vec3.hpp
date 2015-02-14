////////////////////////////////////////////////////////////////////
// FILE:        vec3.hpp
//
// AUTHOR:      Martin Bertram
//
// MODIFIED BY: Noshaba Cheema
//
// DESCRIPTION:	a vector class to represent 3-D points and vectors
////////////////////////////////////////////////////////////////////

#ifndef _VEC3_HPP
#define _VEC3_HPP

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

// constants ///////////////////////////////////////////////////////

#define	VEC_DIM	3
#define VEC_INF 1.0e9
#define VEC_EPS 1.0e-9

#define Null3 Vec3(0.0, 0.0, 0.0)
#define XVec3 Vec3(1.0, 0.0, 0.0)
#define YVec3 Vec3(0.0, 1.0, 0.0)
#define ZVec3 Vec3(0.0, 0.0, 1.0)


// 3D points and vectors /////////////////////////////////////////////
class Vec3 {
public:
	float	p[VEC_DIM];

	Vec3();
	Vec3(const Vec3 &a);
	~Vec3();
	explicit Vec3(const float* x);
	explicit Vec3(FILE *fp);
	Vec3(float x, float y, float z);

	Vec3& operator=(const Vec3& a);

	// scalar-vector arithmetics //////////////////////////////
	Vec3& operator+=(const Vec3& a);
	inline friend Vec3 operator+(Vec3 a, const Vec3& b) {
		return a += b;
	}

	Vec3& operator-=(const Vec3& a);
	inline friend Vec3 operator-(Vec3 a, const Vec3& b) {
		return a -= b;
	}

	inline friend Vec3 operator-(Vec3 a) {
		Vec3 c;
		for( int i=0; i<VEC_DIM; i++)
			c.p[i] = -a.p[i];
		return c;
	}

	Vec3& operator*=(float sc);
	inline friend Vec3 operator*(float sc, Vec3 a) {
		return a *= sc;
	}

	inline friend Vec3 operator*(Vec3 a, float sc) {
		return a *= sc;
	}

	Vec3& operator/=(float sc);
	inline friend Vec3 operator/(Vec3 a, float sc) {
		return a /= sc;
	}
	
	inline bool operator==(const Vec3& v){
		return p[0] == v.p[0] && p[1] == v.p[1] && p[2] == v.p[2];
	}
	
	inline bool operator!=(const Vec3& v){
		return p[0] != v.p[0] || p[1] != v.p[1] || p[2] != v.p[2];
	}

	// dot product ////////////////////////////////////////////////
	float operator*=(const Vec3& b);
	inline friend float operator*(Vec3 a, const Vec3& b) {
		return a *= b;
	}

	// cross product //////////////////////////////////////////////
	inline friend Vec3 operator%(const Vec3& a, const Vec3& b) {
		Vec3 ret;

		ret.p[0] = a.p[1] * b.p[2] - a.p[2] * b.p[1];
		ret.p[1] = a.p[2] * b.p[0] - a.p[0] * b.p[2];
		ret.p[2] = a.p[0] * b.p[1] - a.p[1] * b.p[0];

		return ret;
	}

	float Length() const;
	float Length2() const;
	float LengthXY() const;
	float LengthXY2() const;
	float DotXY(const Vec3& a) const;
	float CrossXY(const Vec3& a) const;
	void MinMaxExpand(Vec3& min, Vec3& max) const;
	void Normalize();
	void Print(char *c) const;
	void FScan(FILE *fp);
	void FPrint(FILE *fp) const;
};
#endif
