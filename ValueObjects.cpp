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

HUD_Element::HUD_Element(float xpos, float ypos, std::string img_path, bool is_displayed){
	this->xpos = xpos;
	this->ypos = ypos;
	this->img_path = img_path;
	this->is_displayed = is_displayed;
	this->load_texture();
}
HUD_Element::~HUD_Element(void){
	glDeleteTextures(1, &(this->texture_id));
}
void HUD_Element::load_texture(void){
	// Data read from the header of the BMP file
	unsigned char header[54];  // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;  // Position in the file where the actual data begins
	unsigned int imageSize;

	// Open the file
	FILE* file = fopen(this->img_path.c_str(), "rb");
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
		// 4 : one byte for each Red, Green, Blue and Alpha component
		imageSize = this->width * this->height * 4;
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
	
	glEnable(GL_TEXTURE_2D);
	// "Bind" the newly created texture : all future texture functions will
	// modify this texture
	glBindTexture(GL_TEXTURE_2D, this->texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->img_data);

	glDisable(GL_TEXTURE_2D);
	delete[] this->img_data;
}
void HUD_Element::draw(void){
	glColor3f(this->grey_shade,this->grey_shade,this->grey_shade);
	if(this->is_displayed){
		glEnable(GL_TEXTURE_2D); //Enable texture
		glBindTexture(GL_TEXTURE_2D, this->texture_id);
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
	glColor3f(1,1,1);
}

Button::Button(std::string name, float xpos, float ypos, unsigned int tile_width, unsigned int tile_height, std::string img_path, bool is_displayed, bool is_activated) : 
	HUD_Element(xpos,ypos,img_path,is_displayed)
{
	this->name = name;
	this->tile_width = tile_width;
	this->tile_height = tile_height;
	this->is_activated = is_activated;
}
Button::~Button(void){}
void Button::draw(void){
	glColor3f(this->grey_shade, this->grey_shade, this->grey_shade);
	if(this->is_displayed){
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, this->texture_id);
		glEnable(GL_TEXTURE_2D); //Enable texture
		glBegin(GL_QUADS); //Begin quadrilateral coordinates
		float t = (float) this->tile_width / (float) this->width;
		if(!this->is_activated){
			glTexCoord2f(0,1); glVertex2f(this->xpos, this->ypos); // top left corner
			glTexCoord2f(t,1); glVertex2f(this->xpos + this->tile_width, this->ypos); // top right corner
			glTexCoord2f(t,0); glVertex2f(this->xpos + this->tile_width, this->ypos + this->tile_height); // bottom right corner
			glTexCoord2f(0,0); glVertex2f(this->xpos, this->ypos + this->tile_height); // bottom left corner
		} else {
			glTexCoord2f(fmod(t,1),1); glVertex2f(this->xpos, this->ypos); // top left corner
			glTexCoord2f(1,1); glVertex2f(this->xpos + this->tile_width, this->ypos); // top right corner
			glTexCoord2f(1,0); glVertex2f(this->xpos + this->tile_width, this->ypos + this->tile_height); // bottom right corner
			glTexCoord2f(fmod(t,1),0); glVertex2f(this->xpos, this->ypos + this->tile_height); // bottom left corner
		}
		glEnd(); //End quadrilateral coordinates
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	glColor3f(1,1,1);
}

Slider::Slider(std::string name, float xpos, float ypos, std::string img_path, bool is_displayed, float min, float max, float value,std::string dir, float b_xpos, float b_ypos, std::string b_img_path) :
	HUD_Element(xpos,ypos,img_path,is_displayed), slider_bar(b_xpos,b_ypos,b_img_path,is_displayed)
{
	this->name = name;
	this->min_value = min;
	this->max_value = max;
	this->abs_value = max - min;
	this->inv_abs_value = 1.0f/abs_value;
	this->min_xpos = xpos;
	this->min_ypos = ypos;
	this->max_xpos = slider_bar.xpos + slider_bar.width - this->width;
	this->max_ypos = slider_bar.ypos + slider_bar.height - this->height;
	this->inv_abs_xpos = 1.0f/(max_xpos - min_xpos);
	this->inv_abs_ypos = 1.0f/(max_ypos - min_ypos);
	(dir == "vertical") ? (direction = Slider::VERTICAL) : (direction = Slider::HORIZONTAL);
	this->set_value(value);
}
Slider::~Slider(void){}
void Slider::set_value(float v){
	this->value = v;
	float rel_val = (value - min_value)*inv_abs_value;
	(direction == Slider::VERTICAL) ? (this->ypos = rel_val * this->max_ypos + (1 - rel_val) * this->min_ypos) : 
									  (this->xpos = rel_val * this->max_xpos + (1 - rel_val) * this->min_xpos);
}
void Slider::set_position(float x, float y){
	if(direction == Slider::VERTICAL){
		// if the slider is vertical, only change the position in Y-direction
		ypos = y - height * .5f;
		if(y >= max_ypos) ypos = max_ypos;
		if(y <= min_ypos) ypos = min_ypos;
		value = ((ypos - min_ypos) * inv_abs_ypos * abs_value) + min_value;
	} else {
		// if the slider is horizontal, only change the position in X-direction
		xpos = x - width * .5f;
		if(xpos >= max_xpos) xpos = max_xpos;
		if(xpos <= min_xpos) xpos = min_xpos;
		value = ((xpos - min_xpos) * inv_abs_xpos * abs_value) + min_value;
	}
}
void Slider::draw(void){
	glColor3f(this->grey_shade, this->grey_shade, this->grey_shade);
	this->slider_bar.draw();
	if(this->is_displayed){
		glEnable(GL_TEXTURE_2D); //Enable texture
		glBindTexture(GL_TEXTURE_2D, this->texture_id);
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
	glColor3f(1,1,1);
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
Plane::Plane(Vec3* color){
	this->color = color;
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
void Object::set_mass(float m){
	this->mass = m;
	this->inverse_mass = 1.0/m;
}
void Object::set_intertia_tensor(Matrix<float> tensor){
	this->inertia_tensor = tensor;
	this->inv_inertia_tensor = !tensor;
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
}
void Sphere::rotate(const Vec3 &n, float theta, const Vec3 rotation_point){
	if(theta != 0 && n.Length2() != 0){
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
	double  a1, a1d = M_PI / (double) this->res,
			a2, a2d = M_PI / (double) this->res,
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
}
void Sphere::select(bool selection){
	this->is_selected = selection;
}
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

void Polyhedron::init(float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle){
	this->mass = mass;
	if(this->mass) this->inverse_mass = 1.0/mass;
	this->drag_coeff = drag_coeff;
	this->color = color;
	this->init_vertex_buffer();
	this->init_planes();
	this->m_i = this->mass/(float) this->vertex_buffer.size();
	this->init_mass_center();
	this->init_inertia_tensor();
	this->rotate(orientation,angle,this->mass_center);
}
void Polyhedron::init_planes(void){
	for(unsigned int i = 0; i < this->index_buffer.size(); ++i){
		this->planes.push_back(new Plane(this->color));
		for(unsigned int j = 0; j < this->index_buffer[i].size(); ++j)
			this->planes[i]->vertex_buffer.push_back(this->vertex_buffer[this->index_buffer[i][j]]);
		this->planes[i]->init_plane_normal();
	}
}
void Polyhedron::init_mass_center(void){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		this->mass_center += this->m_i*(*(this->vertex_buffer[i]));
	this->mass_center /= this->mass;
}
void Polyhedron::rotate(const Vec3 &n, float theta, const Vec3 rotation_point){
	if(theta != 0 && n.Length2() != 0){
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
		for(unsigned int i = 0; i < this->edge_orientation.size(); ++i){
			this->edge_orientation[i] = R*this->edge_orientation[i];
		}
		this->mass_center = R*(this->mass_center - rotation_point) + rotation_point;
		this->inertia_tensor = R*this->inertia_tensor*~R;
		this->inv_inertia_tensor = !this->inertia_tensor;
	}
}
void Polyhedron::rotate(const Matrix<float> &R, const Vec3 rotation_point){
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i){
		*(this->vertex_buffer[i]) = R*(*(this->vertex_buffer[i]) - rotation_point) + rotation_point;
	}
	for(unsigned int i = 0; i < this->planes.size(); ++i){
		this->planes[i]->normal = R*this->planes[i]->normal;
	}
	for(unsigned int i = 0; i < this->axis_orientation.size(); ++i){
		this->axis_orientation[i] = R*this->axis_orientation[i];
	}
	for(unsigned int i = 0; i < this->edge_orientation.size(); ++i){
		this->edge_orientation[i] = R*this->edge_orientation[i];
	}
	this->mass_center = R*(this->mass_center - rotation_point) + rotation_point;
	this->inertia_tensor = R*this->inertia_tensor*~R;
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Polyhedron::select(bool selection){
	this->is_selected = selection;
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		planes[i]->is_selected = selection;
}
void Polyhedron::draw(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		this->planes[i]->draw();
}

Cuboid::Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle){
	this->pmin = pmin;
	this->pmax = pmax;
	this->axis_orientation = {Vec3(1,0,0),Vec3(0,1,0),Vec3(0,0,1)};
	this->edge_orientation = {Vec3(1,0,0),Vec3(0,1,0),Vec3(0,0,1)};
	for(unsigned int i = 0; i < VEC_DIM; ++i){
		this->hl.push_back((this->pmax.p[i] - this->pmin.p[i]) * .5);
	}
	// left, right, top, bottom, back, front
	this->index_buffer = {{0,1,5,4},{3,7,6,2},{4,5,6,7},{0,3,2,1},{0,4,7,3},{1,2,6,5}};
	this->init(mass,drag_coeff,color,orientation,angle);
	this->radius = (this->pmax - this->mass_center).Length();
}
Cuboid::Cuboid(Vec3 pmin, Vec3 pmax, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse){
	this->pmin = pmin;
	this->pmax = pmax;
	this->axis_orientation = {Vec3(1,0,0),Vec3(0,1,0),Vec3(0,0,1)};
	this->edge_orientation = {Vec3(1,0,0),Vec3(0,1,0),Vec3(0,0,1)};
	for(unsigned int i = 0; i < VEC_DIM; ++i){
		this->hl.push_back((this->pmax.p[i] - this->pmin.p[i]) * .5);
	}
	// left, right, top, bottom, back, front
	this->index_buffer = {{0,1,5,4},{3,7,6,2},{4,5,6,7},{0,3,2,1},{0,4,7,3},{1,2,6,5}};
	this->init(mass,drag_coeff,color,orientation,angle);
	this->radius = (this->pmax - this->mass_center).Length();
	this->set_lin_velocity(impulse);
}
Cuboid::~Cuboid(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
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
void Cuboid::init_inertia_tensor(void){
	Vec3 s(this->pmax-this->pmin);
	for(unsigned int i = 0; i < this->inertia_tensor.rows; ++i)
		this->inertia_tensor[i][i] = (this->mass/12.0)*(s*s-s.p[i]*s.p[i]);
	this->inv_inertia_tensor = !this->inertia_tensor;
}
void Cuboid::scale(int scale_dir, float factor){
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
	this->radius = (this->pmax - this->mass_center).Length();
}

Triangle_Prism::Triangle_Prism(float length, float side, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle){
	this->length = length;
	this->side = side;
	this->height = sqrt(3)*0.5f*side;
	float h = height/3.0f;
	this->hl = {length*0.5f, h, h, h};
	// bottom, front, back, left, right
	this->index_buffer = {{2,3,1,0},{0,1,5,4},{4,5,3,2},{2,0,4},{1,3,5}};
	this->init(mass,drag_coeff,color,orientation,angle);
	this->axis_orientation = {planes[4]->normal, planes[0]->normal, planes[1]->normal, planes[2]->normal};
	this->edge_orientation = {planes[4]->normal, Vec3(0,0,1), *(vertex_buffer[4])-*(vertex_buffer[0]), *(vertex_buffer[4])-*(vertex_buffer[2])};
	for(unsigned int i = 0; i < this->edge_orientation.size(); ++i)
		this->edge_orientation[i].Normalize();
	this->radius = (*(this->vertex_buffer[0]) - this->mass_center).Length();
}
Triangle_Prism::Triangle_Prism(float length, float side, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse){
	this->length = length;
	this->side = side;
	this->height = sqrt(3)*0.5f*side;
	float h = height/3.0f;
	this->hl = {length*0.5f, h, h, h};
	// bottom, front, back, left, right
	this->index_buffer = {{2,3,1,0},{0,1,5,4},{4,5,3,2},{2,0,4},{1,3,5}};
	this->init(mass,drag_coeff,color,orientation,angle);
	this->axis_orientation = {planes[4]->normal, planes[0]->normal, planes[1]->normal, planes[2]->normal};
	this->edge_orientation = {planes[4]->normal, Vec3(0,0,1), *(vertex_buffer[4])-*(vertex_buffer[0]), *(vertex_buffer[4])-*(vertex_buffer[2])};
	for(unsigned int i = 0; i < this->edge_orientation.size(); ++i)
		this->edge_orientation[i].Normalize();
	this->radius = (*(this->vertex_buffer[0]) - this->mass_center).Length();
	this->set_lin_velocity(impulse);
}
Triangle_Prism::~Triangle_Prism(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}
void Triangle_Prism::init_vertex_buffer(void){
	this->vertex_buffer = {
							new Vec3(-hl[0], 0, 0.5f*side), new Vec3(hl[0], 0, 0.5f*side),
							new Vec3(-hl[0], 0,-0.5f*side), new Vec3(hl[0], 0,-0.5f*side),
							new Vec3(-hl[0], height, 0), new Vec3(hl[0], height, 0)
	};
}
void Triangle_Prism::init_inertia_tensor(void){
	for(unsigned int i = 0; i < this->inertia_tensor.rows; ++i){
		for(unsigned int j = 0; j < this->inertia_tensor.cols; ++j){
			if(i == j){
				for(unsigned int k = 0; k < this->vertex_buffer.size(); ++k)
					this->inertia_tensor[i][j] += this->m_i * (*(this->vertex_buffer[k]) * *(this->vertex_buffer[k]) - this->vertex_buffer[k]->p[i]);
			} else {
				for(unsigned int k = 0; k < this->vertex_buffer.size(); ++k)
					this->inertia_tensor[i][j] -= this->m_i * this->vertex_buffer[k]->p[i] * this->vertex_buffer[k]->p[j];
			}
		}
	}
}
void Triangle_Prism::scale(int scale_dir, float factor){
	if(scale_dir == SCALE_A) {
		for(unsigned int i = 0; i < this->planes.size(); ++i){
			for(unsigned int j = 0; j < this->planes[i]->vertex_buffer.size(); ++j)
				*(this->planes[i]->vertex_buffer[j]) += this->planes[i]->normal * factor;
		}
		for(unsigned int i = 0; i < this->hl.size(); ++i)
			this->hl[i] += factor;
			
		this->length += factor * 2;
		this->height = this->hl[1] * 3;
		this->side = this->height / sqrt(3) * 2;
	} else if(scale_dir == SCALE_X) {
		for(unsigned int i = 0; i < this->planes[3]->vertex_buffer.size(); ++i)
			*(this->planes[3]->vertex_buffer[i]) += this->planes[3]->normal * factor;
			
		for(unsigned int i = 0; i < this->planes[4]->vertex_buffer.size(); ++i)
			*(this->planes[4]->vertex_buffer[i]) += this->planes[4]->normal * factor;
			
		this->hl[0] += factor;
		this->length += factor * 2;
	} else {
		for(unsigned int i = 0; i < 3; ++i){
			for(unsigned int j = 0; j < this->planes[i]->vertex_buffer.size(); ++j)
				*(this->planes[i]->vertex_buffer[j]) += this->planes[i]->normal * factor;
		}
		for(unsigned int i = 1; i < this->hl.size(); ++i)
			this->hl[i] += factor;
			
		this->height = this->hl[1] * 3;
		this->side = this->height / sqrt(3) * 2;
	}
	this->radius = (*(this->vertex_buffer[0]) - this->mass_center).Length();
}

Cylinder::Cylinder(float length, float circle_radius, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle){
	this->length = length;
	this->circle_radius = circle_radius;
	this->axis_orientation = { Vec3(0,1,0), Vec3(0,0,1) };
	this->edge_orientation = { Vec3(1,0,0) };
	this->radius = length * .5f;
	this->hl = { this->radius, this->circle_radius };
	this->init_index_buffer();
	this->init(mass,drag_coeff,color,orientation,angle);
}
Cylinder::Cylinder(float length, float circle_radius, float mass, float drag_coeff, Vec3* color, Vec3 orientation, float angle, Vec3 impulse){
	this->length = length;
	this->circle_radius = circle_radius;
	this->axis_orientation = { Vec3(0,1,0) };
	this->radius = length * .5f;
	this->hl = { this->radius };
	this->init_index_buffer();
	this->init(mass,drag_coeff,color,orientation,angle);
	this->set_lin_velocity(impulse);
}
Cylinder::~Cylinder(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}
void Cylinder::init_vertex_buffer(void){
	double a = 2 * M_PI / (double) this->res;
	for(int i = -1; i < 2; i+=2){
		for(unsigned int j = 0; j < this->res; ++j){
			this->vertex_buffer.push_back(new Vec3(this->circle_radius * cos(j * a), i * this->length * .5f, this->circle_radius * sin(j * a)));
		}
	}
}
void Cylinder::init_index_buffer(void){
	std::vector<unsigned int> plane_vertices;
	// upper face
	for(unsigned int i = 0; i < this->res; ++i)
		plane_vertices.push_back(i);
	this->index_buffer.push_back(plane_vertices);
	plane_vertices.clear();
	// lower face
	for(unsigned int i = this->res * 2 - 1; i >= this->res; --i)
		plane_vertices.push_back(i);
	this->index_buffer.push_back(plane_vertices);
	plane_vertices.clear();
	// side faces
	for(unsigned int i = 0; i < this->res; ++i){
		this->index_buffer.push_back({i, this->res + i, this->res + (i + 1) % this->res, (i + 1) % this->res});
	}
}
void Cylinder::scale(int scale_dir, float factor){
	Vec3 vertex_norm;
	if(scale_dir == SCALE_A){
		for(unsigned int i = 0; i < this->res; ++i){
			vertex_norm = *(this->planes[0]->vertex_buffer[i]) / this->planes[0]->vertex_buffer[i]->Length();
			*(this->planes[0]->vertex_buffer[i]) += this->planes[0]->normal * factor;
			*(this->planes[1]->vertex_buffer[i]) += this->planes[1]->normal * factor;
		}
		for(unsigned int i = 2; i < this->planes.size(); ++i){
			for(unsigned int j = 0; j < this->planes[i]->vertex_buffer.size(); ++j)
				*(this->planes[i]->vertex_buffer[j]) += this->planes[i]->normal * factor * .5f;
		}
		this->length += factor * 2;
		this->radius += factor;
		this->circle_radius += factor;
	} else if(scale_dir == SCALE_Y){
		for(unsigned int i = 0; i < this->res; ++i){
			*(this->planes[0]->vertex_buffer[i]) += this->planes[0]->normal * factor;
			*(this->planes[1]->vertex_buffer[i]) += this->planes[1]->normal * factor;
		}
		this->length += factor * 2;
		this->radius += factor;
	} else {
		for(unsigned int i = 2; i < this->planes.size(); ++i){
			for(unsigned int j = 0; j < this->planes[i]->vertex_buffer.size(); ++j)
				*(this->planes[i]->vertex_buffer[j]) += this->planes[i]->normal * factor * .5f;
		}
		this->circle_radius += factor;
	}
	this->hl = { this->radius, this->circle_radius };
}
void Cylinder::init_inertia_tensor(void){
	float k = 1.0 / 12.0;
	for(unsigned int i = 0; i < this->inertia_tensor.rows - 1; ++i)
		this->inertia_tensor[i][i] = k*this->mass*(3*this->circle_radius*this->circle_radius + this->length * this->length);
	this->inertia_tensor[2][2] = .5f * this->mass * this->circle_radius * this->circle_radius;
	this->inv_inertia_tensor = !this->inertia_tensor;
}

Target::Target(Vec3 camera_horizontal) : Cylinder(1,3,1,0,new Vec3(1,1,1),Null3,0){
	this->mass = 0;
	this->inverse_mass = 0;
	for(unsigned int i = 0; i < this->inertia_tensor.rows - 1; ++i)
		this->inertia_tensor[i][i] = 0;
	this->inv_inertia_tensor = inertia_tensor;
	this->rotate(camera_horizontal, 60,this->mass_center);
	this->pull(Vec3(0,0,1),-7);
}
Target::~Target(void){
	for(unsigned int i = 0; i < this->planes.size(); ++i)
		delete this->planes[i];
	for(unsigned int i = 0; i < this->vertex_buffer.size(); ++i)
		delete this->vertex_buffer[i];
	delete this->color;
}