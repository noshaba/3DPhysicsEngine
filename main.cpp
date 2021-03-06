#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#include "View.hpp"
#include "Game.hpp"

#include <stdlib.h>

unsigned int window_width = 1366;
unsigned int window_height = 700;

bool round_start = false;
bool ready = false;

Game* game;

Button* button = NULL;
Slider* slider = NULL;
Object* selected_object = NULL;

double xpos_last;
double ypos_last;

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}

void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos){
	if(glfwGetKey(window,GLFW_KEY_LEFT_ALT) || glfwGetKey(window,GLFW_KEY_RIGHT_ALT)){
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE)){
			if(xpos_last < xpos){
				game->rotate_camera(RIGHT,1);
			} else if(xpos_last > xpos){
				game->rotate_camera(LEFT,1);
			}
			if(ypos_last < ypos){
				game->rotate_camera(DOWN,1);
			} else if(ypos_last > ypos){
				game->rotate_camera(UP,1);
			}
		} else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)){
			if(ypos_last < ypos){
				game->rotate_camera(DOWN,.5f);
			} else if(ypos_last > ypos){
				game->rotate_camera(UP,.5f);
			}
			if(xpos_last > xpos){
				game->rotate_camera(RIGHT,.5f);
			} else if(xpos_last < xpos){
				game->rotate_camera(LEFT,.5f);
			}
		} else if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)){
			if(ypos_last > ypos){
				game->rotate_camera(BACK,.5f);
			} else if(ypos_last < ypos){
				game->rotate_camera(FRONT,.5f);
			}
		}
	} else {
		if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) || glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE) || glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)){
			if(button && button->name == "Color")
				button->is_activated = false;
			button = game->get_button(xpos,ypos);
			if(button && button->name == "Color"){
				button->is_activated = true;
				unsigned char pixel[3];
				glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
				game->color_selected_object(Vec3(pixel[0]/255.0f, pixel[1]/255.0f, pixel[2]/255.0f));
			}
			if(slider){
				slider->set_position(xpos,ypos);
				switch(str2int(slider->name.c_str())){
					case str2int("Mass"):
						game->set_mass_selected_object(slider->value);
						break;
					case str2int("Drag"):
						game->set_drag_selected_object(slider->value);
						break;
					case str2int("Restitution"):
						game->set_restitution_selected_object(slider->value);
						break;
					case str2int("Shade"):
						game->get_button("Color")->grey_shade = slider->value;
						break;
				}
			}
			if(!button && !slider && glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_MIDDLE)){
				if(xpos_last < xpos){
					game->cage->rotate(YVec3,5);
				} else if(xpos_last > xpos){
					game->cage->rotate(YVec3,-5);
				}
				if(ypos_last < ypos){
					game->cage->rotate(game->view->camera->horizontal,5);
				} else if(ypos_last > ypos){
					game->cage->rotate(game->view->camera->horizontal,-5);
				}
			}
		}
	}
	xpos_last = xpos;
	ypos_last = ypos;
}

void glfwSetMouseButton(GLFWwindow* window, int mouse_button, int action, int mods){
	double xpos, ypos;
	glfwGetCursorPos(window,&xpos,&ypos);
	if(action == GLFW_PRESS){
		button = game->get_button(xpos, ypos);
		slider = game->get_slider(xpos, ypos);
		if(button) button->is_activated = true;
	} else if(action == GLFW_RELEASE){
		switch(mods){
			case GLFW_MOD_CONTROL:
				selected_object = game->select_object(xpos,ypos);
				if(round_start && selected_object && ready){
					if(!(dynamic_cast<Target*>(selected_object)) && game->state == NONE){
						game->state = HORIZONTAL_IMP;
						selected_object->special = true;
						selected_object->impulse = selected_object->mass_center + ZVec3 * 5;
					}
				}
				if(button) button->is_activated = false;
				button = NULL;
				break;
			default:
				if(selected_object){
					switch(game->state){
						case HORIZONTAL_IMP:
							game->state = VERTICAL_IMP;
							game->horizontal_vec = selected_object->impulse%YVec3;	
							if(game->horizontal_vec == Null3){
								selected_object->impulse = Quaternion(YVec3, 1) * selected_object->impulse;
								game->horizontal_vec = selected_object->impulse%YVec3;
							}
							game->horizontal_vec.Normalize();
							break;
						case VERTICAL_IMP:
							game->state = LENGTH_IMP;
							game->imp_length = selected_object->impulse.Length();
							selected_object->impulse.Normalize();
							break;
						case LENGTH_IMP:
							game->state = SHOOT;
							game->physics.frozen = false;
							selected_object = NULL;
							break;
					}
				}
				slider = NULL;
				if(button){
					button->is_activated = false;
					switch(str2int(button->name.c_str())){
						case str2int("Cuboid"):
							game->add_cuboid(new Cuboid(Vec3(-3,-1,-2),Vec3(3,1,2),8,1,1,new Vec3(0,1,0),Null3,0));
							break;
						case str2int("Sphere"):
							// Vec3((double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX))
							game->add_sphere(new Sphere(1,Vec3(0,0,0),1,1,.45,new Vec3(1,0,0),Vec3((double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX))));
							break;
						case str2int("Triangle_Prism"):
							game->add_triangle_prism(new Triangle_Prism(6,6,6,1,.82f,new Vec3(1,.5f,0),Null3,0));
							break;
						case str2int("Cylinder"):
							game->add_cylinder(new Cylinder(2,1,6,1,.82f,new Vec3(1,0,1),Null3,0));
							break;
						case str2int("Yay"):
							selected_object = NULL;
							game->state = NONE;
							game->reset();
							ready = false;
							round_start = false;
							game->imp_length = 0;
							game->horizontal_vec = Null3;
							game->get_button("Yay")->is_displayed = false;
							game->get_button("Yay")->is_activated = false;
							Object::won = false;
							game->physics.frozen = false;
							break;
						case str2int("Start_Game"):
							selected_object = NULL;
							game->state = NONE;
							game->reset();
							button->is_activated = true;
							round_start = true;
							game->physics.frozen = true;
							game->get_button("Ready")->is_displayed = true;
							game->imp_length = 0;
							game->horizontal_vec = Null3;
							break;
						case str2int("Ready"):
							ready = true;
							game->add_target(new Target(game->view->camera->horizontal));
							game->get_button("Ready")->is_displayed = false;
							game->get_button("Ready")->is_activated = false;
							game->get_button("Start_Game")->is_activated = false;
							game->state = NONE;
							game->imp_length = 0;
							break;
						case str2int("X_Dir"):
							button->is_activated = true;
							game->get_button("Y_Dir")->is_activated = false;
							game->get_button("Z_Dir")->is_activated = false;
							game->get_button("A_Dir")->is_activated = false;
							game->scale_dir = SCALE_X;
							break;
						case str2int("Y_Dir"):
							button->is_activated = true;
							game->get_button("X_Dir")->is_activated = false;
							game->get_button("Z_Dir")->is_activated = false;
							game->get_button("A_Dir")->is_activated = false;
							game->scale_dir = SCALE_Y;
							break;
						case str2int("Z_Dir"):
							button->is_activated = true;
							game->get_button("Y_Dir")->is_activated = false;
							game->get_button("X_Dir")->is_activated = false;
							game->get_button("A_Dir")->is_activated = false;
							game->scale_dir = SCALE_Z;
							break;
						case str2int("A_Dir"):
							button->is_activated = true;
							game->get_button("Y_Dir")->is_activated = false;
							game->get_button("Z_Dir")->is_activated = false;
							game->get_button("X_Dir")->is_activated = false;
							game->scale_dir = SCALE_A;
							break;
						case str2int("Reset"):
							game->reset();
							selected_object = NULL;
							game->get_button("Start_Game")->is_activated = false;
							game->get_button("Ready")->is_activated = false;
							game->get_button("Ready")->is_displayed = false;
							round_start = false;
							ready = false;
							game->state = NONE;
							game->imp_length = 0;
							break;
						case str2int("Pause"):
							game->physics.frozen = !game->physics.frozen || round_start;
							if(game->physics.frozen) button->is_activated = true;
							break;
						case str2int("Move_Front"):
							game->move_selected_object(game->view->camera->direction,.1);
							break;
						case str2int("Move_Back"):
							game->move_selected_object(game->view->camera->direction,-.1);
							break;
						case str2int("Move_Right"):
							game->move_selected_object(game->view->camera->horizontal,.1);
							break;
						case str2int("Move_Left"):
							game->move_selected_object(game->view->camera->horizontal,-.1);
							break;
						case str2int("Move_Up"):
							game->move_selected_object(YVec3,.1);
							break;
						case str2int("Move_Down"):
							game->move_selected_object(YVec3,-.1);
							break;
						case str2int("Scale_Pos"):
							game->scale_selected_object(.1);
							break;
						case str2int("Scale_Neg"):
							game->scale_selected_object(-.1);
							break;
						case str2int("Rot_U"):
							game->rotate_selected_object(game->view->camera->horizontal,-5);
							break;
						case str2int("Rot_D"):
							game->rotate_selected_object(game->view->camera->horizontal, 5);
							break;
						case str2int("Rot_L"):
							game->rotate_selected_object(YVec3,-5);
							break;
						case str2int("Rot_R"):
							game->rotate_selected_object(YVec3, 5);
							break;
						case str2int("Color"):
							unsigned char pixel[3];
							glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
							game->color_selected_object(Vec3(pixel[0]/255.0f, pixel[1]/255.0f, pixel[2]/255.0f));
							break;
					}
				}
				button = NULL;
				break;
		}
	}
}	

void glfwSetScroll(GLFWwindow* window, double xoffset, double yoffset){
	if(yoffset > 0){
		game->move_camera(FRONT,1);
	} else if(yoffset < 0){
		game->move_camera(BACK,1);
	}
}						

void glfwSetKey(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(action == GLFW_RELEASE){
		if(mods == GLFW_MOD_SHIFT){
			switch(key){
				case GLFW_KEY_S:
					game->add_sphere(new Sphere(1,Vec3(0,0,0),1,1,.45f,new Vec3(1,0,0),Vec3((double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX),(double) rand() / (RAND_MAX))));
					break;
				case GLFW_KEY_Q:
					game->add_cuboid(new Cuboid(Vec3(-3,-2,-1),Vec3(3,2,1),1,1,1,new Vec3(0,1,0),Vec3(1,0,0),90,Null3));
					break;
				case GLFW_KEY_T:
					game->add_triangle_prism(new Triangle_Prism(6,6,6,1,.82f,new Vec3(1,.5f,0),Null3,0));
					break;
				case GLFW_KEY_C:
					game->add_cylinder(new Cylinder(2,1,6,1,.82f,new Vec3(1,0,1),Null3,0));
					break;
			}
		} else if(mods == GLFW_MOD_CONTROL) {
			switch(key){
				case GLFW_KEY_EQUAL:
					game->scale_selected_object(.1f);
					break;
				case GLFW_KEY_MINUS:
					game->scale_selected_object(-.1f);
					break;
				case GLFW_KEY_UP:
					game->rotate_selected_object(game->view->camera->horizontal,-5);
					break;
				case GLFW_KEY_DOWN:
					game->rotate_selected_object(game->view->camera->horizontal,5);
					break;
				case GLFW_KEY_LEFT:
					game->rotate_selected_object(YVec3,-5);
					break;
				case GLFW_KEY_RIGHT:
					game->rotate_selected_object(YVec3,5);
					break;
				case GLFW_KEY_W:
					game->move_selected_object(game->view->camera->direction,-.1f);
					break;
				case GLFW_KEY_S:
					game->move_selected_object(game->view->camera->direction,.1f);
					break;
				case GLFW_KEY_A:
					game->move_selected_object(game->view->camera->horizontal,-.1f);
					break;
				case GLFW_KEY_D:
					game->move_selected_object(game->view->camera->horizontal,.1f);
					break;
				case GLFW_KEY_X:
					game->move_selected_object(YVec3,-.1f);
					break;
				case GLFW_KEY_SPACE:
					game->move_selected_object(YVec3,.1f);
					break;
			}
		} else {
			switch(key){
				case GLFW_KEY_P: 
					game->physics.frozen = !game->physics.frozen || round_start;
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
				case GLFW_KEY_R:
					game->reset();
					selected_object = NULL;
					game->get_button("Start_Game")->is_activated = false;
					game->get_button("Ready")->is_activated = false;
					game->get_button("Ready")->is_displayed = false;
					round_start = false;
					ready = false;
					game->state = NONE;
					game->imp_length = 0;
					break;
			}
		}
	} else if(action == GLFW_REPEAT){
		if(mods == GLFW_MOD_CONTROL){
			switch (key){
				case GLFW_KEY_EQUAL:
					game->scale_selected_object(.1f);
					break;
				case GLFW_KEY_MINUS:
					game->scale_selected_object(-.1f);
					break;
				case GLFW_KEY_UP:
					game->rotate_selected_object(game->view->camera->horizontal,-5);
					break;
				case GLFW_KEY_DOWN:
					game->rotate_selected_object(game->view->camera->horizontal,5);
					break;
				case GLFW_KEY_LEFT:
					game->rotate_selected_object(YVec3,-5);
					break;
				case GLFW_KEY_RIGHT:
					game->rotate_selected_object(YVec3,5);
					break;
				case GLFW_KEY_W:
					game->move_selected_object(game->view->camera->direction,-.1f);
					break;
				case GLFW_KEY_S:
					game->move_selected_object(game->view->camera->direction,.1f);
					break;
				case GLFW_KEY_A:
					game->move_selected_object(game->view->camera->horizontal,-.1f);
					break;
				case GLFW_KEY_D:
					game->move_selected_object(game->view->camera->horizontal,.1f);
					break;
				case GLFW_KEY_X:
					game->move_selected_object(YVec3,-.1f);
					break;
				case GLFW_KEY_SPACE:
					game->move_selected_object(YVec3,.1);
					break;
			}
		} else {
			switch(key){
				case GLFW_KEY_W:
					game->move_camera(FRONT,1);
					break;
				case GLFW_KEY_S:
					game->move_camera(BACK,1);
					break;
				case GLFW_KEY_A:
					game->move_camera(LEFT,1);
					break;
				case GLFW_KEY_D:
					game->move_camera(RIGHT,1);
					break;
				case GLFW_KEY_SPACE:
					game->move_camera(UP,1);
					break;
				case GLFW_KEY_X:
					game->move_camera(DOWN,1);
					break;
				case GLFW_KEY_UP:
					game->rotate_camera(UP,1);
					break;
				case GLFW_KEY_DOWN:
					game->rotate_camera(DOWN,1);
					break;
				case GLFW_KEY_RIGHT:
					game->rotate_camera(RIGHT,1);
					break;
				case GLFW_KEY_LEFT:
					game->rotate_camera(LEFT,1);
					break;
			}
		}
	}
}

int main() {
	GLFWwindow* window = NULL;
	printf("Here we go!\n");
	if(!glfwInit()){
		return -1;
	}
	
	window = glfwCreateWindow(window_width, window_height,"Simple 3D Animation", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window,glfwSetKey);
	glfwSetMouseButtonCallback(window,glfwSetMouseButton);
	glfwSetCursorPosCallback(window,glfwSetCursorPos);
	glfwSetScrollCallback(window,glfwSetScroll);
	
	game = new Game(window_width, window_height);
	
	
	while(!glfwWindowShouldClose(window)) {
		game->update();
		if(Object::won) {
			game->get_button("Yay")->is_displayed = true;
			game->physics.frozen = true;
		}
		game->draw();
		// make it appear (before this, it's hidden in the rear buffer)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	printf("Goodbye!\n");
	
	delete game;
	return 0;
}
