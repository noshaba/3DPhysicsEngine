#include "ValueObjects.hpp"
#include <GL/gl.h>
#include <iostream>
#include <float.h>

void Render_Object::material_color(int side, Vec3 color){
	float amb[4], dif[4], spe[4];
	dif[0] = color.p[0];
	dif[1] = color.p[1];
	dif[2] = color.p[2];
	for(int i = 0; i < 3; i++) {
		amb[i] = .1 * dif[i];
		spe[i] = .5;
	}
	amb[3] = dif[3] = spe[3] = 1.0;
	glMaterialfv(side, GL_AMBIENT, amb);
	glMaterialfv(side, GL_DIFFUSE, dif);
	glMaterialfv(side, GL_SPECULAR, spe);
	glMaterialf( side, GL_SHININESS, 20);
}

HUD_Element::HUD_Element(float xpos, float ypos, const char* img_path, bool is_displayed){
	this->xpos = xpos;
	this->ypos = ypos;
	this->img_path = img_path;
	this->is_displayed = is_displayed;
	this->load_texture();
	glGenTextures(1, &(this->texture_id));
}
HUD_Element::~HUD_Element(void){
	glDeleteTextures(1, &(this->texture_id));
	delete[] this->img_data;
	delete[] this->img_path;
}
void HUD_Element::load_texture(void){
	// Data read from the header of the BMP file
	unsigned char header[54];  // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;  // Position in the file where the actual data begins
	unsigned int imageSize;

	// Open the file
	FILE* file = fopen(this->img_path, "rb");
	if(!file) {
		printf("Image could not be opened\n");
	}
	if(fread(header, 1, 54, file) != 54) {  // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
	}
	if(header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	this->width = *(int*)&(header[0x12]);
	this->height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if(imageSize == 0) {
		// 3 : one byte for each Red, Green and Blue component
		imageSize = this->width * this->height * 3;
	}
	if(dataPos == 0) {
		// The BMP header is done that way
		dataPos = 54;
	}

	// Create a buffer
	this->img_data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(this->img_data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	glGenTextures(1, &(this->texture_id));
}
void HUD_Element::bind_texture(void){
		// "Bind" the newly created texture : all future texture functions will
		// modify this texture
		glBindTexture(GL_TEXTURE_2D, this->texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, this->img_data);
}
void HUD_Element::draw(void){
	if(this->is_displayed){
		this->bind_texture();	// bind texture
		glEnable(GL_TEXTURE_2D); //Enable texture
		glPushMatrix();
		glBegin(GL_QUADS); //Begin quadrilateral coordinates
		glTexCoord2f(0.0f,1.0f); glVertex2f(this->xpos,this->ypos); // top left corner
		glTexCoord2f(1.0f,1.0f); glVertex2f(this->xpos + this->width,this->ypos); // top right corner
		glTexCoord2f(1.0f,0.0f); glVertex2f(this->xpos + this->width, this->ypos + this->height); // bottom right corner
		glTexCoord2f(0.0f,0.0f); glVertex2f(this->xpos, this->ypos + this->height); // bottom left corner
		glEnd(); //End quadrilateral coordinates
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}

Button::Button(std::string name, float xpos, float ypos, unsigned int tile_width, unsigned int tile_height, const char* img_path, bool is_displayed) : HUD_Element(xpos,ypos,img_path,is_displayed){
	this->name = name;
	this->tile_width = tile_width;
	this->tile_height = tile_height;
	this->is_activated = false;
}

Button::~Button(void){}

void Button::draw(void){
	if(this->is_displayed){
		this->bind_texture();	// bind texture
		glEnable(GL_TEXTURE_2D); //Enable texture
		glPushMatrix();
		glBegin(GL_QUADS); //Begin quadrilateral coordinates
		if(!this->is_activated){
			glTexCoord2f(0.0f,1.0f); glVertex2f(this->xpos,this->ypos); // top left corner
			glTexCoord2f((float) this->tile_width/this->width,1.0f); glVertex2f(this->xpos + this->tile_width,this->ypos); // top right corner
			glTexCoord2f((float) this->tile_width/this->width,0.0f); glVertex2f(this->xpos + this->tile_width, this->ypos + this->tile_height); // bottom right corner
			glTexCoord2f(0.0f,0.0f); glVertex2f(this->xpos, this->ypos + this->tile_height); // bottom left corner
		} else {
			glTexCoord2f((float) this->tile_width/this->width,1.0f); glVertex2f(this->xpos,this->ypos); // top left corner
			glTexCoord2f(1.0f,1.0f); glVertex2f(this->xpos + this->tile_width,this->ypos); // top right corner
			glTexCoord2f(1.0f,0.0f); glVertex2f(this->xpos + this->tile_width, this->ypos + this->tile_height); // bottom right corner
			glTexCoord2f((float) this->tile_width/this->width,1.0f); glVertex2f(this->xpos, this->ypos + this->tile_height); // bottom left corner
		}
		glEnd(); //End quadrilateral coordinates
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}

Plane::Plane(std::vector<Vec3*> vertex_buffer, Vec3* color){
	this->color = color;
	this->vertex_buffer = vertex_buffer;
	this->init_plane_normal();
}
Plane::Plane(std::vector<Vec3*> vertex_buffer){
	this->color = NULL;
	this->vertex_buffer = vertex_buffer;
	this->init_plane_normal();
}
Plane::Plane(Vec3 normal, Vec3 p){
	this->normal = normal;
	this->vertex_buffer.push_back(new Vec3(p));
}
Plane::~Plane(void){}
void Plane::del(void){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); i++){
		delete this->vertex_buffer[i];
	}
	delete this->color;
}
void Plane::init_plane_normal(void){
	this->normal = (*(this->vertex_buffer[1])-*(this->vertex_buffer[0]))%(*(this->vertex_buffer[2])-*(this->vertex_buffer[0]));
	this->normal.Normalize();
}
void Plane::draw(void){
	if(this->color){
		this->material_color(GL_FRONT,*(this->color));
		glBegin(GL_POLYGON);
		glNormal3fv(this->normal.p);
		for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
			glVertex3fv(this->vertex_buffer[i]->p);
		}
		glEnd();
	} else {
		this->material_color(GL_FRONT,Vec3(1,1,1));
		glBegin(GL_LINE_LOOP);
		glNormal3f(0,0,1);
		for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
			glVertex3fv(this->vertex_buffer[i]->p);
		}
		glEnd();
	}
	if(this->is_selected){
		this->material_color(GL_FRONT_AND_BACK,Vec3(1,1,0));
		glBegin(GL_LINE_LOOP);
		glNormal3f(0,0,1);
		for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
			glVertex3fv(this->vertex_buffer[i]->p);
		}
		glEnd();
	}
}

void Object::set_lin_velocity(Vec3 velocity){
	this->lin_velocity = velocity;
	this->velocity = velocity.Length();
	if(this->velocity)
		this->lin_velocity_n = this->lin_velocity/this->velocity;
}
void Object::set_ang_velocity(Vec3 ang_velocity){
	this->ang_velocity = ang_velocity;
	this->omega = ang_velocity.Length();
	if(this->omega)
		this->ang_velocity_n = this->ang_velocity/this->omega;
}
void Object::update_velocities(Vec3 n, Vec3 r, float P){
	this->set_lin_velocity(this->lin_velocity + P*n*this->inverse_mass);
	this->set_ang_velocity(this->ang_velocity + this->inv_inertia_tensor*(r%n)*P);
}
void Object::old_mass_center(void){
	this->mass_center = this->mass_center_old;
}
void Object::pull(Vec3 n, float overlap){
	this->mass_center += n*overlap;
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		*(this->vertex_buffer[i]) += n*overlap;
}
void Object::move(float dt, Vec3 acceleration){
	this->mass_center_old = this->mass_center;
	this->set_lin_velocity(this->lin_velocity + dt*acceleration);
	this->mass_center = this->mass_center + dt*this->lin_velocity;
	for(unsigned int i = 0; i < this->vertex_buffer.size(); i++)
		*(this->vertex_buffer[i]) += dt*this->lin_velocity;
	// this->orientation = this->orientation + this->spin*dt;
}
void Object::integrate(float dt, Vec3 acceleration, Vec3 ang_acceleration){
	this->move(dt,acceleration);
	this->set_ang_velocity(this->ang_velocity+dt*ang_acceleration);
	if(this->ang_velocity != Null3) this->rotate(Matrix<float>(this->ang_velocity_n,this->omega*dt),this->mass_center);
}
// {new Vec3(0,-10,-10),new Vec3(-10,-10,0),new Vec3(0,-10,10),new Vec3(10,-10,0),new Vec3(0,10,-10),new Vec3(10,10,0),
 // new Vec3(0,10,10),new Vec3(-10,10,0),new Vec3(10,0,-10),new Vec3(10,0,10),new Vec3(-10,0,-10),new Vec3(-10,0,10)}
Cage::Cage(std::vector<Vec3*> vertex_buffer){
	this->color = NULL;
	this->vertex_buffer = vertex_buffer;
	this->index_buffer = {{0,1,2,3},{4,5,6,7},{8,3,9,5},{10,7,11,1},{0,8,4,10},{2,11,6,9},
						  {3,8,0},{6,11,7},{4,8,5},{1,11,2},{2,9,3},{5,9,6},{0,10,1},{7,10,4}};
	this->init_planes();
}
Cage::~Cage(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}
void Cage::init_planes(void){
	for(unsigned int i = 0; i < 6; ++i){
		this->planes.push_back(new Plane({
											this->vertex_buffer[this->index_buffer[i][0]],
											this->vertex_buffer[this->index_buffer[i][1]],
											this->vertex_buffer[this->index_buffer[i][2]],
											this->vertex_buffer[this->index_buffer[i][3]],
										},this->color));
	}
	for(unsigned int i = 6; i < this->index_buffer.size(); ++i){
		this->planes.push_back(new Plane({
									this->vertex_buffer[this->index_buffer[i][0]],
									this->vertex_buffer[this->index_buffer[i][1]],
									this->vertex_buffer[this->index_buffer[i][2]],
								},this->color));
	}
}
void Cage::add_object(Object* object){
	this->objects.push_back(object);
}
void Cage::draw(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		this->planes[i]->draw();
}
void Cage::rotate(const Vec3 &n, float theta){
	theta = theta*M_PI/180.0;
	Matrix<float> R = Matrix<float>(n,theta);
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		*(this->vertex_buffer[i]) = R*(*(this->vertex_buffer[i]));
	
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		this->planes[i]->normal = R*this->planes[i]->normal;
	
	for(unsigned int i = 0; i < this->objects.size(); ++i)
		this->objects[i]->rotate(n,theta*100,Null3);
}
Sphere::Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color){
	this->init(radius,mass_center,mass,drag_coeff,color);
}
Sphere::Sphere(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color, Vec3 init_lin_velocity){
	this->init(radius,mass_center,mass,drag_coeff,color);
	this->set_lin_velocity(init_lin_velocity);
}
Sphere::~Sphere(void){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}
void Sphere::init(float radius, Vec3 mass_center, float mass, float drag_coeff, Vec3* color){
	this->radius = radius;
	this->mass_center = mass_center;
	this->mass = mass;
	if(this->mass) this->inverse_mass = 1.0/mass;
	this->drag_coeff = drag_coeff;
	this->color = color;
	this->init_inertia_tensor();
	this->init_vertex_buffer();
	this->m_i = this->mass/(float) this->vertex_buffer.size();
	this->update_volume();
}
void Sphere::select(bool selection){
	this->is_selected = selection;
}
void Sphere::rotate(const Vec3 &n, float theta, const Vec3 rotation_point){
	Quaternion q(n,theta);
	theta = theta*M_PI/180.0;
	Matrix<float> R = Matrix<float>(n,theta);
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
		*(this->vertex_buffer[i]) = q*(*(this->vertex_buffer[i]) - rotation_point) + rotation_point;
	}
	for(unsigned int i = 0; i < this->normals.size(); ++i){
		this->normals[i] = q*this->normals[i];
	}
	this->mass_center = q*(this->mass_center - rotation_point) + rotation_point;
	this->inertia_tensor = R*this->inertia_tensor*~R;
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Sphere::rotate(const Matrix<float> &R, const Vec3 rotation_point){
	// Quaternion q(this->ang_velocity);
	// for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
		// *(this->vertex_buffer[i]) = q*(*(this->vertex_buffer[i]) - rotation_point) + rotation_point;
	// }
	// for(unsigned int i = 0; i < this->normals.size(); ++i){
		// this->normals[i] = q*this->normals[i];
	// }
	// this->mass_center = R*(this->mass_center - rotation_point) + rotation_point;
	this->inertia_tensor = R*this->inertia_tensor*~R;
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Sphere::init_inertia_tensor(void){
	for(unsigned int i = 0; i < this->inertia_tensor.rows; ++i)
		this->inertia_tensor[i][i] = .4*this->mass*this->radius*this->radius;
	this->inv_inertia_tensor = !this->inertia_tensor;	
}
void Sphere::init_vertex_buffer(void){
	double  a1, a1d = M_PI / this->res,
			a2, a2d = M_PI / this->res,
			s1, s2,
			c1, c2;
	Vec3 	n;
			
	for(unsigned int i = 0; i < this->res; ++i){
		a1 = i * a1d;
		for(unsigned int j = 0; j <= this->res; ++j){
			a2 = (j+.5*(i%2)) * 2 * a2d;
			s1 = sin(a1);
			c1 = cos(a1);
			s2 = sin(a2);
			c2 = cos(a2);
			n  = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);
			this->normals.push_back(n);
			this->vertex_buffer.push_back(new Vec3(this->mass_center + this->radius * n));
			s1 = sin(a1 + a1d);
			c1 = cos(a1 + a1d);
			s2 = sin(a2 + a2d);
			c2 = cos(a2 + a2d);
			n  = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);
			this->normals.push_back(n);
			this->vertex_buffer.push_back(new Vec3(this->mass_center + this->radius * n));
		}
	}
}
void Sphere::scale(int scale_dir, float factor){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		*(this->vertex_buffer[i]) += this->normals[i] * factor;
	this->radius += factor;
	this->update_volume();
}
void Sphere::update_volume(void){
	this->volume_x3 = 4 * M_PI * this->radius * this->radius * this->radius;
	this->volume    = this->volume_x3/3.0;
};
void Sphere::draw(void){
	unsigned int counter = 0;
	this->material_color(GL_FRONT,*(this->color));
	for(unsigned int i = 0; i < this->res; ++i){
		glBegin(GL_TRIANGLE_STRIP);
		for(unsigned int j = 0; j <= this->res; ++j){
			glNormal3fv(this->normals[counter].p);
			glVertex3fv(this->vertex_buffer[counter]->p);
			counter++;
			glNormal3fv(this->normals[counter].p);
			glVertex3fv(this->vertex_buffer[counter]->p);
			counter++;
		}
		glEnd();
	}
	
	if(this->is_selected){
		unsigned int counter = 0;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		this->material_color(GL_FRONT_AND_BACK,Vec3(1,1,0));
		glNormal3f(0,0,1);
		for(unsigned int i = 0; i < this->res; ++i){
			glBegin(GL_TRIANGLE_STRIP);
			for(unsigned int j = 0; j <= this->res; ++j){
				glVertex3fv(this->vertex_buffer[counter]->p);
				counter++;
				glVertex3fv(this->vertex_buffer[counter]->p);
				counter++;
			}
			glEnd();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

Cuboid::Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color){
	this->init(pmin,pmax,mass,drag_coeff,color);
}
Cuboid::Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color, Vec3 impuls, Vec3 orientation, float angle){
	this->init(pmin,pmax,mass,drag_coeff,color);
	this->set_lin_velocity(impuls);
	this->rotate(orientation,angle,this->mass_center);
}
Cuboid::~Cuboid(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}
void Cuboid::init(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color){
	this->pmin = pmin;
	this->pmax = pmax;
	for(unsigned int i = 0; i < this->hl.size(); ++i) this->hl[i] = (this->pmax.p[i] - this->pmin.p[i]) * .5;
	this->mass = mass;
	if(this->mass) this->inverse_mass = 1.0/mass;
	this->drag_coeff = drag_coeff;
	this->color = color;
	this->init_vertex_buffer();
	// left, right, top, bottom, back, front
	this->index_buffer = {{0,1,5,4},{3,7,6,2},{4,5,6,7},{0,3,2,1},{0,4,7,3},{1,2,6,5}};
	this->init_planes();
	this->m_i = this->mass/(float) this->vertex_buffer.size();
	this->init_mass_center();
	this->init_inertia_tensor();
	this->update_volume();
	this->radius = (this->pmax - this->mass_center).Length();
}
void Cuboid::init_vertex_buffer(void){
	this->vertex_buffer = {
							new Vec3(this->pmin.p[0],this->pmin.p[1],this->pmin.p[2]),
							new Vec3(this->pmin.p[0],this->pmin.p[1],this->pmax.p[2]),
							new Vec3(this->pmax.p[0],this->pmin.p[1],this->pmax.p[2]),
							new Vec3(this->pmax.p[0],this->pmin.p[1],this->pmin.p[2]),
							new Vec3(this->pmin.p[0],this->pmax.p[1],this->pmin.p[2]),
							new Vec3(this->pmin.p[0],this->pmax.p[1],this->pmax.p[2]),
							new Vec3(this->pmax.p[0],this->pmax.p[1],this->pmax.p[2]),
							new Vec3(this->pmax.p[0],this->pmax.p[1],this->pmin.p[2])
						};
}
void Cuboid::init_planes(void){
	for(unsigned int i = 0; i < this->index_buffer.size(); ++i){
		this->planes.push_back(new Plane({
											this->vertex_buffer[this->index_buffer[i][0]],
											this->vertex_buffer[this->index_buffer[i][1]],
											this->vertex_buffer[this->index_buffer[i][2]],
											this->vertex_buffer[this->index_buffer[i][3]],
										},this->color));
	}
}
void Cuboid::init_mass_center(void){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		this->mass_center += this->m_i*(*(this->vertex_buffer[i]));
	this->mass_center /= this->mass;
}
void Cuboid::init_inertia_tensor(void){
	Vec3 s(this->pmax-this->pmin);
	for(unsigned int i = 0; i < this->inertia_tensor.rows; ++i)
		this->inertia_tensor[i][i] = (this->mass/12.0)*(s*s-s.p[i]*s.p[i]);
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Cuboid::scale(int scale_dir, float factor){
	if(this->radius < 5){
		if(scale_dir == SCALE_A) {
			for(unsigned int i = 0; i < this->planes.size(); ++i){
				for(unsigned int j = 0; j < this->planes[i]->vertex_buffer.size(); ++j)
					*(this->planes[i]->vertex_buffer[j]) += this->planes[i]->normal * factor;
			}
			for(unsigned int i = 0; i < this->hl.size(); ++i)
				this->hl[i] += factor;
			this->pmax += Vec3(factor,factor,factor);
			this->pmin -= Vec3(factor,factor,factor);
		} else {
			for(unsigned int i = 0; i < this->planes[scale_dir*2]->vertex_buffer.size(); ++i)
				*(this->planes[scale_dir*2]->vertex_buffer[i]) += this->planes[scale_dir*2]->normal * factor;
				
			for(unsigned int i = 0; i < this->planes[scale_dir*2+1]->vertex_buffer.size(); ++i)
				*(this->planes[scale_dir*2+1]->vertex_buffer[i]) += this->planes[scale_dir*2+1]->normal * factor;
			this->hl[scale_dir] += factor;
			this->pmax.p[scale_dir] +=factor;
			this->pmin.p[scale_dir] -= factor;
		}
		this->radius = (this->pmax - this->mass_center).Length() *.5;
		this->update_volume();
	}
}
void Cuboid::select(bool selection){
	this->is_selected = selection;
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		this->planes[i]->is_selected = selection;
}
void Cuboid::update_volume(void){
	this->volume    = this->hl[0] * this->hl[1] * this->hl[2] * 6;
	this->volume_x3 = 3*this->volume;
}
void Cuboid::rotate(const Vec3 &n, float theta, const Vec3 rotation_point){
	theta = theta*M_PI/180.0;
	Matrix<float> R = Matrix<float>(n,theta);
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
		*(this->vertex_buffer[i]) = R*(*(this->vertex_buffer[i]) - rotation_point) + rotation_point;
	}
	for(unsigned int i = 0; i < this->planes.size(); ++i){
		this->planes[i]->normal = R*this->planes[i]->normal;
	}
	for(unsigned int i = 0; i < this->axis_orientation.size(); ++i){
		this->axis_orientation[i] = R*this->axis_orientation[i];
	}
	this->edge_orientation = this->axis_orientation;
	this->mass_center = R*(this->mass_center - rotation_point) + rotation_point;
	this->inertia_tensor = R*this->inertia_tensor*~R;
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Cuboid::rotate(const Matrix<float> &R, const Vec3 rotation_point){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
		*(this->vertex_buffer[i]) = R*(*(this->vertex_buffer[i]) - rotation_point) + rotation_point;
	}
	for(unsigned int i = 0; i < this->planes.size(); ++i){
		this->planes[i]->normal = R*this->planes[i]->normal;
	}
	for(unsigned int i = 0; i < this->axis_orientation.size(); ++i){
		this->axis_orientation[i] = R*this->axis_orientation[i];
	}
	this->edge_orientation = this->axis_orientation;
	this->mass_center = R*(this->mass_center - rotation_point) + rotation_point;
	this->inertia_tensor = R*this->inertia_tensor*~R;
	this->inv_inertia_tensor = !this->inertia_tensor;
	// std::cout << "Cub I:" << std::endl;
	// for(unsigned int i = 0; i < this->inertia_tensor.size(); ++i){
		// for(unsigned int j = 0; j < this->inertia_tensor[0].size(); ++j){
			// std::cout << this->inertia_tensor[i][j] << ' ';
		// }
		// std::cout << std::endl;
	// }
}
void Cuboid::draw(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		this->planes[i]->draw();
}