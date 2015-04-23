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
			Vec3 point;
			Vec3 r_1;
			Vec3 r_2;
		};
		static std::vector<Vec3> manifold;
		static void draw_manifold(void);
		static float distance_plane2point(Plane* plane,Vec3 point);
		static Plane* get_collided_wall(Object* obj, std::vector<Plane*> walls);
		static Vec3 closest_point_on_poly(Vec3 p, Polyhedron* poly);
		static bool outside_scene(Object* obj, std::vector<Plane*> walls);
		static float radius_along_normal(Object* obj, Vec3 n);
		static void projection_along_normal(Polyhedron* poly, Vec3 n, float (&projection)[2]);
		static void pull_to_closest_wall(Object* obj, std::vector<Plane*> walls);
		static Vec3 poly2axis_contact_point(Polyhedron* poly, Vec3 n);
		static Collision_Info sphere2poly(Sphere* sph, Polyhedron* poly);
		static Collision_Info sphere2plane(Sphere* sph, Plane* plane);
		static Collision_Info sphere2sphere(Sphere* sph1, Sphere* sph2);
		static Collision_Info poly2plane(Polyhedron* poly, Plane* plane);
		static Collision_Info poly2sepAxis(Polyhedron* poly1, Polyhedron* poly2, Vec3 n, Vec3 T);
		static Collision_Info poly2poly(Polyhedron* poly1, Polyhedron* poly2);
};

#endif