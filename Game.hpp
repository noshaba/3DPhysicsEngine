#ifndef _GAME_HPP
#define _GAME_HPP

	#include "Physics.hpp"
	#include "Model.hpp"
	#include "View.hpp"

	class Game{
		private:
			Object_Model* object_model;
			Slider_Model* slider_model;
			Button_Model* button_model;
			float a = .05f;
		public:
			float imp_length = 0;
			GameState state = NONE;
			Vec3 horizontal_vec = Null3;
			Physics physics;
			Game(unsigned int window_width, unsigned int window_height);
			~Game(void);
			Cage* cage;
			View* view;
			int scale_dir = SCALE_A;
			void update(void);
			void add_sphere(Sphere* sphere);
			void add_cuboid(Cuboid* cuboid);
			void add_triangle_prism(Triangle_Prism* prism);
			void add_cylinder(Cylinder* cylinder);
			void add_target(Target* target);
			void rotate_camera(Movement m, float value);
			void move_camera(Movement m, float value);
			Object* select_object(double xpos, double ypos);
			void scale_selected_object(float factor);
			void rotate_selected_object(const Vec3 &n, float theta);
			void move_selected_object(const Vec3 &n, float factor);
			void set_mass_selected_object(float mass);
			void set_drag_selected_object(float drag);
			void color_selected_object(Vec3 color);
			void draw(void);
			Slider* get_slider(float xpos, float ypos);
			Button* get_button(float xpos, float ypos);
			Button* get_button(std::string name);
	};

#endif