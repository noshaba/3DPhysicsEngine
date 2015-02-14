#include "View.hpp"
#include "Game.hpp"

#include <stdlib.h>

// #define RAND_VARIANCE ((double) rand() / (RAND_MAX))

unsigned int window_width = 1366;
unsigned int window_height = 700;

// int scene = 2;

// float dt = 1/200.0;
// Vec3 gravity(0,-9.81,0);
// Vec3 acceleration, drag, n, r_1, r_2;
// Collision::Collision_Info colli;
// float P;

View* view = new View(window_width,window_height,Vec3(0,15,40),-25);

// Cage* cage = new Cage({new Vec3(0,-10,-10),new Vec3(-10,-10,0),new Vec3(0,-10,10),new Vec3(10,-10,0),new Vec3(0,10,-10),new Vec3(10,10,0),
					   // new Vec3(0,10,10),new Vec3(-10,10,0),new Vec3(10,0,-10),new Vec3(10,0,10),new Vec3(-10,0,-10),new Vec3(-10,0,10)});
Game* game = new Game();

Button_Model* button_model = new Button_Model("ButtonDatabase.db");

// std::vector<Sphere*> spheres;
// std::vector<Cuboid*> cuboids;
							
// void init_scene(){
	// switch(scene){
		// case 0:
			// {
				// spheres = {
							// new Sphere(1,Vec3(0,0,0),1,0, new Vec3(1,0,0), Vec3(100,100,0))
						// };
			// }
			// break;
		// case 1:
			// {
				// gravity = Null3;
				// spheres = {
							// new Sphere(1,Vec3(5,0,0),1,.45, new Vec3(1,0,0)),
							// new Sphere(1,Vec3(0,-9,0),2,0, new Vec3(1,0,0), Vec3(10,0,10))
						// };
			// }
			// break;
		// case 2:
			// {
				// spheres = {
							// new Sphere(1,Vec3(-5,-9,0),1,.45, new Vec3(1,0,0)),
							// new Sphere(1,Vec3(-5,-7,0),1,.45, new Vec3(1,0,0), Vec3(5,0,0)),
							// new Sphere(1,Vec3(0,9,0),1,.45, new Vec3(1,0,0))
						// };
			// }
			// break;
		// case 3:
			// {
				// spheres = {
							// new Sphere(1,Vec3(0,5,0),1,.45,new Vec3(1,0,0))
						// };
				// cuboids = {
							// new Cuboid(Vec3(-5,0,-5),Vec3(4,2,7),8,1,new Vec3(0,1,0))
						// };
			// }
			// break;
		// default:
			// break;
	// }
// }

double xpos_last;
double ypos_last;

void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos){
	if(glfwGetKey(window,GLFW_KEY_LEFT_ALT) || glfwGetKey(window,GLFW_KEY_RIGHT_ALT)){
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE)){
			if(xpos_last < xpos){
				view->camera->rotate(RIGHT,1);
			} else if(xpos_last > xpos){
				view->camera->rotate(LEFT,1);
			}
			if(ypos_last < ypos){
				view->camera->rotate(DOWN,1);
			} else if(ypos_last > ypos){
				view->camera->rotate(UP,1);
			}
		} else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)){
			if(ypos_last < ypos){
				view->camera->move(DOWN,.5);
			} else if(ypos_last > ypos){
				view->camera->move(UP,.5);
			}
			if(xpos_last > xpos){
				view->camera->move(RIGHT,.5);
			} else if(xpos_last < xpos){
				view->camera->move(LEFT,.5);
			}
		} else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)){
			if(ypos_last > ypos){
				view->camera->move(BACK,.5);
			} else if(ypos_last < ypos){
				view->camera->move(FRONT,.5);
			}
		}
	} else {
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) || glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE) || glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)){
			if(xpos_last < xpos){
				game->cage->rotate(YVec3,5);
			} else if(xpos_last > xpos){
				game->cage->rotate(YVec3,-5);
			}
			if(ypos_last < ypos){
				game->cage->rotate(view->camera->horizontal,5);
			} else if(ypos_last > ypos){
				game->cage->rotate(view->camera->horizontal,-5);
			}
		}
	}
	xpos_last = xpos;
	ypos_last = ypos;
}

void glfwSetMouseButton(GLFWwindow* window, int button, int action, int mods){
	double xpos, ypos;
	if(action == GLFW_RELEASE){
		switch(mods){
			case GLFW_MOD_CONTROL:
				glfwGetCursorPos(window,&xpos,&ypos);
				game->select_object(view->viewport,view->camera->position,xpos,ypos);
				break;
		}
	}
}	

void glfwSetScroll(GLFWwindow* window, double xoffset, double yoffset){
	if(yoffset > 0){
		view->camera->move(FRONT,1);
	} else if(yoffset < 0){
		view->camera->move(BACK,1);
	}
}						

void glfwSetKey(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_RELEASE){
		if(mods == GLFW_MOD_SHIFT){
			switch(key){
				case GLFW_KEY_S:
					game->add_sphere(new Sphere(1,Vec3(0,0,0),1,.45,new Vec3(1,0,0),Vec3((double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX))));
					break;
				case GLFW_KEY_C:
					game->add_cuboid(new Cuboid(Vec3(-1,-6,-1),Vec3(1,-4,1),8,1,new Vec3(0,1,0)));
					break;
			}
		} else {
			switch(key){
				case GLFW_KEY_P:
					game->physics.frozen = !game->physics.frozen;
					break;
				case GLFW_KEY_EQUAL:
					game->scale_selected_object(.1);
					break;
				case GLFW_KEY_MINUS:
					game->scale_selected_object(-.1);
					break;
				case GLFW_KEY_1:
					game->scale_dir = SCALE_X;
					break;
				case GLFW_KEY_2:
					game->scale_dir = SCALE_Y;
					break;
				case GLFW_KEY_3:
					game->scale_dir = SCALE_Z;
					break;
				case GLFW_KEY_4:
					game->scale_dir = SCALE_A;
					break;
			}
		}
	} else if(action == GLFW_REPEAT){
		switch (key){
			case GLFW_KEY_W:
				view->camera->move(FRONT,1);
				break;
			case GLFW_KEY_S:
				view->camera->move(BACK,1);
				break;
			case GLFW_KEY_A:
				view->camera->move(LEFT,1);
				break;
			case GLFW_KEY_D:
				view->camera->move(RIGHT,1);
				break;
			case GLFW_KEY_SPACE:
				view->camera->move(UP,1);
				break;
			case GLFW_KEY_X:
				view->camera->move(DOWN,1);
				break;
			case GLFW_KEY_UP:
				view->camera->rotate(UP,1);
				break;
			case GLFW_KEY_DOWN:
				view->camera->rotate(DOWN,1);
				break;
			case GLFW_KEY_RIGHT:
				view->camera->rotate(RIGHT,1);
				break;
			case GLFW_KEY_LEFT:
				view->camera->rotate(LEFT,1);
				break;
			case GLFW_KEY_EQUAL:
					game->scale_selected_object(.1);
					break;
			case GLFW_KEY_MINUS:
				game->scale_selected_object(-.1);
				break;
			default:
				break;
		}
	}
}

// draw the entire scene
void render3Dscene() {
	// switch on lighting (or you don't see anything)
	view->set_3Dviewport_and_lightning();
	// set background color
	glClearColor(17/255.0, 17/255.0, 17/255.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	game->draw();
	// cage->draw();
	// for(unsigned int i = 0; i < spheres.size(); ++i)
		// spheres[i]->draw();
	// for(unsigned int i = 0; i < cuboids.size(); ++i)
		// cuboids[i]->draw();
}
void render2Dhud(){
	view->set_2Dviewport();
	button_model->draw();
}

int main() {
	GLFWwindow* window = NULL;
	printf("Here we go!\n");

	if(!glfwInit()){
		return -1;
	}
	
	window = glfwCreateWindow(window_width, window_height,"Simple 3D Animation", NULL, NULL);
	glfwSetKeyCallback(window,glfwSetKey);
	glfwSetMouseButtonCallback(window,glfwSetMouseButton);
	glfwSetCursorPosCallback(window,glfwSetCursorPos);
	glfwSetScrollCallback(window,glfwSetScroll);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// init_scene();
	while(!glfwWindowShouldClose(window)) {
		glfwMakeContextCurrent(window);
		game->update();
		// for(unsigned int i = 0; i < spheres.size(); ++i){
			//////////////////////////////
			// Movement & External Forces //
			//////////////////////////////
			// drag = -(spheres[i]->drag_coeff/spheres[i]->mass*spheres[i]->lin_velocity.Length2())*spheres[i]->tangent;

			// acceleration = gravity+drag;
			// spheres[i]->move(dt,acceleration);
			
			////////////////////////////////////////////////////////////////////
			// If a sphere is too fast and moves out of the box pull it back in //
			////////////////////////////////////////////////////////////////////

			// if(Collision::outside_scene(spheres[i],cage->planes)){
				// spheres[i]->old_mass_center();
				// n = Collision::get_collided_wall(spheres[i],cage->planes)->normal;
				// spheres[i]->update_lin_velocity(n,spheres[i]->relative_momentum_wall(n));
			// }
			
			// for(unsigned int j = 0; j < cage->planes.size(); ++j){
				// colli = Collision::sphere2plane(spheres[i],cage->planes[j]);
				// if(colli.collision){
					// spheres[i]->pull(colli.normal,colli.overlap);
					// spheres[i]->update_lin_velocity(colli.normal,spheres[i]->relative_momentum_wall(colli.normal));
				// }
			// }
			
			/////////////////////////////
			// Collision against cuboids //
			/////////////////////////////
			
			// for(unsigned int j = 0; j < cuboids.size(); ++j){
				// if(Collision::sphere2aabb(spheres[i],cuboids[j])){
					// if(Collision::sphere2obb(spheres[i],cuboids[j])){
						
					// }
				// }
			// }
			
			///////////////////////////////////
			// Collision against other spheres //
			///////////////////////////////////
			
			// for(unsigned int j = 0; j < spheres.size(); ++j){
				// if(i==j) continue;
				// colli = Collision::sphere2sphere(spheres[i],spheres[j]);
				// if(colli.collision){
					// spheres[i]->pull(colli.normal,colli.overlap);
					// spheres[j]->pull(-colli.normal,colli.overlap);
					// r_1 = .5*(spheres[j]->mass_center - spheres[i]->mass_center);
					// r_2 = -r_1;
					// P = Object::relative_momentum(spheres[i],spheres[j],r_1,r_2,colli.normal);
					// spheres[i]->update_lin_velocity(colli.normal,P);
					// spheres[j]->update_lin_velocity(-colli.normal,P);
				// }
			// }
			
		// }
		render3Dscene();
		render2Dhud();
		// make it appear (before this, it's hidden in the rear buffer)
		glfwSwapBuffers(window);
		glfwPollEvents();
		//std::cin.ignore();
	}

	glfwTerminate();

	printf("Goodbye!\n");
	
	delete view;
	delete game;
	delete button_model;
	// for(unsigned int i = 0; i < spheres.size(); ++i) delete spheres[i];
	return 0;
}
