#include <float.h>

#include "Collision.hpp"

float Collision::distance_plane2point(Plane* plane,Vec3 point){
	return fabs((point - *(plane->vertex_buffer[0]))*plane->normal); 
}
Plane* Collision::get_collided_wall(Object* obj, std::vector<Plane*> walls){
	Plane* collided_wall;
	float hit_time;
	float shortest_distance = FLT_MAX;
	for(unsigned int i = 0; i < walls.size(); ++i){
		hit_time = (obj->mass_center-*(walls[i]->vertex_buffer[0]))*walls[i]->normal/(obj->lin_velocity_n*walls[i]->normal);
		if(!hit_time || isnan(hit_time)){
			return walls[i];
		}
		if(hit_time < 0) continue;
		if(hit_time < shortest_distance){
			shortest_distance = hit_time;
			collided_wall = walls[i];
		}
	}
	return collided_wall;
}
Vec3 Collision::closest_point_on_OBB(Vec3 p, Cuboid* cub){
	Vec3 distance_vec = p - cub->mass_center;
	Vec3 closest = cub->mass_center;
	float distance;
	for(unsigned int i = 0; i < VEC_DIM; ++i){
		distance = distance_vec * cub->axis_orientation[i];
		if(distance >  cub->hl.p[i]) distance =  cub->hl.p[i];
		if(distance < -cub->hl.p[i]) distance = -cub->hl.p[i];
		closest += distance * cub->axis_orientation[i];
	}
	return closest;
}
bool Collision::outside_scene(Object* obj, std::vector<Plane*> walls){
	for(unsigned int i = 0; i < walls.size(); ++i)
		if((obj->mass_center - *(walls[i]->vertex_buffer[0]))*walls[i]->normal < 0)
			return true;
	return false;
}
Collision::Collision_Info Collision::sphere2plane(Sphere* sph, Plane* plane){
	Collision_Info colli;
	colli.distance = distance_plane2point(plane,sph->mass_center);
	colli.overlap = sph->radius - colli.distance;
	colli.normal = plane->normal;
	colli.collision = colli.distance <= sph->radius;
	if(colli.collision){
		sph->pull(colli.normal,colli.overlap);
		colli.point = sph->mass_center + colli.normal*sph->radius;
	}
	return colli;
}
Collision::Collision_Info Collision::sphere2sphere(Sphere* sph1, Sphere* sph2){
	Collision_Info colli;
	colli.normal = sph1->mass_center - sph2->mass_center;
	colli.distance = colli.normal.Length();
	if(colli.distance) colli.normal /= colli.distance;
	colli.overlap = sph1->radius+sph2->radius-colli.distance;
	colli.collision = colli.distance <= sph1->radius + sph2->radius;
	if(colli.collision){
		sph1->pull(colli.normal,colli.overlap*.5);
		sph2->pull(-colli.normal,colli.overlap*.5);
		colli.point = sph1->mass_center - colli.normal*sph1->radius;
	}
	return colli;
}
Collision::Collision_Info Collision::sphere2obb(Sphere* sph, Cuboid* cub){
	Collision_Info colli;
	Vec3 closest = Collision::closest_point_on_OBB(sph->mass_center,cub);
	colli.normal = sph->mass_center - closest;
	colli.distance = colli.normal.Length();
	if(colli.distance) colli.normal /= colli.distance;
	colli.overlap = sph->radius - colli.distance;
	colli.collision = colli.distance <= sph->radius;
	if(colli.collision){
		sph->pull(colli.normal,colli.overlap*.5);
		cub->pull(-colli.normal,colli.overlap*.5);
		colli.point = closest;
	}
	return colli;
}
Collision::Collision_Info Collision::obb2plane(Cuboid* cub, Plane* plane){
	Collision_Info colli;
	colli.normal = plane->normal;
	colli.distance = distance_plane2point(plane,cub->mass_center);
	colli.point = Null3;
	for(int i = 0; i < VEC_DIM; ++i){
		colli.point.p[i] = cub->hl.p[i]*(colli.normal*cub->axis_orientation[i]);
	}
	float r = colli.point.Length();
	colli.overlap = r - colli.distance;
	colli.collision = colli.distance <= r;
	if(colli.collision)
		cub->pull(colli.normal,colli.overlap);
	return colli;
}
// bool Collision::sphere2aabb(Sphere* sph, Cuboid* cub){
	// float distance = 0;
	// for(unsigned int i = 0; i < VEC_DIM; ++i){
		// if(sph->mass_center.p[i] < cub->pmin.p[i])
			// distance += (sph->mass_center.p[i]-cub->pmin.p[i])*(sph->mass_center.p[i]-cub->pmin.p[i]);
		// else if(sph->mass_center.p[i] > cub->pmax.p[i])
			// distance += (sph->mass_center.p[i]-cub->pmax.p[i])*(sph->mass_center.p[i]-cub->pmax.p[i]);
	// }
	// return distance <= sph->radius*sph->radius;
// }