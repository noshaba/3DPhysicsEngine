#ifndef _GAME_HPP
#define _GAME_HPP

	#include "Physics.hpp"
	#include "Model.hpp"

	class Game{
		private:
			Object_Model* object_model;
			Slider_Model* slider_model;
			Button_Model* button_model;
			void toggle_options(bool displayed);
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
			void add_cylinder(Cylinder* cylinder);
			void select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos);
			void scale_selected_object(float factor);
			void rotate_selected_object(const Vec3 &n, float theta);
			void move_selected_object(const Vec3 &n, float factor);
			void set_mass_selected_object(float mass);
			void set_drag_selected_object(float drag);
			void color_selected_object(Vec3 color);
			void draw(void);
			void draw_HUD(void);
			Slider* get_slider(float xpos, float ypos);
			Button* get_button(float xpos, float ypos);
			Button* get_button(std::string name);
	};

#endif