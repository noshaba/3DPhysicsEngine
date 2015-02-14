#ifndef _VIEW_HPP
#define _VIEW_HPP

	#include "ValueObjects.hpp"

	class Camera{
		public:
			Vec3 position, look_at, up_vector, direction, horizontal;
			float up_rotation = 0;
			Camera(Vec3 position, float up_rotation);
			~Camera(void);
			void rotate(Movement m, float theta);
			void move(Movement m, float value);
		private:
			void rotate_horizontal(float theta);
			void rotate_vertical(float theta);
			void translate(Vec3 direction, float value);
	};
	
	class View{
		public:
			View(unsigned int window_width, unsigned int window_height, Vec3 camera_position, float up_rotation);
			~View(void);
			float near_plane, far_plane;
			Camera* camera;
			const Vec3 get_selection_ray(double xpos, double ypos);
			void set_3Dviewport_and_lightning(void);
			void set_2Dviewport(void);
			unsigned int window_width;
			unsigned int window_height;
			Viewport viewport;
	};
#endif