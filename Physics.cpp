#include "Physics.hpp"
#include <assert.h> 
#include <algorithm>

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
void Physics::apply_momenti(Object* obj, Vec3 r, Vec3 n){
	Vec3 v = obj->lin_velocity;
	Vec3 w = obj->ang_velocity;
	float m = obj->inverse_mass;
	// float s_f = obj->static_friction;
	// float d_f = obj->dynamic_friction;
	float e   = std::min(obj->restitution,__cage->restitution);
	float s_f = sqrt(obj->static_friction*__cage->static_friction);
	float d_f = sqrt(obj->dynamic_friction*__cage->dynamic_friction);
	std::vector<std::vector<float> > I = obj->inv_inertia_tensor;

	Vec3  rv = v+(w%r);
	float im = m+n*(((I*(r%n))%r));
	
	/// Momentum along normal
	float rn = n*rv;
	float Pn = -(1+e)*rn/im;
	obj->update_velocities(n,r,Pn);
	
	/// Friction momentum
	// Vec3 t = rv - (rv*n)*n;
	// t.Normalize();
	// float Pt = -rv*t/im;
	// if(fabs(Pt) <= 0.0001f) return;
    // if(fabs(Pt) >= Pn*s_f){ 
		// Pt = -Pn*d_f; // Coulumb's law
		// std::cout << "hello" << std::endl;
	// }
	// std::cout << Pt << std::endl;
	// obj->update_velocities(t,r,Pt);
}
void Physics::apply_momenti(Object* obj1, Object* obj2, Vec3 r_1, Vec3 r_2, Vec3 n){
	Vec3 v_1 = obj1->lin_velocity;
	Vec3 v_2 = obj2->lin_velocity;
	Vec3 w_1 = obj1->ang_velocity;
	Vec3 w_2 = obj2->ang_velocity;
	float m_1 = obj1->inverse_mass;
	float m_2 = obj2->inverse_mass;
	float s_f = sqrt(obj1->static_friction*obj2->static_friction);
	float d_f = sqrt(obj1->dynamic_friction*obj2->dynamic_friction);
	float e   = std::min(obj1->restitution,obj2->restitution);
	std::vector<std::vector<float> > I_1 = obj1->inv_inertia_tensor;
	std::vector<std::vector<float> > I_2 = obj2->inv_inertia_tensor;
	// float v = n*(v_1-v_2);
	// return -2*v/(m_1+m_2);
	Vec3  rv = v_1+(w_1%r_1)-v_2-(w_2%r_2);
	float im = m_1+m_2+n*(((I_1*(r_1%n))%r_1)+((I_2*(r_2%n))%r_2));
	
	/// Momentum along normal
	float rn = n*rv;
	float Pn = -(1+e)*rn/im;
	obj1->update_velocities(n,r_1,Pn);
	obj2->update_velocities(n,r_2,-Pn);
	
	/// Friction momentum
	Vec3 t = rv - (rv*n)*n;
	t.Normalize();
	float Pt = -rv*t/im;
	if(Pt <= 0.0001f) return;
    if(Pt*Pt >= Pn*Pn*s_f*s_f) Pt = -Pn*d_f; // Coulumb's law
	obj1->update_velocities(t,r_1,Pt);
	obj2->update_velocities(t,r_2,-Pt);
}
void Physics::draw_collision_point(void){
	Render_Object::material_color(GL_FRONT_AND_BACK,Vec3(1,1,1));
	glPointSize(10);
	glBegin(GL_POINTS);
	glNormal3f(0,0,1);
	glVertex3fv(__colli.point.p);
	glEnd();
}
void Physics::update(void){
	if(!this->frozen){
	
		////////////////////////////////////////////////////// Spheres //////////////////////////////////////////////////////
	
		// __cuboids[0]->ang_velocity.Print("c");
		// __spheres[0]->ang_velocity.Print("s");
		// __cuboids[0]->r.Print("rad");
		// this->frozen = true;
		
		// std::cout << __spheres[0]->velocity + __cuboids[0]->velocity + __spheres[0]->omega + __cuboids[0]->omega << std::endl;
		// std::cout << __spheres[0]->velocity + __spheres[0]->omega << std::endl;
		 // std::cout << __cuboids[0]->velocity + __cuboids[0]->omega << std::endl;
		
		// this->frozen = true;
	
		for(unsigned int i = 0; i < __spheres.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			// __drag = -(__spheres[i]->drag_coeff*__spheres[i]->inverse_mass*__spheres[i]->lin_velocity.Length2())*__spheres[i]->lin_velocity_n;
			// __friction = -(__spheres[i]->drag_coeff*__spheres[i]->ang_velocity.Length2())*__spheres[i]->ang_velocity_n;
			
			// __spheres[i]->r.Print("radS");
			
			// __force  = __spheres[i]->mass*this->gravity;
			// __torque = __spheres[i]->r%__force;
			
			// __force -= 0.001*__spheres[i]->lin_velocity;
			// __torque -= 0.001*__spheres[i]->ang_velocity;
		
			__acceleration = __spheres[i]->inverse_mass*__force;
			// __ang_acceleration = __spheres[i]->inv_inertia_tensor*__torque;
			
			// __acceleration = __ang_acceleration = Null3;
			
			__spheres[i]->integrate(this->dt,__acceleration,__ang_acceleration);
			
			//////////////////////////////////////////////////////////////////////
			// If a sphere is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__spheres[i],__cage->planes)){
				__spheres[i]->old_mass_center();
				__n = Collision::get_collided_wall(__spheres[i],__cage->planes)->normal;
				__r = -__spheres[i]->radius*__n;
				__spheres[i]->r = __r;
				this->apply_momenti(__spheres[i],__r,__n);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::sphere2plane(__spheres[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __spheres[i]->mass_center;
					__spheres[i]->r = __r;
					this->apply_momenti(__spheres[i],__r,__colli.normal);
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
					this->apply_momenti(__spheres[i],__spheres[j],__r_1,__r_2,__colli.normal);
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
					this->apply_momenti(__spheres[i],__cuboids[j],__r_1,__r_2,__colli.normal);
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
			
			__force  = __cuboids[i]->mass*(this->gravity);
			__torque = __cuboids[i]->r%__force;
			
			__ang_acceleration = __cuboids[i]->inv_inertia_tensor*(__torque);
			
			// __acceleration = __ang_acceleration = Null3;
			
			__cuboids[i]->integrate(this->dt,__acceleration,__ang_acceleration);
			
			//////////////////////////////////////////////////////////////////////
			// If a cuboid is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__cuboids[i],__cage->planes)){
				__cuboids[i]->old_mass_center();
				__n = Collision::get_collided_wall(__cuboids[i],__cage->planes)->normal;
				__r = Null3;
				for(int j = 0; j < VEC_DIM; ++j){
					__r.p[j] = __cuboids[i]->hl.p[j]*(__n*__cuboids[i]->axis_orientation[j]);
				}
				__cuboids[i]->r = __r;
				this->apply_momenti(__cuboids[i],__r,__n);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::obb2plane(__cuboids[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __cuboids[i]->mass_center;
					__cuboids[i]->r = __r;
					this->apply_momenti(__cuboids[i],__r,__colli.normal);
				}
			}
		}
	}
}