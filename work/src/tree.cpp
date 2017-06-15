/*
 * tree.cpp
 *
 *  Created on: 15/06/2017
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
//#include "simple_shader.hpp"
#include "opengl.hpp"

using namespace std;
using namespace cgra;



class Tree{

	int screenWidth = 600;
	int screenHeight = 600;

	public:vector<GLfloat>xPositions;
	private:vector<GLfloat>yPositions;
	private:vector<GLfloat>zPositions;
	private:vector<GLfloat>sizes;
	private:vector<vec3>angles; // the direction vector of the branches

	private:int rootXPos;
	private:int rootYPos;
	private:int rootZPos;

	public:Tree(int rootX, int rootY, int rootZ){
		rootXPos = rootX;
		rootYPos = rootY;
		rootZPos = rootZ;
		createTree();
	}

	public:void createTree(){
		// clear all the vectors to ensure they are clear();
		xPositions.clear();
		yPositions.clear();
		zPositions.clear();
		sizes.clear();
		angles.clear();

		createTree(screenWidth/2, screenHeight - 10, -20, 9 , vec3(0,1,0)); // size = (rand() % screenHeight/4) + 40
	}

	public:void createTree(float xPos, float yPos, float zPos, float size, vec3 angle){

		if(size < 3 || yPos - size < 10){
			//std::cout << "Return in createTree" << std::endl;
			return;
		}
		static const int shrinkMax = 900;
		static const int shrinkMin = 1;
		static const int maxAngle = 45;
		static const int maxBranches = 8;
		static const int minBranches = 0;


		vec3 targetDir = normalize(angle); // normalize the angle of the branch
		vec3 axis = vec3(0,0,1); // the branch drawn if straight up
		float rotAngle = acos((targetDir.x*axis.x) + (targetDir.y*axis.y) + (targetDir.z*axis.z)); //inverse of the dot product

		vec3 curBranch = targetDir * size;
		vec3 newPosVector = vec3(xPos, yPos, zPos) + curBranch;
		GLfloat newXPos = newPosVector.x;
		GLfloat newYPos = newPosVector.y;
		GLfloat newZPos = newPosVector.z;

		xPositions.push_back(xPos);
		yPositions.push_back(yPos);
		zPositions.push_back(zPos);
		sizes.push_back(size);
		angles.push_back(angle);


		//int numOfBranches = (rand() % (maxBranches - minBranches)) - minBranches + 1; // calculates the number of branches and ensures there is at least one
		int numOfBranches = 4;
		for(int i = 0; i < numOfBranches; i++){
			// create new size and angle
			vec3 newBranchAngle =  vec3(rand() % 45, rand() % 45, rand() % 45); // find a way to make this controlled random
			int newSize = size -1;//size * (rand() % shrinkMax + shrinkMin) / 1000.0f;
			//recurse
			createTree(newXPos, newYPos, newZPos, newSize, newBranchAngle);
		}
		//std::cout << "The create tree method ended" << std::endl;
	}

	public:void drawTree(int branches){
		//std::cout << "The draw method started" << std::endl;
		if(branches > xPositions.size()){
			std::cout << "There were more branches the points";
			return;
		}
		for(int i = 0; i < branches; i++){
			glColor3f(255,255,255);
			int branchWidth =  4;
			float size = sizes[i]/10;
			glPushMatrix();
			glTranslatef(xPositions[i]/10, yPositions[i]/10, zPositions[i]/10);
			vec3 targetDir = normalize(angles[i]); // normalize the angle of the branch
					vec3 xAxis = vec3(1,0,0); // the branch drawn if straight up
					float rotAngle = acos((targetDir.x*xAxis.x) + (targetDir.y*xAxis.y) + (targetDir.z*xAxis.z)); //inverse of the dot product
					if( fabs(rotAngle) > 0)
							{
								vec3 cross = vec3((targetDir.y*xAxis.z)-(targetDir.z*xAxis.y), (targetDir.z*xAxis.x)-(targetDir.x*xAxis.z), (targetDir.x*xAxis.y)-(targetDir.y*xAxis.x)); // cross product
							    vec3 rotAxis = normalize(cross); // normalise the cross product of the direction and staring vectors
							    glRotatef( -rotAngle/M_PI*180, rotAxis.x, rotAxis.y, rotAxis.z ); // rotate so the bone drawsin right direction
							}
					cgraSphere(1,10,10,false);
			//glRotate(angles[i])
//					std::cout << "_____________________" << std::endl;
//					std::cout << branchWidth << std::endl;
//				//	std::cout << size << std::cout;
//					std::cout << "_____________________" << std::endl;
			//cgraCylinder(0.1, 0.1, size, 10, 10, false);
		//	std::cout << "should have drawn cylinder" << std::endl;
			glPopMatrix();
		}
		//std::cout << "The tree finished the draw method" << std::endl;

		for(int i = 0; i < branches; i++){
			glPushMatrix();
			std::cout << "----------------------------" << std::endl;
			std::cout << xPositions[i] << std::endl;
			std::cout << yPositions[i] << std::endl;
			std::cout << zPositions[i] << std::endl;
			std::cout << "----------------------------" << std::endl;

			glTranslatef(xPositions[i], yPositions[i], zPositions[i]);
			cgraSphere(10,10,10,false);
			glPopMatrix();

		}
	}

	public:void draw(){
		glTranslatef(rootXPos, rootYPos, rootZPos);
		cgraCylinder(1,1,8,10,10,false);
	}

};



