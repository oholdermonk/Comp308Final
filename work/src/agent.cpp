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

#include "cgra_math.hpp"
#include "agent.hpp"
#include "opengl.hpp"
#include "geometry.hpp"

using namespace std;
using namespace cgra;
using namespace math;

void Agent::initModel() {
	string filename;
	switch (agentType) {
	case human: 
		filename = "./work/res/models/lego.obj";
		break;
	case bird: 
		filename =  "";
		break;
	}
	model = Geometry::getModel(filename);
}

float Agent::initRadius() {
	switch (agentType) {
	case human: return 0.5f;
	case bird: return 0.25f;
	}
	return 0.0f;
}

Agent::Agent(AgentType _agentType) {
	agentType = _agentType;
	radius = initRadius();
	initModel();
}

void Agent::update(vec2 dp) {
	position += dp;
	rotation = atan2f(dp.y, dp.x);
	if (cgra::distance(position, path.back()) < 0.1f) {
		path.pop_back();
		if (path.empty()) {
			_needMove = false;
			_needPath = false;
			path.clear();
		}
	}
}

void Agent::updateKallmann(vec2 dp) {
	position += dp;
	rotation = atan2f(dp.y, dp.x);
}

void Agent::render() {
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(position.x, position.y, 0);
	glRotatef(degrees(rotation), 0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= 360; i+=45) {
		glVertex3f(cosf(radians((float)i))*radius, sinf(radians((float)i))*radius,0);
	}
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(0.5f, 0.2f, 0);
	glVertex3f(0.5f, -0.2f, 0);
	glVertex3f(0.7f, -0.2f, 0);
	glVertex3f(0.7f, 0.2f, 0);
	glEnd();

	glPopMatrix();
}

float Agent::getRadius() {
	return radius;
}

vec2 Agent::getPosition() {
	return position;
}

vec2 Agent::getTarget() {
	return target;
}

void Agent::setTarget(vec2 _target) {
	if (cgra::distance(position, _target) < 0.1f) {
		return;
	}
	target = _target;
	_needMove = true;
	_needPath = true;
	_needTarget = false;
}

void Agent::setPosition(vec2 _position) {
	position = _position;
}

void Agent::setId(int _id) {
	id = _id;
}

int Agent::getId() {
	return id;
}

GsPolygon Agent::getOutline() {
	GsPolygon poly;
	vector<vec2> corners = getVertices();
	for (int i = 0; i < corners.size(); i++) {
		poly.push().set(corners[i].x, corners[i].y);
	}
	return poly;
}

vector<vec2> Agent::getVertices() {
	vector<vec2> corners;
	corners.push_back(position + rotateVec2(radius, 0, 0 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 1 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 2 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 3 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 4 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 5 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 6 * pi() / 4));
	corners.push_back(position + rotateVec2(radius, 0, 7 * pi() / 4));
	return corners;
}

bool Agent::needPath() {
	return _needPath;
}

bool Agent::needMove() {
	return _needMove;
}

void Agent::setNeedPath() {
	_needPath = true;
}

void Agent::setPath(std::vector<cgra::vec2> path) {
	this->path = path;
	_needPath = false;
}

vec2 Agent::getNextTarget() {
	if (path.empty()) {
		return position;
	}
	return path.back();
}

vec2 Agent::rotateVec2(float x, float y, float rad) {
	vec2 ret;
	ret.x = x*cosf(rad) - y*sinf(rad);
	ret.y = x*sinf(rad) + y*cosf(rad);
	return ret;
}

void Agent::setNeedTarget() {
	_needTarget = true;
}

bool Agent::needTarget() {
	return _needTarget;
}

void Agent::setIsRandom(bool isRandom) {
	_isRandom = isRandom;
}

bool Agent::isRandom() {
	return _isRandom;
}

vector<float> Agent::getModel() {
	return model;
}