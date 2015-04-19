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
		protected:
			Vec3* color;
	};
	
	class HUD_Element{
		protected:
			const char* img_path;
			unsigned char* img_data;
			GLuint texture_id;
			float xpos;
			float ypos;
			unsigned int height;
			unsigned int width;
			void load_texture(void);
			void bind_texture(void);
		public:
			HUD_Element(float xpos, float ypos, const char* img_path, bool is_displayed);
			~HUD_Element(void);
			void draw(void);
			bool is_displayed;
	};
	
	class Button : public HUD_Element {
		private:
			unsigned int tile_width;
			unsigned int tile_height;
		public:
			Button(std::string name, float xpos, float ypos, unsigned int tile_width, unsigned int tile_height, const char* img_path, bool is_displayed);
			~Button(void);
			std::string name;
			bool is_activated;
			void draw(void);
	};
	
	class Plane : public Render_Object{
		public:
			Vec3 normal;
			Plane(std::vector<Vec3*> vertex_buffer, Vec3* color);
			Plane(std::vector<Vec3*> vertex_buffer);
			Plane(Vec3 normal, Vec3 p);
			~Plane(void);
			void draw(void);
			void del(void);
		private:
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
			void select(bool selection);
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
			std::vector<Plane*> planes;
			void scale(int scale_dir, float factor);
		private:
			void init_vertex_buffer(void);
			void init_inertia_tensor(void);
	};

#endif