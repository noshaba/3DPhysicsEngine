#include "Physics.hpp"
#include <assert.h> 

Physics::Physics(void){
	this->frozen = false;
	this->dt = 1/100.0;
	this->gravity = Vec3(0,0,0);
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
	
	// float v = n*(v_1+(w_1%r_1)-v_2-(w_2%r_2));
	// return -2*v/(m_1+m_2+n*(((I_1*(r_1%n))%r_1)+((I_2*(r_2%n))%r_2)));
	float v = n*(v_1-v_2);
	return -2*v/(m_1+m_2);
}
float Physics::relative_momentum_wall(Object* obj, Vec3 n){
	return -2*(n*obj->lin_velocity)*obj->mass;
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
				__r = -__spheres[i]->radius*__n;
				__spheres[i]->update_velocities(__n,__r,this->relative_momentum_wall(__spheres[i],__n));
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::sphere2plane(__spheres[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __spheres[i]->mass_center;
					__P = this->relative_momentum_wall(__spheres[i],__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__r,__P);
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
					__r_2 = __colli.point - __spheres[j]->mass_center;
					__P = this->relative_momentum(__spheres[i],__spheres[j],__r_1,__r_2,__colli.normal);
					// __spheres[i]->lin_velocity.Print("sph1");
					// __spheres[j]->lin_velocity.Print("sph2");
					__spheres[i]->update_velocities(__colli.normal,__r_1,__P);
					__spheres[j]->update_velocities(__colli.normal,__r_2,-__P);
					// __colli.point.Print("C");
					// __spheres[i]->lin_velocity.Print("sph1");
					// __spheres[j]->lin_velocity.Print("sph2");
					// this->frozen = true;
					// return;
				}
			}
			
			///////////////////////////////
			// Collision against cuboids //
			///////////////////////////////
			
			for(unsigned int j = 0; j < __cuboids.size(); ++j){
				__colli = Collision::sphere2obb(__spheres[i],__cuboids[j]);
				if(__colli.collision){
					__r_1 = __colli.point - __spheres[i]->mass_center;
					__r_2 = __colli.point - __cuboids[j]->mass_center;
					__P = this->relative_momentum(__spheres[i],__cuboids[j],__r_1,__r_2,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__r_1,__P);
					__cuboids[j]->update_velocities(__colli.normal,__r_2,-__P);
					// __r_1.Print("r1");
					// __r_2.Print("r2");
					// __colli.point.Print("C");
					// __colli.normal.Print("N");
					// std::cout << __P << std::endl;
					// this->frozen = true;
					// return;
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
				__r = Null3;
				for(int j = 0; j < VEC_DIM; ++j){
					__r.p[j] = __cuboids[i]->hl.p[j]*(__n*__cuboids[i]->axis_orientation[j])-__cuboids[i]->mass_center.p[j];
				}
				__P = this->relative_momentum_wall(__cuboids[i],__n);
				__cuboids[i]->update_velocities(__n,__r,__P);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::obb2plane(__cuboids[i],__cage->planes[j]);
				if(__colli.collision){
					__r = __colli.point - __cuboids[i]->mass_center;
					__P = this->relative_momentum_wall(__cuboids[i],__colli.normal);
					__cuboids[i]->update_velocities(__colli.normal,__r,__P);
				}
			}
		}
	}
}