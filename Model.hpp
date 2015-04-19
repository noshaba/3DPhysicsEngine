#ifndef _MODEL_HPP
#define _MODEL_HPP

	#include "ValueObjects.hpp"
	
	class Object_Model{
		private:
			std::vector<Object*> objects;
			Vec3 selection_ray_direction(Viewport viewport, double xpos, double ypos);
			void select_closest_object(Vec3 camera_position, Vec3 dir);
		public:
			Object_Model(void);
			~Object_Model(void);
			Object* selected_object = NULL;
			void add_object(Object* object);
			void select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos);
			void deselect_all_objects(void);
			void scale_selected_object(int scale_dir, float factor);
			void rotate_selected_object(const Vec3 &n, float theta);
			void draw(void);
	};

	class Button_Model{
		private:
			std::string database;
			std::vector<Button*> buttons;
			void load_buttons(void);
		public:
			Button_Model(std::string database);
			~Button_Model(void);
			void draw(void);
	};
#endif