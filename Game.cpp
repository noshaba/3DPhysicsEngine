#include "Game.hpp"

Game::Game(void){
	this->physics = Physics();
	this->object_model = new Object_Model();
	this->cage = new Cage({new Vec3(0,-10,-10),new Vec3(-10,-10,0),new Vec3(0,-10,10),new Vec3(10,-10,0),new Vec3(0,10,-10),new Vec3(10,10,0),
						   new Vec3(0,10,10),new Vec3(-10,10,0),new Vec3(10,0,-10),new Vec3(10,0,10),new Vec3(-10,0,-10),new Vec3(-10,0,10)});
	this->physics.add_cage(this->cage);
	this->slider_model = new Slider_Model("SliderDatabase.db");
}
Game::~Game(void){
	delete this->object_model;
	delete this->slider_model;
	delete this->cage;
}
void Game::update(void){
	this->physics.update();
}
void Game::add_sphere(Sphere* sphere){
	this->object_model->add_object(sphere);
	this->cage->add_object(sphere);
	this->physics.add_sphere(sphere);
}
void Game::add_cuboid(Cuboid* cuboid){
	this->object_model->add_object(cuboid);
	this->cage->add_object(cuboid);
	this->physics.add_polyhedron(cuboid);
}
void Game::add_triangle_prism(Triangle_Prism* prism){
	this->object_model->add_object(prism);
	this->cage->add_object(prism);
	this->physics.add_polyhedron(prism);
}
void Game::add_cylinder(Cylinder* cylinder){
	this->object_model->add_object(cylinder);
	this->cage->add_object(cylinder);
	this->physics.add_polyhedron(cylinder);
}
void Game::add_target(Target* target){
	this->object_model->add_object(target);
	this->cage->add_object(target);
	this->physics.add_polyhedron(target);
}
void Game::select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos){
	this->object_model->select_object(viewport, camera_position,xpos,ypos);
	if(this->object_model->selected_object){
		this->physics.frozen = true;
		this->slider_model->get_slider("Mass")->set_value(this->object_model->selected_object->mass);
		this->slider_model->get_slider("Drag")->set_value(this->object_model->selected_object->drag_coeff);
	}
}
void Game::scale_selected_object(float factor){
	this->object_model->scale_selected_object(this->scale_dir,factor);
}
void Game::rotate_selected_object(const Vec3 &n, float theta){
	this->object_model->rotate_selected_object(n,theta);
}
void Game::move_selected_object(const Vec3 &n, float factor){
	this->object_model->move_selected_object(n,factor);
}
void Game::set_mass_selected_object(float mass){
	if(this->object_model->selected_object)
		this->object_model->selected_object->set_mass(mass);
}
void Game::set_drag_selected_object(float drag){
	if(this->object_model->selected_object)
		this->object_model->selected_object->drag_coeff = drag;
}
void Game::color_selected_object(Vec3 color){
	if(this->object_model->selected_object)
		*(this->object_model->selected_object->color) = color;
}
void Game::draw(void){
	this->cage->draw();
	this->object_model->draw();
	Collision::draw_manifold();
}
void Game::draw_HUD(void){
	this->slider_model->draw();
}
Slider* Game::get_slider(float xpos, float ypos){
	return this->slider_model->get_slider(xpos,ypos);
}