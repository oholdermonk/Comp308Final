/*
 * leaf.cpp
 *
 *  Created on: 13/06/2017
 *      Author: Ashton
 */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "cgra_geometry.hpp"
#include "cgra_math.hpp"
#include "simple_image.hpp"
#include "opengl.hpp"

using namespace std;
using namespace cgra;


class Leaf{
	public: vec3 position;
	public: bool reached = false;

public: Leaf(){
	position = vec3::random();
	position.x = (position.x *200) -100;
	position.y = (position.y *200) +80;
	position.z = (position.z *200) -100;
}

public: void setReached(){
	//std::cout << "A leaf has been reached" << std::endl;
	reached = true;
}

public: void show(){
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glColor3f(1, 0, 0);
	cgraSphere(2,10,10,false);
	glPopMatrix();
}

};



