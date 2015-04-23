#ifndef _GAME_HPP
#define _GAME_HPP

	#include "Physics.hpp"
	#include "Model.hpp"

	class Game{
		private:
			Object_Model* object_model;
		public:
			Physics physics;
			Game(void);
			~Game(void);
			Cage* cage;
			int scale_dir = SCALE_A;
			void update(void);
			void add_sphere(Sphere* sphere);
			void add_cuboid(Cuboid* cuboid);
			void add_triangle_prism(Triangle_Prism* prism);
			void select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos);
			void scale_selected_object(float factor);
			void rotate_selected_object(const Vec3 &n, float theta);
			void move_selected_object(const Vec3 &n, float factor);
			void draw(void);
	};

#endif