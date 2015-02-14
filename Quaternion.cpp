#include "Quaternion.hpp"

Quaternion::Quaternion(const Vec3 &normal, float theta){
	//convert degrees to radians divided by 2
	theta = theta/360*M_PI;
	this->w = cos(theta);
	this->axis = normal*sin(theta);
}
const Quaternion Quaternion::operator*(const Quaternion& q) const{
	Quaternion r;
	Vec3 axs = this->axis;
	r.w = this->w*q.w - (axs *= q.axis);
	r.axis = (this->axis*q.w) + (q.axis*w) + (this->axis%q.axis);
	return r;
}
const Vec3 Quaternion::operator*(const Vec3& p) const{
	return p + (this->axis%p)*(2*(this->w)) + (this->axis%(this->axis%p))*2;
}