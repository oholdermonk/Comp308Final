/**
*
*         COMP 308 Final Project
*              Tri 1 2017
*     Victoria University of Wellington
*
*          Author: Jiaheng Wang
*
*/
#include <vector>

#include "agent.hpp"
#include "parkobject.hpp"
#include "gsim/se_lct.h"

using namespace std;

struct gridPos {
	gridPos() {

	}
	gridPos(int _x, int _y) {
		x = _x; y = _y;
	}
	int x, y;
	inline bool operator==(const gridPos& rhs) {
		return x == rhs.x && y == rhs.y;
	}

	float dist(const gridPos& rhs) {
		return sqrtf((x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y));
	}
};

struct edge {
	edge(gridPos gp1, gridPos gp2) {
		if (gp1.y < gp2.y) {
			x1 = gp1.x; y1 = gp1.y;
			x2 = gp2.x; y2 = gp2.y;
		}
		else if (gp1.y == gp2.y && gp1.x < gp2.x) {
			x1 = gp1.x; y1 = gp1.y;
			x2 = gp2.x; y2 = gp2.y;
		}
		else {
			x2 = gp1.x; y2 = gp1.y;
			x1 = gp2.x; y1 = gp2.y;
		}
	}
	int x1, y1, x2, y2;
	int xAt(int y) {
		if (y2 == y1) { return x1; }
		//cout << (y - y1)*(x2 - x1) / (y2 - y1) + x1 << endl;
		return (y - y1)*(x2 - x1) / (y2 - y1) + x1;
	}
};


inline bool operator<(const gridPos& lhs, const gridPos& rhs) {
	if (lhs.x == rhs.x) {
		return lhs.y < rhs.y;
	}
	return lhs.x < rhs.x;
}

class World {
private:
	std::vector<Agent> agents;
	std::vector<ParkObject> parkObjects;
	SeLct *TheLCT;
	bool **grid;
	float x1, x2, y1, y2, stepSize;
	void updateKallmann();
	void updateAstar();
	gridPos posToGrid(cgra::vec2);
	cgra::vec2 gridToPos(gridPos);
	void setObstacleToGrid(std::vector<cgra::vec2>, bool);
	std::vector<cgra::vec2> aStarSearch(gridPos, gridPos, int);
public:
	void update();
	void addAgent(Agent);
	void removeAgent(int);
	void create_lct();
	void addObject(ParkObject);
	void removeObject(int);
	void init(float, float, float, float);
	void create_grid(float = 0.1f);
	vector<Agent>* getAgents();
	vector<ParkObject>* getParkObjects();
	static const bool useKallmann = true;
	static const bool useCollision = true;
	static const bool oneAgent = true;
	static const bool twoAgent = !true;
	static const bool threeAgent = !true;
	static const bool maze = !true;
	static const bool useBird = !false;
	static const bool stressTest = !true;
};
