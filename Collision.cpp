#include <float.h>

#include "Collision.hpp"

std::vector<Vec3> Collision::manifold;
void Collision::draw_manifold(void){
	Render_Object::material_color(GL_FRONT_AND_BACK,Vec3(1,1,0));
	glPointSize(10);
	glBegin(GL_POINTS);
	glNormal3f(0,0,1);
	for(unsigned int i = 0; i < Collision::manifold.size(); ++i)
		glVertex3fv(Collision::manifold[i].p);
	glEnd();
}
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
	for(unsigned int i = 0; i < cub->hl.size(); ++i){
		distance = distance_vec * cub->axis_orientation[i];
		if(distance >  cub->hl[i]) distance =  cub->hl[i];
		if(distance < -cub->hl[i]) distance = -cub->hl[i];
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
float Collision::radius_along_normal(Object* obj, Vec3 normal){
	if(Sphere* sph = dynamic_cast<Sphere*>(obj))
		return sph->radius;
	if(Cuboid* cub = dynamic_cast<Cuboid*>(obj)){
		Vec3 r = Null3;
		for(int i = 0; i < cub->hl.size(); ++i){
			r.p[i] = cub->hl[i]*(normal*cub->axis_orientation[i]);
		}
		return r.Length();
	}
}
void Collision::pull_to_closest_wall(Object* obj, std::vector<Plane*> walls){
	Plane* plane = Collision::get_collided_wall(obj,walls);
	Collision_Info colli;
	colli.normal = plane->normal;
	colli.distance = distance_plane2point(plane,obj->mass_center);
	float r = radius_along_normal(obj,colli.normal);
	colli.overlap = r - colli.distance;
	obj->pull(colli.normal,colli.overlap);
}
Collision::Collision_Info Collision::sphere2plane(Sphere* sph, Plane* plane){
	sph->manifold.clear();
	Collision_Info colli;
	colli.distance = distance_plane2point(plane,sph->mass_center);
	colli.overlap = sph->radius - colli.distance;
	colli.normal = plane->normal;
	colli.collision = colli.distance <= sph->radius;
	if(colli.collision){
		sph->pull(colli.normal,colli.overlap);
		colli.point = sph->mass_center - colli.normal*sph->radius;
		sph->manifold.push_back(colli.point);
		Collision::manifold.push_back(colli.point);
	}
	return colli;
}
Collision::Collision_Info Collision::sphere2sphere(Sphere* sph1, Sphere* sph2){
	sph1->manifold.clear();
	sph2->manifold.clear();
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
		sph1->manifold.push_back(colli.point);
		sph2->manifold.push_back(colli.point);
		Collision::manifold.push_back(colli.point);
	}
	return colli;
}
Collision::Collision_Info Collision::sphere2obb(Sphere* sph, Cuboid* cub){
	sph->manifold.clear();
	cub->manifold.clear();
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
		sph->manifold.push_back(closest);
		cub->manifold.push_back(closest);
		Collision::manifold.push_back(closest);
	}
	return colli;
}
Collision::Collision_Info Collision::obb2plane(Cuboid* cub, Plane* plane){
	cub->manifold.clear();
	Collision_Info colli;
	colli.normal = plane->normal;
	colli.distance = distance_plane2point(plane,cub->mass_center);
	Vec3 rv = Null3;
	for(int i = 0; i < cub->hl.size(); ++i){
		rv.p[i] = cub->hl[i]*(colli.normal*cub->axis_orientation[i]);
	}
	float r = rv.Length();
	colli.overlap = r - colli.distance;
	colli.collision = colli.distance <= r;
	if(colli.collision){
		cub->pull(colli.normal,colli.overlap);
		float min = FLT_MAX;
		float projection;
		for(unsigned int i = 0; i < cub->vertex_buffer.size(); ++i){
			projection = *(cub->vertex_buffer[i])*colli.normal;
			if(projection < min){
				cub->manifold.clear();
				min = projection;
				cub->manifold.push_back(*(cub->vertex_buffer[i]));
				colli.point = *(cub->vertex_buffer[i]);
			} else if(projection == min){
				cub->manifold.push_back(*(cub->vertex_buffer[i]));
				colli.point += *(cub->vertex_buffer[i]);
			}
		}
		colli.point /= (float) cub->manifold.size();
		Collision::manifold.insert(Collision::manifold.end(),cub->manifold.begin(),cub->manifold.end());
	}
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