/**
*
*         COMP 308 Final Project
*              Tri 1 2017
*     Victoria University of Wellington
*
*          Author: Jiaheng Wang
*
*/
#pragma once

#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"
#include "gsim/se_lct.h"

enum ParkObjectType {bench, tree, fence};

class ParkObject {
private:
	ParkObjectType objType;
	cgra::vec2 position;
	int id = 0;
	float rotation = 0; // in radians
	std::string m_filename;
	cgra::vec2 dimensions;
	cgra::vec2 initDimensions();
	cgra::vec2 rotateVec2(float, float, float);
public:
	ParkObject(ParkObjectType, cgra::vec2);
	GsPolygon getOutline();
	std::vector<cgra::vec2> getVertices();
	void setID(int);
	int getID();
	void changeRotation(float);
	float getRotation();
	cgra::vec2 getPosition();
	void setPosition(cgra::vec2);
};