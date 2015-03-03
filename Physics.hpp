#ifndef _PHYSICS_HPP
#define _PHYSICS_HPP

	#include "ValueObjects.hpp"
	#include "Collision.hpp"

	class Physics{
		private:
			float dt;
			Vec3 gravity, __force, __torque, __acceleration, __ang_acceleration, __drag, __friction, __n, __r,__r_1, __r_2;
			Collision::Collision_Info __colli;
			Cage* __cage;
			std::vector<Sphere*> __spheres;
			std::vector<Cuboid*> __cuboids;
			void apply_momenti(Object* obj, Vec3 r, Vec3 n);
			void apply_momenti(Object* obj1, Object* obj2, Vec3 r_1, Vec3 r_2, Vec3 n);
		public:
			bool frozen;
			Physics(void);
			~Physics(void);
			void add_cage(Cage* cage);
			void add_sphere(Sphere* sphere);
			void add_cuboid(Cuboid* cuboid);
			void update(void);
			void draw_collision_point(void);
	};

#endif