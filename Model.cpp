#include "Model.hpp"

#include <fstream>

Object_Model::Object_Model(void){}
Object_Model::~Object_Model(void){
	for(unsigned int i = 0; i < this->objects.size(); ++i)
		delete this->objects[i];
}
void Object_Model::add_object(Object* object){
	this->objects.push_back(object);
}
Vec3 Object_Model::selection_ray_direction(Viewport viewport, double xpos, double ypos){
	Vec3 v;
	double ray_start[3], ray_end[3];
	gluUnProject(xpos, viewport.viewport[3] - ypos, 0.0, viewport.model_view_matrix, viewport.projection_matrix, 
				 viewport.viewport, &ray_start[0], &ray_start[1], &ray_start[2]); 
	gluUnProject(xpos, viewport.viewport[3] - ypos, 1.0, viewport.model_view_matrix, viewport.projection_matrix, 
				 viewport.viewport, &ray_end[0], &ray_end[1], &ray_end[2]); 
				 
	for(unsigned int i = 0; i < VEC_DIM; i++)
		v.p[i] = (float) ray_end[i] - ray_start[i];
	v.Normalize();
	return v;
}
void Object_Model::select_closest_object(Vec3 camera_position, Vec3 dir){
	std::vector<Object*> cut_objects;
	for(unsigned int i = 0; i < this->objects.size(); ++i){
		// if object is in front of me
		if((this->objects[i]->mass_center - camera_position)*dir >= 0){
			// if object intersects with direction of the camera
			if((dir%(this->objects[i]->mass_center - camera_position)).Length() <= (this->objects[i]->radius)) 
				cut_objects.push_back(this->objects[i]);
		}
	}
	if(!cut_objects.empty()){
		this->selected_object = cut_objects[0];
		for(unsigned int i = 1; i < cut_objects.size(); ++i){
			if((cut_objects[i]->mass_center-camera_position).Length2() < (this->selected_object->mass_center-camera_position).Length2())
				this->selected_object = cut_objects[i];
		}
		this->selected_object->select(true);
	} else {
		this->selected_object = NULL;
	}
}
void Object_Model::deselect_all_objects(void){
	for(unsigned int i = 0; i < this->objects.size(); ++i)
		this->objects[i]->select(false);
}
void Object_Model::select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos){
	this->deselect_all_objects();
	this->select_closest_object(camera_position,this->selection_ray_direction(viewport,xpos,ypos));
}
void Object_Model::scale_selected_object(int scale_dir, float factor){
	if(this->selected_object) {
		this->selected_object->scale(scale_dir, factor);
	}
}
void Object_Model::rotate_selected_object(const Vec3 &n, float theta){
	if(this->selected_object) {
		this->selected_object->rotate(n,theta,this->selected_object->mass_center);
	}
}
void Object_Model::move_selected_object(const Vec3 &n, float factor){
	if(this->selected_object){
		this->selected_object->pull(n,factor);
	}
}
void Object_Model::draw(void){
	for(unsigned int i = 0; i < this->objects.size(); ++i)
		this->objects[i]->draw();
}

Button_Model::Button_Model(std::string database){
	this->database = database;
	this->load_buttons();
}
Button_Model::~Button_Model(void){
	for(unsigned int i = 0; i < this->buttons.size(); ++i)
		delete this->buttons[i];
}
void Button_Model::load_buttons(void){
	std::ifstream file(this->database.c_str());
	std::string name, xpos, ypos, tile_width, tile_height, img_path, is_displayed, is_activated;
	if(file.is_open() && file.good()){
		while(!file.eof()){
			// load buttons from file and save them as button objects
			getline(file,name);
			getline(file,xpos);
			getline(file,ypos);
			getline(file,tile_width);
			getline(file,tile_height);
			getline(file,img_path);
			getline(file,is_displayed);
			getline(file,is_activated);
			
			Button* button = new Button(name,atof(xpos.c_str()),atof(ypos.c_str()),atoi(tile_width.c_str()),atoi(tile_height.c_str()),img_path.c_str(),(bool) atoi(is_displayed.c_str()), (bool) atoi(is_activated.c_str()));
			this->buttons.push_back(button);
		}
		file.close();
	}
}
Button* Button_Model::get_button(double xpos, double ypos){
	for(unsigned int i = 0; i < this->buttons.size(); i++){
		// return button when clicked on
		if((xpos >= buttons[i]->xpos) && (xpos <= buttons[i]->xpos + buttons[i]->tile_width) &&
		   (ypos >= buttons[i]->ypos) && (ypos <= buttons[i]->ypos + buttons[i]->tile_height)) return buttons[i];
	}
	return NULL;
}
Button* Button_Model::get_button(std::string name){
	for(unsigned int i = 0; i < this->buttons.size(); i++){
		// return button when clicked on
		if(buttons[i]->name == name) return buttons[i];
	}
	return NULL;
}
void Button_Model::draw(void){
	for(unsigned int i = 0; i < this->buttons.size(); ++i)
		(this->buttons[i])->draw();
}

Slider_Model::Slider_Model(std::string database){
	this->database = database;
	this->load_sliders();
}
Slider_Model::~Slider_Model(void){
	for(unsigned int i = 0; i < this->sliders.size(); ++i)
		delete this->sliders[i];
}
void Slider_Model::load_sliders(void){
	std::ifstream file(this->database.c_str());
	std::string name, xpos, ypos, img_path, is_displayed, min, max, direction, b_xpos, b_ypos, b_img_path;
	if(file.is_open() && file.good()){
		while(!file.eof()){
			// load buttons from file and save them as button objects
			getline(file,name);
			getline(file,xpos);
			getline(file,ypos);
			getline(file,img_path);
			getline(file,is_displayed);
			getline(file,min);
			getline(file,max);
			getline(file,direction);
			getline(file,b_xpos);
			getline(file,b_ypos);
			getline(file,b_img_path);
			
			Slider* slider = new Slider(name,atof(xpos.c_str()),atof(ypos.c_str()),img_path.c_str(),(bool) atoi(is_displayed.c_str()),atof(min.c_str()),atof(max.c_str()),direction,atof(b_xpos.c_str()),atof(b_ypos.c_str()),b_img_path);
			this->sliders.push_back(slider);
		}
		file.close();
	}
}
Slider* Slider_Model::get_slider(double xpos, double ypos){
	for(unsigned int i = 0; i < this->sliders.size(); i++){
		// return button when clicked on
		if((xpos >= sliders[i]->xpos) && (xpos <= sliders[i]->xpos + sliders[i]->width) &&
		   (ypos >= sliders[i]->ypos) && (ypos <= sliders[i]->ypos + sliders[i]->height)) return sliders[i];
	}
	return NULL;
}
Slider* Slider_Model::get_slider(std::string name){
	for(unsigned int i = 0; i < this->sliders.size(); i++){
		// return button when clicked on
		if(sliders[i]->name == name) return sliders[i];
	}
	return NULL;
}
void Slider_Model::draw(void){
	for(unsigned int i = 0; i < this->sliders.size(); ++i)
		(this->sliders[i])->draw();
}