#include "Game.hpp"

Game::Game(void){
	this->physics = Physics();
	this->object_model = new Object_Model();
	this->cage = new Cage({new Vec3(0,-10,-10),new Vec3(-10,-10,0),new Vec3(0,-10,10),new Vec3(10,-10,0),new Vec3(0,10,-10),new Vec3(10,10,0),
						   new Vec3(0,10,10),new Vec3(-10,10,0),new Vec3(10,0,-10),new Vec3(10,0,10),new Vec3(-10,0,-10),new Vec3(-10,0,10)});
	this->physics.add_cage(this->cage);
}
Game::~Game(void){
	delete this->object_model;
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
void Game::select_object(Viewport viewport, Vec3 camera_position, double xpos, double ypos){
	this->object_model->select_object(viewport, camera_position,xpos,ypos);
	this->physics.frozen = this->object_model->selected_object;
}
void Game::scale_selected_object(float factor){
	this->object_model->scale_selected_object(this->scale_dir,factor);
}
void Game::rotate_selected_object(const Vec3 &n, float theta){
	this->object_model->rotate_selected_object(n,theta);
}
void Game::draw(void){
	this->cage->draw();
	this->object_model->draw();
	Collision::draw_manifold();
}