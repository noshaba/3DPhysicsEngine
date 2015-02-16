#include "Physics.hpp"

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
	float m_1 = obj1->mass;
	float m_2 = obj2->mass;
	std::vector<std::vector<float> > I_1 = obj1->inertia_tensor;
	std::vector<std::vector<float> > I_2 = obj2->inertia_tensor;
	
	float v = n*(v_1+(w_1%r_1)-v_2-(w_2%r_2));
	return -2*v/(1/m_1+1/m_2+n*(((I_1|(r_1%n))%r_1)+((I_2|(r_2%n))%r_2)));
}
float Physics::relative_momentum_wall(Object* obj, Vec3 n){
	return -2*(n*obj->lin_velocity)*obj->mass;
}
void Physics::update(void){
	if(!this->frozen){
	
		////////////////////////////////////////////////////// Spheres //////////////////////////////////////////////////////
	
		for(unsigned int i = 0; i < __spheres.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			__drag = -(__spheres[i]->drag_coeff/__spheres[i]->mass*__spheres[i]->lin_velocity.Length2())*__spheres[i]->tangent;

			__acceleration = this->gravity+__drag;
			__spheres[i]->move(this->dt,__acceleration);
			
			//////////////////////////////////////////////////////////////////////
			// If a sphere is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__spheres[i],__cage->planes)){
				__spheres[i]->old_mass_center();
				__n = Collision::get_collided_wall(__spheres[i],__cage->planes)->normal;
				__spheres[i]->update_lin_velocity(__n,this->relative_momentum_wall(__spheres[i],__n));
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::sphere2plane(__spheres[i],__cage->planes[j]);
				if(__colli.collision){
					__spheres[i]->pull(__colli.normal,__colli.overlap);
					__spheres[i]->update_lin_velocity(__colli.normal,this->relative_momentum_wall(__spheres[i],__colli.normal));
				}
			}
			
			/////////////////////////////////////
			// Collision against other spheres //
			/////////////////////////////////////
			
			for(unsigned int j = 0; j < __spheres.size(); ++j){
				if(i==j) continue;
				__colli = Collision::sphere2sphere(__spheres[i],__spheres[j]);
				if(__colli.collision){
					__spheres[i]->pull(__colli.normal,__colli.overlap*.5);
					__spheres[j]->pull(-__colli.normal,__colli.overlap*.5);
					__r_1 = .5*(__spheres[j]->mass_center - __spheres[i]->mass_center);
					__r_2 = -__r_1;
					__P = this->relative_momentum(__spheres[i],__spheres[j],__r_1,__r_2,__colli.normal);
					__spheres[i]->update_lin_velocity(__colli.normal,__P);
					__spheres[j]->update_lin_velocity(-__colli.normal,__P);
				}
			}
			
			///////////////////////////////
			// Collision against cuboids //
			///////////////////////////////
			
			for(unsigned int j = 0; j < __cuboids.size(); ++j){
				__colli = Collision::sphere2obb(__spheres[i],__cuboids[j]);
				if(__colli.collision){
					__spheres[i]->pull(__colli.normal,__colli.overlap*.5);
					__cuboids[j]->pull(-__colli.normal,__colli.overlap*.5);
					__spheres[i]->update_lin_velocity(__colli.normal,this->relative_momentum_wall(__spheres[i],__colli.normal));
				}
			}
		}
		
		////////////////////////////////////////////////////// Cuboids //////////////////////////////////////////////////////
		
		for(unsigned int i = 0; i < __cuboids.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			__drag = -(__cuboids[i]->drag_coeff/__cuboids[i]->mass*__cuboids[i]->lin_velocity.Length2())*__cuboids[i]->tangent;

			__acceleration = this->gravity+__drag;
			__cuboids[i]->move(this->dt,__acceleration);
			
			//////////////////////////////////////////////////////////////////////
			// If a cuboid is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__cuboids[i],__cage->planes)){
				__cuboids[i]->old_mass_center();
				__n = Collision::get_collided_wall(__cuboids[i],__cage->planes)->normal;
				__cuboids[i]->update_lin_velocity(__n,this->relative_momentum_wall(__cuboids[i],__n));
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::obb2plane(__cuboids[i],__cage->planes[j]);
				if(__colli.collision){
					__cuboids[i]->pull(__colli.normal,__colli.overlap);
					__cuboids[i]->update_lin_velocity(__colli.normal,this->relative_momentum_wall(__cuboids[i],__colli.normal));
				}
			}
		}
	}
}