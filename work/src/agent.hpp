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
#include <map>
#include "cgra_math.hpp"
#include "gsim/se_lct.h"


enum AgentType {human, bird};

class Agent {
private:
	cgra::vec2 position;
	cgra::vec2 target;
	int id;
	AgentType agentType;
	float radius;
	void initModel();
	float initRadius();
	float rotation; // in radians
	bool _needPath = false;
	bool _needMove = false;
	bool _needTarget = false;
	bool _isRandom = false;
	std::vector<cgra::vec2> path;
	cgra::vec2 rotateVec2(float, float, float);
	float *model;
	//static std::map<AgentType, float*> modelMap = std::map<AgentType, float*>();
public:
	Agent(AgentType);
	float getRadius();
	void render();
	void update(cgra::vec2);
	void updateKallmann(cgra::vec2);
	cgra::vec2 getPosition();
	cgra::vec2 getTarget();
	void setTarget(cgra::vec2);
	void setPosition(cgra::vec2);
	void setId(int);
	int getId();
	GsPolygon getOutline();
	std::vector<cgra::vec2> getVertices();
	bool needPath();
	bool needMove();
	void setPath(std::vector<cgra::vec2>);
	cgra::vec2 getNextTarget();
	void setNeedPath();
	void setNeedTarget();
	bool needTarget();
	void setIsRandom(bool);
	bool isRandom();
	float* getModel(int &);
	//static void initModelMap();
};