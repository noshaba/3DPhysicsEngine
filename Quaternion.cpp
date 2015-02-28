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
const Quaternion Quaternion::operator*(const float &s) const{
	Quaternion q;
	q.w = this->w*s;
	q.axis = this->axis*s;
	return q;
}
const Quaternion Quaternion::operator+(const Quaternion &q) const{
	Quaternion r;
	r.w = this->w + q.w;
	r.axis = this->axis + q.axis;
	return r;
}
const std::vector<std::vector<float> > Quaternion::matrix(){
	float fTx  = 2.0f*this->axis.p[0];
	float fTy  = 2.0f*this->axis.p[1];
	float fTz  = 2.0f*this->axis.p[2];
	float fTwx = fTx*this->w;
	float fTwy = fTy*this->w;
	float fTwz = fTz*this->w;
	float fTxx = fTx*this->axis.p[0];
	float fTxy = fTy*this->axis.p[0];
	float fTxz = fTz*this->axis.p[0];
	float fTyy = fTy*this->axis.p[1];
	float fTyz = fTz*this->axis.p[1];
	float fTzz = fTz*this->axis.p[2];
	
	return {{1.0f-(fTyy+fTzz), fTxy-fTwz, fTxz+fTwy},
			{fTxy+fTwz, 1.0f-(fTxx+fTzz), fTyz-fTwx},
			{fTxz-fTwy, fTyz+fTwx, 1.0f-(fTxx+fTyy)}};
}
const float Quaternion::length(){
	return sqrt(w*w+this->axis.Length2());
}
void Quaternion::normalize(){
	const float length = this->length();
	if (length == 0){
		this->w = 1;
		this->axis = Null3;
	} else {
		float inv = 1.0f / length;
		this-> w = w*inv;
		this->axis = axis*inv;
	}
}