#ifndef _COLLISION_HPP
#define _COLLISION_HPP

#include "ValueObjects.hpp"

class Collision{
	public:
		struct Collision_Info{
			bool collision;
			float distance;
			float overlap;
			Vec3 normal;
		};
		static std::vector<Vec3> manifold;
		static void draw_manifold(void);
		static float distance_plane2point(Plane* plane,Vec3 point);
		static Plane* get_collided_wall(Object* obj, std::vector<Plane*> walls);
		static Vec3 closest_point_on_OBB(Vec3 p, Cuboid* cub);
		static bool outside_scene(Object* obj, std::vector<Plane*> walls);
		static float radius_along_normal(Object* obj, Vec3 normal);
		static void pull_to_closest_wall(Object* obj, std::vector<Plane*> walls);
		// static bool sphere2aabb(Sphere* sph, Cuboid* cub);
		static Collision_Info sphere2obb(Sphere* sph, Cuboid* cub);
		static Collision_Info sphere2plane(Sphere* sph, Plane* plane);
		static Collision_Info sphere2sphere(Sphere* sph1, Sphere* sph2);
		static Collision_Info obb2plane(Cuboid* cub, Plane* plane);
};

#endif