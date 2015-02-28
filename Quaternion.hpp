#ifndef _QUATERNION_HPP
#define _QUATERNION_HPP

	#include "vec3.hpp"
	
	#include <vector>

	class Quaternion{
		public:
			Quaternion(const Vec3 &normal, float theta);
			const Quaternion operator*(const Quaternion& q) const;
			const Quaternion operator*(const float &s) const;
			const Vec3 operator*(const Vec3& p) const;
			const Quaternion operator+(const Quaternion &q) const;
			const std::vector<std::vector<float> > matrix();
			const float length();
			void normalize();
		private:
			Quaternion(){};
			float w;
			Vec3 axis;
	};
	
#endif