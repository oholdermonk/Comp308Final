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
#include "opengl.hpp"
#include "geometry.hpp"
using namespace std;
using namespace cgra;


ParkObject::ParkObject(ParkObjectType _parkObjectType, vec2 _position) {
	objType = _parkObjectType;
	dimensions = initDimensions();
	position = _position;
	initModel();
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
	case bench: return vec2(1, 5);
	case tree: return vec2(1, 1);
	case fence: return vec2(0.1, 3);
	}
	return vec2(0, 0);
}

void ParkObject::render() {
	glPushMatrix();
	glColor3f(0, 0, 1);
	if (objType == tree) { glColor3f(0, 1, 0); }
	glTranslatef(position.x, position.y, 0);
	glRotatef(degrees(rotation), 0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-dimensions.x / 2, -dimensions.y / 2, 0);
	glVertex3f(-dimensions.x / 2, dimensions.y / 2, 0);
	glVertex3f(dimensions.x / 2, dimensions.y / 2, 0);
	glVertex3f(dimensions.x / 2, -dimensions.y / 2, 0);
	glEnd();
	glPopMatrix();
}

void ParkObject::setID(int _id) {
	id = _id;
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

void ParkObject::setRotation(float rot) {
	rotation = rot;
}

vec2 ParkObject::getPosition() {
	return position;
}

void ParkObject::initModel() {
	string filename;
	switch (objType) {
	case bench:
		filename = "./work/res/models/Bench_LowRes.obj";
		break;
	case tree:
		filename = "";
		break;
	case fence:
		filename = "";
		break;
	}
	model = Geometry::getModel(filename);
}

float* ParkObject::getModel(int &size) {
	size = sizeof(*model) / sizeof(float);
	return model;
}