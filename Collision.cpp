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
Vec3 Collision::closest_point_on_poly(Vec3 p, Polyhedron* poly){
	Vec3 distance_vec = p - poly->mass_center;
	Vec3 closest = poly->mass_center;
	float distance;
	for(unsigned int i = 0; i < poly->hl.size(); ++i){
		distance = distance_vec * poly->axis_orientation[i];
		if(distance >  poly->hl[i]) distance =  poly->hl[i];
		if(distance < -poly->hl[i]) distance = -poly->hl[i];
		closest += distance * poly->axis_orientation[i];
	}
	return closest;
}
bool Collision::outside_scene(Object* obj, std::vector<Plane*> walls){
	for(unsigned int i = 0; i < walls.size(); ++i)
		if((obj->mass_center - *(walls[i]->vertex_buffer[0]))*walls[i]->normal < 0)
			return true;
	return false;
}
float Collision::radius_along_normal(Object* obj, Vec3 n){
	if(Sphere* sph = dynamic_cast<Sphere*>(obj))
		return sph->radius;
	if(Polyhedron* poly = dynamic_cast<Polyhedron*>(obj)){
		float r = 0;
		for(int i = 0; i < poly->hl.size(); ++i){
			r += fabs(poly->hl[i]*poly->axis_orientation[i]*n);
		}
		return r;
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
		colli.r_1 = colli.point - sph->mass_center;
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
		colli.r_1 = colli.point - sph1->mass_center;
		colli.r_2 = colli.point - sph2->mass_center;
	}
	return colli;
}
Collision::Collision_Info Collision::sphere2poly(Sphere* sph, Polyhedron* poly){
	sph->manifold.clear();
	poly->manifold.clear();
	Collision_Info colli;
	Vec3 closest = Collision::closest_point_on_poly(sph->mass_center,poly);
	colli.normal = sph->mass_center - closest;
	colli.distance = colli.normal.Length();
	if(colli.distance) colli.normal /= colli.distance;
	colli.overlap = sph->radius - colli.distance;
	colli.collision = colli.distance <= sph->radius;
	if(colli.collision){
		sph->pull(colli.normal,colli.overlap*.5);
		poly->pull(-colli.normal,colli.overlap*.5);
		colli.point = closest;
		sph->manifold.push_back(closest);
		poly->manifold.push_back(closest);
		Collision::manifold.push_back(closest);
		colli.r_1 = colli.point - sph->mass_center;
		colli.r_2 = colli.point - poly->mass_center;
	}
	return colli;
}
Vec3 Collision::poly2axis_contact_point(Polyhedron* poly, Vec3 n){
	Vec3 contact_point;
	float min = FLT_MAX;
	float projection;
	for(unsigned int i = 0; i < poly->vertex_buffer.size(); ++i){
	projection = *(poly->vertex_buffer[i])*n;
		if(projection < min){
			poly->manifold.clear();
			min = projection;
			poly->manifold.push_back(*(poly->vertex_buffer[i]));
			contact_point = *(poly->vertex_buffer[i]);
		} else if(projection == min){
			poly->manifold.push_back(*(poly->vertex_buffer[i]));
			contact_point += *(poly->vertex_buffer[i]);
		}
	}
	contact_point /= (float) poly->manifold.size();
	Collision::manifold.insert(Collision::manifold.end(),poly->manifold.begin(),poly->manifold.end());
	return contact_point;
}
Collision::Collision_Info Collision::poly2plane(Polyhedron* poly, Plane* plane){
	poly->manifold.clear();
	Collision_Info colli;
	colli.normal = plane->normal;
	colli.distance = distance_plane2point(plane,poly->mass_center);
	float r = radius_along_normal(poly, plane->normal);
	colli.overlap = r - colli.distance;
	colli.collision = colli.distance <= r;
	if(colli.collision){
		poly->pull(colli.normal,colli.overlap);
		colli.point = poly2axis_contact_point(poly, colli.normal);
		colli.r_1 = colli.point - poly->mass_center;
	}
	return colli;
}
Collision::Collision_Info Collision::poly2sepAxis(Polyhedron* poly1, Polyhedron* poly2, Vec3 n, Vec3 T){
	Collision_Info colli;
	colli.distance = T*n;
	if(colli.distance > 0) n = -n;
	colli.normal = n;
	colli.distance = fabs(colli.distance);
	float r = 0;
	r += radius_along_normal(poly1,n);
	r += radius_along_normal(poly2,n);
	colli.overlap = r - colli.distance;
	colli.collision = colli.distance <= r;
	return colli;
}
Collision::Collision_Info Collision::poly2poly(Polyhedron* poly1, Polyhedron* poly2){
	poly1->manifold.clear();
	poly2->manifold.clear();
	Collision_Info colli;
	colli.overlap = FLT_MAX;
	Vec3 T = poly2->mass_center - poly1->mass_center;
	Collision_Info c;
	for(unsigned int i = 0; i < poly1->axis_orientation.size(); ++i){
		c = poly2sepAxis(poly1,poly2,poly1->axis_orientation[i],T);
		if(!c.collision){
			colli.collision = false;
			return colli;
		}
		if(c.overlap < colli.overlap)
			colli = c;
	}
	for(unsigned int i = 0; i < poly2->axis_orientation.size(); ++i){
		c = poly2sepAxis(poly1,poly2,poly2->axis_orientation[i],T);
		if(!c.collision){
			colli.collision = false;
			return colli;
		}
		if(c.overlap < colli.overlap)
			colli = c;
	}
	Vec3 n;
	for(unsigned int i = 0; i < poly1->edge_orientation.size(); ++i){
		for(unsigned int j = 0; j < poly2->edge_orientation.size(); ++j){
			n = poly1->edge_orientation[i]%poly2->edge_orientation[j];
			if(n.Length2() != 1) continue;
			c = poly2sepAxis(poly1,poly2,n,T);
			if(!c.collision){
				colli.collision = false;
				return colli;
			}
			if(c.overlap < colli.overlap)
				colli = c;
		}
	}
	if(colli.collision){
		poly1->pull(colli.normal,colli.overlap*.5);
		poly2->pull(-colli.normal,colli.overlap*.5);
		colli.r_1 = poly2axis_contact_point(poly1, colli.normal) - poly1->mass_center;
		colli.r_2 = poly2axis_contact_point(poly2,-colli.normal) - poly2->mass_center;
	}
	return colli;
}