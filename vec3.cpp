////////////////////////////////////////////////////////////////////
// FILE:        vec3.cpp
//
// AUTHOR:      Martin Bertram
//
// DESCRIPTION: a vector class to represent 3-D points and vectors
////////////////////////////////////////////////////////////////////

#include "vec3.hpp"

Vec3::Vec3() {}

// sets the vector
Vec3::Vec3(const float* x) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] = x[i];
}

Vec3::Vec3(FILE *fp) {
  for(int i = 0; i < VEC_DIM; i++)
    fscanf(fp, "%lf,", &p[i]);
}

Vec3::Vec3(const Vec3 &a) {
  p[0] = a.p[0];
  p[1] = a.p[1];
  p[2] = a.p[2];
}

Vec3::Vec3(float x, float y, float z) {
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

Vec3::~Vec3(){}

Vec3& Vec3::operator=(const Vec3& a) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] = a.p[i];
  return *this;
}

Vec3& Vec3::operator+=(const Vec3& a) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] += a.p[i];
  return *this;
}

Vec3& Vec3::operator-=(const Vec3& a) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] -= a.p[i];
  return *this;
}

Vec3& Vec3::operator*=(float sc) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] *= sc;
  return *this;
}

float Vec3::operator*=(const Vec3& b) {
  float r = 0;

  for(int i = 0; i < VEC_DIM; i++)
    r += p[i] * b.p[i];
  return r;
}

Vec3& Vec3::operator/=(float sc) {
  for(int i = 0; i < VEC_DIM; i++)
    p[i] /= sc;
  return *this;
}

float Vec3::Length() const {
  float r = 0;

  for(int i = 0; i < VEC_DIM; i++)
    r += p[i] * p[i];
  return sqrt(r);
}

float Vec3::Length2() const {
  float r = 0;

  for(int i = 0; i < VEC_DIM; i++)
    r += p[i] * p[i];
  return r;
}

void Vec3::Normalize() {
  float l = 1.0 / Length();

  for(int i = 0; i < VEC_DIM; i++)
    p[i] *= l;
}

float Vec3::LengthXY() const {
  return sqrt(p[0] * p[0] + p[1] * p[1]);
}

float Vec3::LengthXY2() const {
  return p[0] * p[0] + p[1] * p[1];
}

float Vec3::DotXY(const Vec3& a) const {
  return p[0] * a.p[0] + p[1] * a.p[1];
}

float Vec3::CrossXY(const Vec3& a) const {
  return p[0] * a.p[1] - p[1] * a.p[0];
}

void Vec3::MinMaxExpand(Vec3& min, Vec3& max) const {
  for(int i = 0; i < VEC_DIM; i++) {
    if(p[i] < min.p[i])
      min.p[i] = p[i];
    if(p[i] > max.p[i])
      max.p[i] = p[i];
  }
}

void Vec3::Print(char *c = NULL) const {
  if(!c)
    strcpy(c, "Vec3");

  printf("%s = (", c);
  for(int i = 0; i < VEC_DIM; i++)
    printf(" %12.9lf", p[i]);
  printf(")\n");
}

void Vec3::FScan(FILE* fp) {
  for(int i = 0; i < VEC_DIM; i++)
    fscanf(fp, "%lf", &p[i]);
}

void Vec3::FPrint(FILE* fp) const {
  for(int i = 0; i < VEC_DIM; i++)
    fprintf(fp, " %12.9lf", p[i]);
}
