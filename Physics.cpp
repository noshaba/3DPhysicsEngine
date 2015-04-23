#include "Physics.hpp"
#include <assert.h> 

Physics::Physics(void){
	this->frozen = false;
	this->dt = 1/120.0;
	this->gravity = Vec3(0,-9.81,0);
}
Physics::~Physics(void){}
void Physics::add_cage(Cage* cage){
	__cage = cage;
}
void Physics::add_sphere(Sphere* sphere){
	__spheres.push_back(sphere);
}
void Physics::add_polyhedron(Polyhedron* polyhedron){
	__polyhedra.push_back(polyhedron);
}
float Physics::relative_momentum(Object* obj1, Object* obj2, Vec3 r_1, Vec3 r_2, Vec3 n){
	Vec3 v_1 = obj1->lin_velocity;
	Vec3 v_2 = obj2->lin_velocity;
	Vec3 w_1 = obj1->ang_velocity;
	Vec3 w_2 = obj2->ang_velocity;
	float m_1 = obj1->inverse_mass;
	float m_2 = obj2->inverse_mass;
	Matrix<float> I_1 = obj1->inv_inertia_tensor;
	Matrix<float> I_2 = obj2->inv_inertia_tensor;
	
	float rv = n*(v_1+(w_1%r_1)-v_2-(w_2%r_2));
	return -2*rv/(m_1+m_2+n*(((I_1*(r_1%n))%r_1)+((I_2*(r_2%n))%r_2)));
	// float v = n*(v_1-v_2);
	// return -2*v/(m_1+m_2);
}
float Physics::relative_momentum(Object* obj, Vec3 r, Vec3 n){
	Vec3 v = obj->lin_velocity;
	Vec3 w = obj->ang_velocity;
	float m = obj->inverse_mass;
	Matrix<float> I = obj->inv_inertia_tensor;
	
	float rv    = n*(v+(w%r));
	float imsum = (m+n*(((I*(r%n))%r)));
	// n.Print("n");
	// v.Print("v");
	// w.Print("w");
	// r.Print("r");
	// std::cout << "rv: " << rv << std::endl;
	// std::cout << "imsum: " << imsum << std::endl;
	// std::cout << "I " << std::endl;
	// obj->inertia_tensor.print();
	// std::cout << std::endl;
	// std::cout << "Inv tensor" << std::endl;
	// I.print();
	// std::cout << std::endl;
	return -2*rv/imsum;
}
void Physics::update(void){
	if(!this->frozen){
	
		Collision::manifold.clear();
	
		////////////////////////////////////////////////////// Spheres //////////////////////////////////////////////////////
	
		for(unsigned int i = 0; i < __spheres.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			__drag = -(__spheres[i]->drag_coeff*__spheres[i]->inverse_mass*__spheres[i]->lin_velocity.Length2())*__spheres[i]->lin_velocity_n;
			
			__acceleration = this->gravity+__drag;
			
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
					__P = this->relative_momentum(__spheres[i],__colli.r_1,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__colli.r_1,__P);
					// __spheres[i]->manifold[0].Print("contact");
					// this->frozen = true;
				}
			}
			
			/////////////////////////////////////
			// Collision against other spheres //
			/////////////////////////////////////
			
			for(unsigned int j = 0; j < __spheres.size(); ++j){
				if(i==j) continue;
				__colli = Collision::sphere2sphere(__spheres[i],__spheres[j]);
				if(__colli.collision){
					__P = this->relative_momentum(__spheres[i],__spheres[j],__colli.r_1,__colli.r_2,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__colli.r_1,__P);
					__spheres[j]->update_velocities(__colli.normal,__colli.r_2,-__P);
					// this->frozen = true;
				}
			}
			
			/////////////////////////////////
			// Collision against polyhedra //
			/////////////////////////////////
			
			for(unsigned int j = 0; j < __polyhedra.size(); ++j){
				__colli = Collision::sphere2poly(__spheres[i],__polyhedra[j]);
				if(__colli.collision){
					__P = this->relative_momentum(__spheres[i],__polyhedra[j],__colli.r_1,__colli.r_2,__colli.normal);
					__spheres[i]->update_velocities(__colli.normal,__colli.r_1,__P);
					__polyhedra[j]->update_velocities(__colli.normal,__colli.r_2,-__P);
					// this->frozen = true;
				}
			}
		}
		
		////////////////////////////////////////////////////// Polyhedra //////////////////////////////////////////////////////
		
		for(unsigned int i = 0; i < __polyhedra.size(); ++i){
			////////////////////////////////
			// Movement & External Forces //
			////////////////////////////////
			__drag = -(__polyhedra[i]->drag_coeff*__polyhedra[i]->inverse_mass*__polyhedra[i]->lin_velocity.Length2())*__polyhedra[i]->lin_velocity_n;

			__acceleration = this->gravity+__drag;
			
			__polyhedra[i]->integrate(this->dt,__acceleration,Null3);
			
			//////////////////////////////////////////////////////////////////////////
			// If a polyhedron is too fast and moves out of the box pull it back in //
			//////////////////////////////////////////////////////////////////////////

			if(Collision::outside_scene(__polyhedra[i],__cage->planes)){
				__polyhedra[i]->old_mass_center();
				Collision::pull_to_closest_wall(__polyhedra[i],__cage->planes);
			}
			
			/////////////////////////////
			// Collision against walls //
			/////////////////////////////
			
			for(unsigned int j = 0; j < __cage->planes.size(); ++j){
				__colli = Collision::poly2plane(__polyhedra[i],__cage->planes[j]);
				if(__colli.collision){
					__P = this->relative_momentum(__polyhedra[i],__colli.r_1,__colli.normal);
					__P*=.1;
					__polyhedra[i]->update_velocities(__colli.normal,__colli.r_1,__P);
					// __polyhedra[i]->manifold[k].Print("contact");
					// std::cout << "P: " << __P << std::endl;
					// __polyhedra[i]->ang_velocity.Print("ang");
					// this->frozen = true;
				}
			}
			
			///////////////////////////////////////
			// Collision against other polyhedra //
			///////////////////////////////////////
			
			for(unsigned int j = 0; j < __polyhedra.size(); ++j){
				if(i==j) continue;
				__colli = Collision::poly2poly(__polyhedra[i],__polyhedra[j]);
				if(__colli.collision){
					__P = this->relative_momentum(__polyhedra[i],__polyhedra[j],__colli.r_1,__colli.r_2,__colli.normal);
					__P*=.1;
					__polyhedra[i]->update_velocities(__colli.normal,__colli.r_1,__P);
					__polyhedra[j]->update_velocities(__colli.normal,__colli.r_2,-__P);
					//this->frozen = true;
				}
			}
		}
	}
}