#ifndef _VALUEOBJECTS_HPP
#define _VALUEOBJECTS_HPP

	#define GLFW_INCLUDE_GLU
	#define SCALE_X 0
	#define SCALE_Y 1
	#define SCALE_Z 2
	#define SCALE_A -1
	
	#include <GL/glfw3.h>
	#include <vector>
	
	#include "Quaternion.hpp"
	#include "MatrixOperations.hpp"
	
	enum Movement{
		RIGHT,LEFT,UP,DOWN,FRONT,BACK
	};
	
	struct Viewport{
		double model_view_matrix[16], projection_matrix[16]; 
		int viewport[4];
	};
	
	class Render_Object{
		public:
			std::vector<Vec3*> vertex_buffer;
			bool is_selected = false;
			virtual void draw(void) = 0;
			static void material_color(int side, Vec3 color);
			Vec3* color;
	};
	
	class HUD_Element{
		protected:
			std::string img_path;
			unsigned char* img_data;
			GLuint texture_id;
			void load_texture(void);
		public:
			HUD_Element(float xpos, float ypos, std::string img_path, bool is_displayed);
			~HUD_Element(void);
			float xpos;
			float ypos;
			unsigned int height;
			unsigned int width;
			void draw(void);
			bool is_displayed;
			float grey_shade = 1;
	};
	
	class Button : public HUD_Element {
		public:
			Button(std::string name, float xpos, float ypos, unsigned int tile_width, unsigned int tile_height, std::string img_path, bool is_displayed, bool is_activated);
			~Button(void);
			std::string name;
			unsigned int tile_width;
			unsigned int tile_height;
			bool is_activated;
			void draw(void);
	};
	
	class Slider : public HUD_Element {
		private:
			float min_value;
			float max_value;
			float abs_value;
			float inv_abs_value;
			HUD_Element slider_bar;
			float min_xpos;
			float max_xpos;
			float inv_abs_xpos;
			float min_ypos;
			float max_ypos;
			float inv_abs_ypos;
			enum Direction {VERTICAL, HORIZONTAL} direction;
		public:
			Slider(std::string name, float xpos, float ypos, std::string img_path, bool is_displayed, float min, float max, float value, std::string dir, float b_xpos, float b_ypos, std::string b_img_path);
			~Slider(void);
			void set_position(float xpos, float ypos);
			void set_value(float v);
			std::string name;
			float value;
			void draw(void);
	};
	
	class Plane : public Render_Object{
		public:
			Vec3 normal;
			Plane(std::vector<Vec3*> vertex_buffer, Vec3* color);
			Plane(std::vector<Vec3*> vertex_buffer);
			Plane(Vec3 normal, Vec3 p);
			Plane(Vec3* color);
			~Plane(void);
			void draw(void);
			void del(void);
			void init_plane_normal(void);
	}; 
	
	class Object : public Render_Object{
		public:
			/// primary physics state
			Vec3 mass_center_old  = Null3;	// position of the mass center in previous frame
			Vec3 mass_center      = Null3;	// position of mass center
			
			/// secondary physics state
			Vec3 lin_velocity     = Null3;	// linear velocity in meters per second
			Vec3 lin_velocity_n   = Null3;	// direction of lin_velocity
			Vec3 ang_velocity     = Null3;	// angular velocity
			Vec3 ang_velocity_n   = Null3;  // direction of ang_velocity
			
			Vec3 impulse 		  = Null3;
			
			bool horizontal_imp	  = false;
			bool vertical_imp	  = false;
			
			std::vector<Vec3> manifold;
			Matrix<float> inertia_tensor = Matrix<float>({{0,0,0},{0,0,0},{0,0,0}});
			Matrix<float> inv_inertia_tensor = Matrix<float>({{0,0,0},{0,0,0},{0,0,0}});
			float drag_coeff  	  = 0;
			float mass 	   		  = 0;
			float m_i 			  = 0; 
			float inverse_mass 	  = 0;
			float omega           = 0;
			float velocity        = 0;
			float radius 		  = 0;
			void update_velocities(Vec3 n, Vec3 r, float P);
			void set_lin_velocity(Vec3 velocity);
			void set_ang_velocity(Vec3 ang_velocity);
			void old_mass_center(void);
			void move(float dt, Vec3 acceleration);
			void integrate(float dt, Vec3 acceleration,Vec3 ang_acceleration);
			void pull(Vec3 n, float overlap);
			void set_mass(float m);
			void set_intertia_tensor(Matrix<float> tensor);
			virtual void select(bool selection) = 0;
			virtual void scale(int scale_dir, float factor) = 0;
			virtual void rotate(const Vec3 &n, float theta, const Vec3 rotation_point) = 0;
			virtual void rotate(const Matrix<float> &R, const Vec3 rotation_point) = 0;
		protected:
			virtual void init_inertia_tensor(void) = 0;
			virtual void init_vertex_buffer(void) = 0;
			
	};
	
	class Cage : public Render_Object{
		public:
			std::vector<Plane*> planes;
			std::vector<Object*> objects;
			Cage(std::vector<Vec3*> vertex_buffer);
			~Cage(void);
			void draw(void);
			void rotate(const Vec3 &n, float theta);
			void add_object(Object* object);
		private:
			std::vector<std::vector<unsigned int> > index_buffer;
			void init_planes(void);
	};
	
	class Sphere : public Object{
		public:
			Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color);
			Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color, Vec3 init_lin_velocity);
			~Sphere(void);
			void rotate(const Vec3 &n, float theta, const Vec3 rotation_point);
			void rotate(const Matrix<float> &R, const Vec3 rotation_point);
			void scale(int scale_dir, float factor);
			void select(bool selection);
			void draw(void);
		private:
			unsigned int res = 24;
			void init(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color);
			void init_inertia_tensor(void);
			void init_vertex_buffer(void);
			std::vector<Vec3> normals;
	};
	
	class Polyhedron : public Object{
		public:
			std::vector<float> hl;
			std::vector<Vec3> axis_orientation;
			std::vector<Vec3> edge_orientation;
			std::vector<Plane*> planes;
			void rotate(const Vec3 &n, float theta, const Vec3 rotation_point);
			void rotate(const Matrix<float> &R, const Vec3 rotation_point);
			void select(bool selection);
			void draw(void);
			virtual void scale(int scale_dir, float factor) = 0;
		protected:
			std::vector<std::vector<unsigned int> > index_buffer;
			void init_planes(void);
			void init_mass_center(void);
			void init(float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle);
			virtual void init_vertex_buffer(void) = 0;
			virtual void init_inertia_tensor(void) = 0;
	};
	
	class Cuboid : public Polyhedron{
		public:
			Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle);
			Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse);
			~Cuboid(void);
			Vec3 pmin = Null3;
			Vec3 pmax = Null3;
			void scale(int scale_dir, float factor);
		private:
			void init_vertex_buffer(void);
			void init_inertia_tensor(void);
	};
	
	class Triangle_Prism : public Polyhedron{
		public:
			Triangle_Prism(float length, float side, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle);
			Triangle_Prism(float length, float side, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse);
			~Triangle_Prism(void);
			float length = 0;
			float side = 0;
			float height = 0;
			void scale(int scale_dir, float factor);
		private:
			void init_vertex_buffer(void);
			void init_inertia_tensor(void);
	};
	
	class Cylinder : public Polyhedron{
		public:
			Cylinder(float length, float circle_radius, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle);
			Cylinder(float length, float circle_radius, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse);
			~Cylinder(void);
			float length = 0;
			float circle_radius = 0;
			void scale(int scale_dir, float factor);
		private:
			void init_vertex_buffer(void);
			void init_inertia_tensor(void);
			void init_index_buffer(void);
			unsigned int res = 24;
	};
	
	class Target : public Cylinder{
		public:
			Target(Vec3 camera_horizontal);
			~Target(void);
	};

#endif