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
			virtual void material_color(int side, Vec3 color);
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
			~Plane(void);
			void draw(void);
			void del(void);
		private:
			void init_plane_normal(void);
	}; 
	
	class Object : public Render_Object{
		public:
			Vec3 mass_center_old  = Null3;
			Vec3 mass_center      = Null3;
			Vec3 lin_velocity     = Null3;
			Vec3 lin_acceleration = Null3;
			Vec3 ang_velocity     = Null3;
			Vec3 tangent          = Null3;
			Vec3 binormal         = Null3;
			std::vector<std::vector<float> > inertia_tensor = {{0,0,0},{0,0,0},{0,0,0}};
			float drag_coeff  	  = 0;
			float mass 	   		  = 0;
			float omega           = 0;
			float velocity        = 0;
			float volume_x3 	  = 0;
			float radius 		  = 0;
			void update_lin_velocity(Vec3 n, float P);
			void set_lin_velocity(Vec3 velocity);
			void old_mass_center(void);
			virtual void scale(int scale_dir, float factor) = 0;
			virtual void rotate(const Vec3 &n, float theta, const Vec3 rotation_point) = 0;
			virtual void move(float dt, Vec3 acceleration) = 0;
			virtual void pull(Vec3 n, float overlap);
			virtual void select(bool selection) = 0;
		protected:
			virtual void init_inertia_tensor(void) = 0;
			virtual void init_vertex_buffer(void) = 0;
			virtual void update_volume(void) = 0;
			
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
			float volume_x3 = 12000;
		private:
			std::vector<std::vector<unsigned int> > index_buffer;
			void init_planes(void);
	};
	
	class Sphere : public Object{
		public:
			Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color);
			Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color, Vec3 init_lin_velocity);
			~Sphere(void);
			void move(float dt, Vec3 acceleration);
			void rotate(const Vec3 &n, float theta, const Vec3 rotation_point);
			void scale(int scale_dir, float factor);
			void select(bool selection);
			void draw(void);
		private:
			unsigned int res = 24;
			void init(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color);
			void init_inertia_tensor(void);
			void init_vertex_buffer(void);
			void update_volume(void);
			std::vector<Vec3> normals;
	};
	
	class Cuboid : public Object{
		public:
			Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color);
			~Cuboid(void);
			Vec3 pmin = Null3;
			Vec3 pmax = Null3;
			Vec3 hl   = Null3;
			float m_i = 0;
			std::vector<Vec3> orientation = {Vec3(1,0,0),Vec3(0,1,0),Vec3(0,0,1)};
			std::vector<Plane*> planes;
			// void pull(Vec3 n, float overlap);
			void move(float dt, Vec3 acceleration);
			void rotate(const Vec3 &n, float theta, const Vec3 rotation_point);
			void scale(int scale_dir, float factor);
			void select(bool selection);
			void draw(void);
		private:
			std::vector<std::vector<unsigned int> > index_buffer;
			void init(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color);
			void init_vertex_buffer(void);
			void init_planes(void);
			void init_mass_center(void);
			void init_inertia_tensor(void);
			void update_volume(void);
	};

#endif