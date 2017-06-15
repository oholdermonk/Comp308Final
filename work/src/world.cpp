/**
*
*         COMP 308 Final Project
*              Tri 1 2017
*     Victoria University of Wellington
*
*          Author: Jiaheng Wang
*
*/
#include "world.hpp"
#include "cgra_math.hpp"
#include <vector>
#include <map>

#define FLT_MAX          3.402823466e+38F        // max value

using namespace cgra;
using namespace std;

void World::render() {
	for (int i = 0; i < agents.size(); i++) {
		agents[i].render();
	}
	for (int i = 0; i < parkObjects.size(); i++) {
		parkObjects[i].render();
	}
}

void World::update() {
	clock_t begin = clock();
	useKallmann ? updateKallmann() : updateAstar();
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	//cout << "time elapsed: " << elapsed_secs << endl;
}

void World::updateKallmann() {
	for (int i = 0; i < agents.size(); i++) {

		vec2 pos = agents[i].getPosition();
		vec2 tar = agents[i].getTarget();

		if (length(pos - tar) > 0.05f) {
			float radius = agents[i].getRadius();
			TheLCT->remove_polygon(agents[i].getId());
			bool found = TheLCT->search_channel(pos.x, pos.y, tar.x, tar.y, radius);

			if (found) {
				GsPolygon CurPath;
				TheLCT->make_funnel_path(CurPath, agents[i].getRadius(), GS_TORAD(15));
				//printf ( "OK.\n" );
				GsPnt2 dir = (CurPath.get(1) - CurPath.get(0));
				dir.normalize();
				//	cout << dir.x << " " << dir.y << endl;
				agents[i].updateKallmann(vec2(dir.x, dir.y)*0.02f);
			}
			agents[i].setId(TheLCT->insert_polygon(agents[i].getOutline()));
		}
	}
}

void World::updateAstar() {
	for (int i = 0; i < agents.size(); i++) {

		if (agents[i].needMove()) {
			vec2 pos = agents[i].getPosition();
			vec2 nextPos = agents[i].getNextTarget();
			vec2 dir;
			if (length(nextPos - pos) == 0) {
				dir = vec2();
			}
			else {
				dir = normalize(nextPos - pos);
			}
			gridPos next = posToGrid(nextPos + dir*agents[i].getRadius());
			setObstacleToGrid(agents[i].getVertices(), false);
			if (agents[i].needPath() || grid[next.y][next.x] || !useCollision) {
				vec2 tar = agents[i].getTarget();
				float radius = agents[i].getRadius();
				agents[i].setPath(aStarSearch(posToGrid(pos), posToGrid(tar), (int)(radius / stepSize)));
			}
			nextPos = agents[i].getNextTarget();
			dir = nextPos - pos;
			//	cout << dir.x << " " << dir.y << endl;
			if (length(dir) == 0) {
				agents[i].setNeedPath();
			}
			else {
				agents[i].update(normalize(dir)*0.02f);
			}
			setObstacleToGrid(agents[i].getVertices(), true);
		}
	}
}

void World::addAgent(Agent a) {
	if (useKallmann) {
		a.setId(TheLCT->insert_polygon(a.getOutline()));
	}
	else {
		setObstacleToGrid(a.getVertices(), true);
	}
	agents.push_back(a);
}

void World::addObject(ParkObject po) {
	if (useKallmann) {
		po.setID(TheLCT->insert_polygon(po.getOutline()));
	}
	else {
		setObstacleToGrid(po.getVertices(), true);
	}
	parkObjects.push_back(po);
}

void World::init(float _x1, float _y1, float _x2, float _y2) {
	agents.clear();
	parkObjects.clear();
	x1 = _x1;
	x2 = _x2;
	y1 = _y1;
	y2 = _y2;
	useKallmann ? create_lct() : create_grid();

	Agent a1 = Agent(human);
	a1.setPosition(vec2(0, 5));
	a1.setTarget(vec2(0, -5));


	Agent a2 = Agent(human);
	a2.setPosition(vec2(-5, 0));
	a2.setTarget(vec2(5, 0));

	ParkObject p1 = ParkObject(bench, vec2(0, 0));
	p1.setRotation(math::pi() / 2);

	if (oneAgent) {
		addObject(p1);
		addAgent(a1);
	}
	else if (twoAgent) {
		addAgent(a1);
		addAgent(a2);
	}
	else if (threeAgent) {
		addAgent(a1);
		addAgent(a2);
		Agent a3 = Agent(human);
		a3.setPosition(vec2(5, 5));
		a3.setTarget(vec2(-5, -5));
		addAgent(a3);
		addObject(p1);
	}
	else if (maze) {
		addObject(ParkObject(bench, vec2(-7, -7.5)));
		addObject(ParkObject(bench, vec2(-7, -2.5)));
		addObject(ParkObject(bench, vec2(-7, 3)));
		addObject(ParkObject(bench, vec2(-7, 5)));
		addObject(ParkObject(bench, vec2(-4.9, 5)));
		addObject(ParkObject(bench, vec2(-4.9, 3)));
		addObject(ParkObject(bench, vec2(-4.9, -2.5)));
		addObject(ParkObject(bench, vec2(-4.9, -6.4)));
		Agent a5 = Agent(useBird ? bird : human);
		a5.setPosition(vec2(-9, 0));
		a5.setTarget(vec2(0, 0));
		addAgent(a5);
		addObject(ParkObject(bench, vec2(-3.5, 7)));
	}
	else if (stressTest) {
		addObject(ParkObject(bench, vec2(-7, -7.5)));
		addObject(ParkObject(bench, vec2(-7, -2.5)));
		addObject(ParkObject(bench, vec2(-7, 3)));
		addObject(ParkObject(bench, vec2(-7, 5)));
		addObject(ParkObject(bench, vec2(-4.9, 5)));
		addObject(ParkObject(bench, vec2(-4.9, 3)));
		addObject(ParkObject(bench, vec2(-4.9, -2.5)));
		addObject(ParkObject(bench, vec2(-4.9, -6.4)));
		Agent a5 = Agent(useBird ? bird : human);
		a5.setPosition(vec2(-9, 0));
		a5.setTarget(vec2(9, 0));
		addAgent(a5);
		addObject(ParkObject(bench, vec2(-3.5, 7)));
		addAgent(a1);
		//addAgent(a2);
		Agent a3 = Agent(human);
		a3.setPosition(vec2(5, 5));
		a3.setTarget(vec2(-9,-5));
		addAgent(a3);
		addObject(p1);
	}
}

void World::create_lct() {
	if (World::useKallmann) {
		TheLCT = new SeLct;
		GsPolygon poly;
		poly.push().set(x1, y1);
		poly.push().set(x1, y2);
		poly.push().set(x2, y2);
		poly.push().set(x2, y1);
		TheLCT->init(poly, 0.001f);

		for (int i = 0; i < parkObjects.size(); i++) {
			TheLCT->insert_polygon(parkObjects[i].getOutline());
		}
	}
}

void World::create_grid(float step) {
	stepSize = step;
	int x = (x2 - x1) / step;
	int y = (y2 - y1) / step;

	grid = new bool*[y];
	for (int i = 0; i < y; i++) {
		grid[i] = new bool[x];
	}

	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			grid[i][j] = (i == 0 || i == y - 1 || j == 0 || j == x - 1);
		}
	}
}

gridPos World::posToGrid(vec2 pos) {
	return gridPos((int)((pos.x - x1) / stepSize), (int)((pos.y - y1) / stepSize));
}

vec2 World::gridToPos(gridPos grid) {
	return vec2(
		(grid.x + 0.5)*stepSize + x1,
		(grid.y + 0.5)*stepSize + y1);
}

void World::setObstacleToGrid(vector<vec2> vertices, bool isObstacle) {
	vector<edge> edgePairs = vector<edge>(); // (x1,y1,x2,y2)
	int s = vertices.size() - 1;
	int e = 0;
	for (int i = 0; i < vertices.size(); i++) {
		gridPos gp1 = posToGrid(vertices[s]);
		gridPos gp2 = posToGrid(vertices[e]);
		edgePairs.push_back(edge(gp1, gp2));
		s = i;
		e = s + 1;
	}

	sort(edgePairs.begin(), edgePairs.end(), [](edge i, edge j) -> bool {return i.y1 < j.y1; });
	vector<edge> activeList = vector<edge>();
	int currentY = edgePairs[0].y1;
	while (edgePairs[0].y1 <= currentY) {
		activeList.push_back(edgePairs[0]);
		edgePairs.erase(edgePairs.begin());
	}

	while (!activeList.empty()) {
		sort(activeList.begin(), activeList.end(), [currentY](edge i, edge j)->bool {return i.xAt(currentY) < j.xAt(currentY); });
		for (int i = 0; i < activeList.size() - 1; i += 2) {
			int xstart = activeList[i].xAt(currentY);
			int xend = activeList[i + 1].xAt(currentY);
			for (int x = activeList[i].xAt(currentY); x < activeList[i + 1].xAt(currentY); x++) {
				if (x >= 0 && x < (x2 - x1) / stepSize && currentY >= 0 && currentY < (y2 - y1) / stepSize) {
					grid[currentY][x] = isObstacle;
				}
			}
		}
		currentY++;
		sort(activeList.begin(), activeList.end(), [](edge i, edge j)->bool {return i.y2 < j.y2; });
		while (!activeList.empty() && activeList[0].y2 <= currentY) {
			activeList.erase(activeList.begin());
		}
		while (!edgePairs.empty() && edgePairs[0].y1 <= currentY) {
			activeList.push_back(edgePairs[0]);
			edgePairs.erase(edgePairs.begin());
		}
	}
}


vector<vec2> World::aStarSearch(gridPos origin, gridPos target, int radius) {
	int ySize = (y2 - y1) / stepSize;
	int xSize = (x2 - x1) / stepSize;

	bool **closedSet = new bool*[ySize];
	for (int y = 0; y < ySize; y++) {
		bool *x = new bool[xSize];
		for (int i = 0; i < xSize; i++) {
			x[i] = grid[y][i];
		}
		closedSet[y] = x;
	}

	vector<gridPos> openSet;
	openSet.push_back(origin);

	map<gridPos, gridPos> cameFrom;

	float **gValue = new float*[ySize];
	for (int y = 0; y < ySize; y++) {
		float *x = new float[xSize];
		for (int i = 0; i < xSize; i++) {
			x[i] = FLT_MAX;
		}
		gValue[y] = x;
	}
	gValue[origin.y][origin.x] = 0;
	for (int i = 0; i < ySize; i++) {
		for (int j = 0; j < xSize; j++) {
			if (grid[i][j]) {
				for (int jl = j - radius < 0 ? 0 : j - radius; jl <= j + radius && jl < xSize; jl++) {
					for (int il = i - radius < 0 ? 0 : i - radius; il <= i + radius && il < ySize; il++) {
						closedSet[il][jl] = true;
					}
				}
			}
		}
	}

	while (!openSet.empty()) {
		gridPos current;
		int currentIndex;
		{
			float currentGValue = FLT_MAX;
			for (int i = 0; i < openSet.size(); i++) {
				gridPos node = openSet[i];
				if (gValue[node.y][node.x] < currentGValue) {
					current = node;
					currentIndex = i;
					currentGValue = gValue[(int)node.y][(int)node.x];
				}
			}
		}
		if (current == target) {
			gridPos prev;
			gridPos thisOne = current;
			vector<vec2> path;
			path.clear();
			path.push_back(gridToPos(current));
			while (true) {
				prev = cameFrom[thisOne];
				if (prev == origin) {
					for (int y = 0; y < ySize; y++) {
						free(closedSet[y]);
						free(gValue[y]);
					}
					free(closedSet);
					free(gValue);
					return path;
				}
				path.push_back(gridToPos(prev));
				thisOne = prev;
			}
		}

		openSet.erase(openSet.begin() + currentIndex);
		closedSet[current.y][current.x] = true;

		for (int y = current.y - 1; y <= current.y + 1 && y < ySize; y++) {
			if (y < 0) {
				y = 0;
			}
			for (int x = current.x - 1; x <= current.x + 1 && x < xSize; x++) {
				if (x < 0) {
					x = 0;
				}
				if (closedSet[y][x]) {
					continue;
				}

				gridPos neighbour; neighbour.x = x; neighbour.y = y;
				bool isNew = true;
				for (int i = 0; i < openSet.size(); i++) {
					if (openSet[i] == neighbour) {
						isNew = false;
						break;
					}
				}
				if (isNew) {
					openSet.push_back(neighbour);
				}
				float tentGValue = gValue[current.y][current.x] + current.dist(neighbour);
				if (tentGValue >= gValue[neighbour.y][neighbour.x]) {
					continue;
				}
				cameFrom[neighbour] = current;
				gValue[neighbour.y][neighbour.x] = tentGValue;
			}
		}
	}
	for (int y = 0; y < ySize; y++) {
		free(closedSet[y]);
		free(gValue[y]);
	}
	free(closedSet);
	free(gValue);
	return vector<vec2>();
}

vector<Agent> World::getAgents() {
	return agents;
}

vector<ParkObject> World::getParkObjects() {
	return parkObjects;
}