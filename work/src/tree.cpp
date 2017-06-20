/*
 * tree.cpp
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
#include "leaf.cpp"
#include "branch.cpp"

using namespace std;
using namespace cgra;


class Tree {
	public:bool finishedGrowing = false;
	float max_dist = 150;
	float min_dist = 10;
	public: vector<Branch>branches = vector<Branch>();
	public: vector<Leaf>leaves = vector<Leaf>();

public: Tree() {
	for (int i = 0; i < 2000; i++) {
		Leaf l = Leaf();
		leaves.push_back(l);
	}
	Branch *root = new Branch(vec3(0, 0, 0), vec3(0, 1, 0));
	branches.push_back(root);
	Branch *current = new Branch(root);

	while (!(closeEnough(current))) {
		Branch *trunk = new Branch(current);
		branches.push_back(trunk);
		current = trunk;
	}
}

public: bool closeEnough(Branch b) {

	for (Leaf l : leaves) {
		float d = length(l.position - b.position);
		if (d < max_dist) {
			return true;
		}
	}

	return false;
}

public: void grow() {
	for (int j = 0; j < leaves.size(); j++) {
		int closest = -1;
		vec3 closestDir = vec3(0, 0, 0); // a 'null' vector
		float record = -1;

		for (int i = 0; i < branches.size(); i++) {
			vec3 dir = leaves[j].position - branches[i].position;
			float d = length(dir);

			if (d < min_dist) {
				leaves[j].setReached();
				closest = -1;
				break;
			}
			else if (d > max_dist) {
			}
			else if (closest == -1 || d < record) {
				closest = i;
				closestDir = dir;
				record = d;
			}
		}
		if (closest > -1) {
			closestDir = normalize(closestDir);
			branches[closest].direction = branches[closest].direction + closestDir;
			branches[closest].count = branches[closest].count + 1;
		}
	}

	for (int i = leaves.size() - 1; i >= 0; i--) {
		if (leaves[i].reached == 1) {
			leaves.erase(leaves.begin() + i);
		}
		if(leaves.size() == 0){
			finishedGrowing = true;
			std::cout << "finished growing" << std::endl;
		}
	}
	for (int i = branches.size() - 1; i >= 0; i--) {

		if (branches[i].count > 0) {

			branches[i].direction = branches[i].direction / branches[i].count;

			vec3 ran = vec3::random();
			branches[i].direction += normalize(ran)*0.3f;
			branches[i].direction = normalize(branches[i].direction);
			Branch newB = new Branch(branches[i]);
			branches.push_back(newB);
			branches[i].reset();
		}
	}
}

public: void show() {
	for (Leaf l : leaves) {
		l.show();
	}
	for (int i = 0; i < branches.size(); i++) {
		Branch b = branches[i];
		if (b.parent != nullptr) {
			glPushMatrix();

			// ... Rotate branch
			vec3 dir = branches[i].parent->position - branches[i].position;
			vec3 target_dir = normalize( dir); // normalise the bones direction
			vec3 z_axis = vec3(0,0,1); // the default direction of a bone
			float rot_angle = acos((target_dir.x*z_axis.x) + (target_dir.y*z_axis.y) + (target_dir.z*z_axis.z)); //inverse of the dot product

			glTranslatef(branches[i].position.x, branches[i].position.y, branches[i].position.z);

			if( fabs(rot_angle) > 0) {
				vec3 cross = vec3((target_dir.y*z_axis.z)-(target_dir.z*z_axis.y), (target_dir.z*z_axis.x)-(target_dir.x*z_axis.z), (target_dir.x*z_axis.y)-(target_dir.y*z_axis.x)); // cross product
				vec3 rot_axis = normalize(cross); // normalise the cross product of the direction and staring vectors
				glRotatef( -rot_angle/math::pi()*180, rot_axis.x, rot_axis.y, rot_axis.z ); // rotate so the branch draws in right direction
			}
		//	renderCylinder(0.1, 0.1, branches[i].length, 6, 6, false);
			cgraCylinder(0.1,0.1, branches[i].length, 6, 6, false);
			glPopMatrix();
			
		}
	}
}
};
