/**
*
*         COMP 308 Final Project
*              Tri 1 2017
*     Victoria University of Wellington
*
*          Author: Jiaheng Wang
*
*/

#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "parkobject.hpp"
#include "geometry.hpp"
using namespace std;
using namespace cgra;


ParkObject::ParkObject(ParkObjectType _parkObjectType, vec2 _position) {
	objType = _parkObjectType;
	dimensions = initDimensions();
	position = _position;
}

GsPolygon ParkObject::getOutline() {
	GsPolygon poly;
	vector<vec2> corners = getVertices();
	for (int i = 0; i < corners.size(); i++) {
		poly.push().set(corners[i].x, corners[i].y);
	}
	return poly;
}

vec2 ParkObject::initDimensions() {
	switch (objType) {
	case bench: return vec2(2.4, 1);
	case tree: return vec2(0.5, 0.5);
	//case fence: return vec2(0.1, 3);
	}
	return vec2(0, 0);
}

void ParkObject::setID(int _id) {
	id = _id;
}

int ParkObject::getID()
{
	return id;
}

vector<vec2> ParkObject::getVertices() {
	vector<vec2> corners;
	corners.push_back(position + rotateVec2(-dimensions.x / 2, -dimensions.y / 2, rotation));
	corners.push_back(position + rotateVec2(-dimensions.x / 2, dimensions.y / 2, rotation));
	corners.push_back(position + rotateVec2(dimensions.x / 2, dimensions.y / 2, rotation));
	corners.push_back(position + rotateVec2(dimensions.x / 2, -dimensions.y / 2, rotation));
	return corners;
}

vec2 ParkObject::rotateVec2(float x, float y, float rad) {
	vec2 ret;
	ret.x = x*cosf(rad) - y*sinf(rad);
	ret.y = x*sinf(rad) + y*cosf(rad);
	return ret;
}

void ParkObject::changeRotation(float rot) {
	rotation = rot;
}

vec2 ParkObject::getPosition() {
	return position;
}

void ParkObject::setPosition(vec2 _position) {
	position = _position;
}

float ParkObject::getRotation() {
	return rotation;
}