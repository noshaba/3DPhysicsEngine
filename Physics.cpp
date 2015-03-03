#include "Physics.hpp"
#include <assert.h> 

Physics::Physics(void){
	this->frozen = false;
	this->dt = 1/100.0;
	this->gravity = Vec3(0,-9.81,0);
}
Physics::~Physics(void){}
void Physics::add_cage(Cage* cage){
	__cage = cage;
}
void Physics::add_sphere(Sphere* sphere){
	__spheres.push_back(sphere);
}
void Physics::add_cuboid(Cuboid* cuboid){
	__cuboids.push_back(cuboid);
}
float Physics::relative_momentum(Object* obj1, Object* obj2, Vec3 r_1, Vec3 r_2, Vec3 n){
	Vec3 v_1 = obj1->lin_velocity;
	Vec3 v_2 = obj2->lin_velocity;
	Vec3 w_1 = obj1->ang_velocity;
	Vec3 w_2 = obj2->ang_velocity;
	float m_1 = obj1->inverse_mass;
	float m_2 = obj2->inverse_mass;
	std::vector<std::vector<float> > I_1 = obj1->inv_inertia_tensor;
	std::vector<std::vector<float> > I_2 = obj2->inv_inertia_tensor;
	
	float rv = n*(v_1+(w_1%r_1)-v_2-(w_2%r_2));
	return -2*rv/(m_1+m_2+n*(((I_1*(r_1%n))%r_1)+((I_2*(r_2%n))%r_2)));
	// float v = n*(v_1-v_2);
	// return -2*v/(m_1+m_2);
}
float Physics::relative_momentum(Object* obj, Vec3 r, Vec3 n){
	Vec3 v = obj->lin_velocity;
	Vec3 w = obj->ang_velocity;
	float m = obj->inverse_mass;
	std::vector<std::vector<float> > I = obj->inv_inertia_tensor;
	
	float rv = n*(v+(w%r));
	return -2*rv/(m+n*(((I*(r%n))%r)));
}
void Physics::draw_collision_point(void){
	Render_Object::material_color(GL_FRONT_AND_BACK,Vec3(1,0,0));
	glPointSize(10);
	glBegin(GL_POINTS);
	glNormal3f(0,0,1);
	glVertex3fv(__colli.point.p);
	glEnd();
}
void Physics::update(void){
	if(!this->frozen){
	
		////////////////////////////////////////////////////// Spheres //////////////////////////////////////////////////////
	
		for(unsigned int i = 0; i < __spheres.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			// __drag = -(__spheres[i]->drag_coeff*__spheres[i]->inverse_mass*__spheres[i]->lin_velocity.Length2())*__spheres[i]->lin_velocity_n;
			// __friction = -(__spheres[i]->drag_coeff*__spheres[i]->ang_velocity.Length2())*__spheres[i]->ang_velocity_n;
			
			__acceleration = this->gravity;
			
			// __force  = __spheres[i]->mass*this->gravity;
			// __torque = __spheres[i]->r%__force;
		
			// __ang_acceleration = __spheres[i]->inv_inertia_tensor*(__torque);
			__spheres[i]->integrate(this->dt,__acceleration,Null3);
			
			//////////////////////////////////////////////////////////////////////
			// If a sphere is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__spheres[i],__cage->planes)){
				__spheres[i]->old_mass_center();
				Collision::pull_to_closest_wall(__spheres[i],__cage->planes);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::sphere2plane(__spheres[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __spheres[i]->mass_center;
					__spheres[i]->r = __r;
					__P = this->relative_momentum(__spheres[i],__r,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__r,__P);
					__colli.point.Print("contact");
					this->frozen = true;
					return;
				}
			}
			
			/////////////////////////////////////
			// Collision against other spheres //
			/////////////////////////////////////
			
			for(unsigned int j = 0; j < __spheres.size(); ++j){
				if(i==j) continue;
				__colli = Collision::sphere2sphere(__spheres[i],__spheres[j]);
				if(__colli.collision){
					__r_1 = __colli.point - __spheres[i]->mass_center;
					__spheres[i]->r = __r_1;
					__r_2 = __colli.point - __spheres[j]->mass_center;
					__spheres[j]->r = __r_2;
					__P = this->relative_momentum(__spheres[i],__spheres[j],__r_1,__r_2,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__r_1,__P);
					__spheres[j]->update_velocities(__colli.normal,__r_2,-__P);
				}
			}
			
			///////////////////////////////
			// Collision against cuboids //
			///////////////////////////////
			
			for(unsigned int j = 0; j < __cuboids.size(); ++j){
				__colli = Collision::sphere2obb(__spheres[i],__cuboids[j]);
				if(__colli.collision){
					__r_1 = __colli.point - __spheres[i]->mass_center;
					__spheres[i]->r = __r_1;
					__r_2 = __colli.point - __cuboids[j]->mass_center;
					__cuboids[j]->r = __r_2;
					__P = this->relative_momentum(__spheres[i],__cuboids[j],__r_1,__r_2,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__r_1,__P);
					__cuboids[j]->update_velocities(__colli.normal,__r_2,-__P);
				}
			}
		}
		
		////////////////////////////////////////////////////// Cuboids //////////////////////////////////////////////////////
		
		for(unsigned int i = 0; i < __cuboids.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			// __drag = -(__cuboids[i]->drag_coeff*__cuboids[i]->inverse_mass*__cuboids[i]->lin_velocity.Length2())*__cuboids[i]->lin_velocity_n;
			// __friction = -(__cuboids[i]->drag_coeff*__cuboids[i]->ang_velocity.Length2())*__cuboids[i]->ang_velocity_n;

			__acceleration = this->gravity;
			
			// __force  = __cuboids[i]->mass*(this->gravity);
			// __torque = __cuboids[i]->r%__force;
			
			// __ang_acceleration = __cuboids[i]->inv_inertia_tensor*(__torque);
			
			__cuboids[i]->integrate(this->dt,__acceleration,Null3);
			
			//////////////////////////////////////////////////////////////////////
			// If a cuboid is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__cuboids[i],__cage->planes)){
				__cuboids[i]->old_mass_center();
				Collision::pull_to_closest_wall(__cuboids[i],__cage->planes);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::obb2plane(__cuboids[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __cuboids[i]->mass_center;
					__cuboids[i]->r = __r;
					__P = this->relative_momentum(__cuboids[i],__r,__colli.normal);
					__cuboids[i]->update_velocities(__colli.normal,__r,__P);
					__colli.point.Print("contact");
					this->frozen = true;
					return;
				}
			}
		}
	}
}