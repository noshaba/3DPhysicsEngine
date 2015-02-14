#include "View.hpp"

Camera::Camera(Vec3 position, float up_rotation){
	this->position = position;
	this->look_at = position + Vec3(0,0,-10);
	this->up_vector = YVec3;
	this->direction = this->position - this->look_at;
	this->direction.Normalize();
	this->horizontal = this->up_vector%this->direction;
	this->horizontal.Normalize();
	if(up_rotation != 0)
		this->rotate_vertical(up_rotation);
}
Camera::~Camera(void){}
void Camera::rotate(Movement m, float theta){
	switch(m){
		case UP:
			this->rotate_vertical(theta);
			break;
		case DOWN:
			this->rotate_vertical(-theta);
			break;
		case LEFT:
			this->rotate_horizontal(theta);
			break;
		case RIGHT:
			this->rotate_horizontal(-theta);
			break;
		default:
			break;
	}
}
void Camera::rotate_vertical(float theta){
	Quaternion q(this->horizontal,theta);
	if(this->up_rotation + theta < 90 && this->up_rotation + theta > -90){
		this->up_rotation += theta;
		this->look_at = q*(this->look_at - this->position) + this->position;
		this->direction = q*this->direction;
	}
}
void Camera::rotate_horizontal(float theta){
	Quaternion q(YVec3,theta);
	this->look_at = q*(this->look_at - this->position) + this->position;
	this->direction = q*this->direction;
	this->horizontal = q*this->horizontal;
}
void Camera::move(Movement m, float value){
	switch(m){
		case DOWN:
			this->translate(this->up_vector,-value);
			break;
		case UP:
			this->translate(this->up_vector, value);
			break;
		case FRONT:
			this->translate(this->direction,-value);
			break;
		case BACK:
			this->translate(this->direction, value);
			break;
		case RIGHT:
			this->translate(this->horizontal,value);
			break;
		case LEFT:
			this->translate(this->horizontal,-value);
			break;
		default:
			break;
	}
}
void Camera::translate(Vec3 direction, float value){
	this->position = this->position + direction * value;
	this->look_at = this->look_at + direction * value;
}

View::View(unsigned int window_width, unsigned int window_height, Vec3 camera_position, float up_rotation){
	this->window_width = window_width;
	this->window_height = window_height;
	this->near_plane = 1.0;
	this->far_plane = 100.0;
	this->camera = new Camera(camera_position, up_rotation);
}
View::~View(void){
	delete this->camera;
}
void View::set_2Dviewport(void){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, this->window_width, this->window_height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
}
void View::set_3Dviewport_and_lightning(void) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	GLfloat lp1[4]  = { 10,  5,  10,  0};
	GLfloat lp2[4]  = { -5,  5, -10,  0};
	GLfloat red[4]  = {1.0, .8,  .8,  1};
	GLfloat blue[4] = { .8, .8, 1.0,  1};

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT1, GL_POSITION, lp1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  red);
	glLightfv(GL_LIGHT1, GL_SPECULAR, red);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_POSITION, lp2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE,  blue);
	glLightfv(GL_LIGHT2, GL_SPECULAR, blue);
	glEnable(GL_LIGHT2);

	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// init viewport to canvassize
	glViewport(0, 0, this->window_width, this->window_height);
	
	// init coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0,(-1.0/this->window_width)*this->window_height,(1.0/this->window_width)*this->window_height,this->near_plane,this->far_plane);
	gluLookAt(this->camera->position.p[0], this->camera->position.p[1], this->camera->position.p[2],
			  this->camera->look_at.p[0], this->camera->look_at.p[1], this->camera->look_at.p[2],
			  this->camera->up_vector.p[0], this->camera->up_vector.p[1], this->camera->up_vector.p[2]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, this->viewport.model_view_matrix); 
	glGetDoublev(GL_PROJECTION_MATRIX, this->viewport.projection_matrix); 
	glGetIntegerv(GL_VIEWPORT, this->viewport.viewport);
}
const Vec3 View::get_selection_ray(double xpos, double ypos){
	Vec3 v;
	double ray_start[3], ray_end[3];
	double model_view_matrix[16], projection_matrix[16]; 
	int viewport[4];
	
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view_matrix); 
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix); 
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	gluUnProject(xpos, viewport[3] - ypos, 0.0, model_view_matrix, projection_matrix, 
				 viewport, &ray_start[0], &ray_start[1], &ray_start[2]); 
	gluUnProject(xpos, viewport[3] - ypos, 1.0, model_view_matrix, projection_matrix, 
				 viewport, &ray_end[0], &ray_end[1], &ray_end[2]); 
				 
	for(unsigned int i = 0; i < VEC_DIM; i++)
		v.p[i] = (float) ray_end[i] - ray_start[i];
	v.Normalize();
	return v;
}