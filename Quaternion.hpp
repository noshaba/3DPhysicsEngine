#ifndef _QUATERNION_HPP
#define _QUATERNION_HPP

	#include "vec3.hpp"

	class Quaternion{
		public:
			Quaternion(const Vec3 &normal, float theta);
			const Quaternion operator*(const Quaternion& q) const;
			const Vec3 operator*(const Vec3& p) const;
		private:
			Quaternion(){};
			float w;
			Vec3 axis;
	};
	
#endif