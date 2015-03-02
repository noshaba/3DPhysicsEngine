#ifndef _QUATERNION_HPP
#define _QUATERNION_HPP

	#include "vec3.hpp"
	
	#include <vector>

	class Quaternion{
		public:
			Quaternion(const Vec3 &normal, float theta);
			Quaternion(const Vec3 &ang_v);
			Quaternion(float w, const Vec3 &axis);
			const Quaternion operator*(const Quaternion& q) const;
			const Quaternion operator*(const float &s) const;
			const Vec3 operator*(const Vec3& p) const;
			const Quaternion operator+(const Quaternion &q) const;
			const std::vector<std::vector<float> > matrix() const;
			const float length();
			void normalize();
			float w;
			Vec3 axis;
		private:
			Quaternion(){};
	};
	
#endif