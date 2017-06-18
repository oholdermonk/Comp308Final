//---------------------------------------------------------------------------
//
// Copyright (c) 2016 Taehyun Rhee, Joshua Scott, Ben Allen
//
// This software is provided 'as-is' for assignment of COMP308 in ECS,
// Victoria University of Wellington, without any express or implied warranty. 
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>

//#include "cgra_cow.hpp"
#include "cgra_math.hpp"
#include "mesh.hpp"
#include "opengl.hpp"

using namespace std;
using namespace cgra;


mesh::mesh(string filename) {
	m_filename = filename;
	std::string pre = "./work/res/assets/";
	cout << filename << endl;
	if(filename==pre+"table.obj"){
		cout << "filename" << endl;
		m_texName="wood.jpg";
	} else if (filename==pre+"teapot.obj"){
		m_texName="cubeMap.jpg";
	} else if (filename==pre+"torus.obj"){
		m_texName="normalMap.jpg";
	} else if (filename ==pre+"box.obj"){
		m_texName="brick.jpg";
	}
	readOBJ(filename);
	//cout << m_triangles.size() << endl;
	// if (m_triangles.size() > 0) {
	// 	//cout << m_triangles.size() << endl;
	// 	createDisplayListPoly();
	// 	createDisplayListWire();
	// }
}


mesh::~mesh() { }

bool mesh::exists (const std::string& name) {
	ifstream f(name.c_str());
	return f.good();
}

void mesh::readOBJ(string filename) {

	// Make sure our mesh information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back(vec3(0,0,0));
	m_uvs.push_back(vec2(0,0));
	m_normals.push_back(vec3(0,0,1));


	if(exists(filename)){


	
		ifstream objFile(filename);

		if(!objFile.is_open()) {
			cerr << "Error reading " << filename << endl;
			throw runtime_error("Error :: could not open file.");
		}

		cout << "Reading file " << filename << endl;
		
		// good() means that failbit, badbit and eofbit are all not set
		while(objFile.good()) {

			// Pull out line from file
			string line;
			std::getline(objFile, line);
			istringstream objLine(line);
			//cout << line << endl;
			// Pull out mode from line
			string mode;
			objLine >> mode;

			// Reading like this means whitespace at the start of the line is fine
			// attempting to read from an empty string/line will set the failbit
			if (!objLine.fail()) {

				if (mode == "v") {
					vec3 v;
					objLine >> v.x >> v.y >> v.z;
					m_points.push_back(v);

				} else if(mode == "vn") {
					vec3 vn;
					objLine >> vn.x >> vn.y >> vn.z;
					m_normals.push_back(vn);

				} else if(mode == "vt") {
					vec2 vt;
					objLine >> vt.x >> vt.y;

					m_uvs.push_back(vt);


				}  else if(mode == "s") {
					vec2 vt;
					objLine.ignore();
					
					


				} else if(mode == "f") {
					
					vector<vertex> verts;
					while (objLine.good()){
						vertex v;
						
						
						//-------------------------------------------------------------
						// [Assignment 1] :
						// Modify the following to parse the bunny.obj. It has no uv
						// coordinates so each vertex for each face is in the format
						// v//vn instead of the usual v/vt/vn.
						//
						// Modify the following to parse the dragon.obj. It has no
						// normals or uv coordinates so the format for each vertex is
						// v instead of v/vt/vn or v//vn.
						//
						// Hint : Check if there is more than one uv or normal in
						// the uv or normal vector and then parse appropriately.
						//-------------------------------------------------------------
						
						if(m_uvs.size()<=1 && m_normals.size()<=1){
							objLine >> v.p;		// Scan in position index
							
						} else if(m_uvs.size()<=1){
							objLine >> v.p;		// Scan in position index
							objLine.ignore(1);	// Ignore the '/' character
							objLine.ignore(1);	// Ignore the '/' character
							objLine >> v.n;		// Scan in normal index

						} else {

							// Assignment code (assumes you have all of v/vt/vn for each vertex)
							objLine >> v.p;		// Scan in position index
							objLine.ignore(1);	// Ignore the '/' character
							objLine >> v.t;		// Scan in uv (texture coord) index
							objLine.ignore(1);	// Ignore the '/' character
							objLine >> v.n;		// Scan in normal index
						}
						//cout << "are we making verts?" << endl;

						verts.push_back(v);
					}
					//cout << "are we reading f?" << verts.size() << endl;
					// IF we have 3 verticies, construct a triangle
					if(verts.size() >= 3){
						triangle tri;
						tri.v[0] = verts[0];
						tri.v[1] = verts[1];
						tri.v[2] = verts[2];
						tri.v[3] = verts[3];
						m_triangles.push_back(tri);
						//cout << "making a triangle?" << endl;

					}
				}
			}
		}

		cout << "Reading OBJ file is DONE." << endl;
		cout << m_points.size()-1 << " points" << endl;
		cout << m_uvs.size()-1 << " uv coords" << endl;
		cout << m_normals.size()-1 << " normals" << endl;
		cout << m_triangles.size() << " faces" << endl;


		// If we didn't have any normals, create them
		if (m_normals.size() <= 1) createNormals();
		//cout << "done?" << endl;
	} else {
		cerr << "invalid filename" << endl;
	}
}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to populate the normals for 
// the model currently loaded. Compute per face normals
// first and get that working before moving onto calculating
// per vertex normals.
//-------------------------------------------------------------
void mesh::createNormals() {
	for(int cur = 1; cur< m_triangles.size(); cur++){
		m_normals.push_back(vec3(0,0,0));

	}
	int vertNum = 1;
	




	for(int a = 1; a<m_triangles.size();a++){
		
		if(m_triangles[a].v[0].n==0){
			m_triangles[a].v[0].n=vertNum++;
		}
		if(m_triangles[a].v[1].n==0){
			m_triangles[a].v[1].n=vertNum++;
		}
		if(m_triangles[a].v[2].n==0){
			m_triangles[a].v[2].n=vertNum++;
		}



		vec3 vert1=m_points[m_triangles[a].v[0].p];
		
		vec3 vert2=m_points[m_triangles[a].v[1].p];
		
		vec3 vert3=m_points[m_triangles[a].v[2].p];
		

		vec3 norm = cross(vert2-vert1,vert3-vert1);

		// m_triangles[a].v[0].n = m_triangles[a].v[0].p;
		// m_triangles[a].v[1].n = m_triangles[a].v[1].p;
		// m_triangles[a].v[2].n = m_triangles[a].v[2].p;


		m_normals[m_triangles[a].v[0].n] = (m_normals[m_triangles[a].v[0].n]) +norm;
		m_normals[m_triangles[a].v[1].n] = (m_normals[m_triangles[a].v[1].n]) +norm;
		m_normals[m_triangles[a].v[2].n] = (m_normals[m_triangles[a].v[2].n]) +norm;



	}


	cout<< m_normals.size() << endl;

	for(int cur = 0; cur< m_normals.size(); cur++){
		//cout << m_normals[cur] << endl;
		if(length(m_normals[cur])!=0){
			m_normals[cur] = normalize(m_normals[cur]);
			//cout << "DID A THING" << endl;
		} 

		
	}


}


//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as polygon model
//-------------------------------------------------------------
void mesh::createDisplayListPoly() {
	// Delete old list if there is one
	if (m_displayListPoly) glDeleteLists(m_displayListPoly, 1);

	// Create a new list
	cout << "Creating Poly mesh" << endl;
	m_displayListPoly = glGenLists(1);
	glNewList(m_displayListPoly, GL_COMPILE);

	glBegin(GL_TRIANGLES);

		
	for(triangle t : m_triangles){

		for(vertex tempV : t.v){
			int pointID = tempV.p;
			int normalID = tempV.n;
			int texID = tempV.t;
			vec2 tex = m_uvs[texID];
			vec3 point = m_points[pointID];
			vec3 normal = m_normals[normalID];

			if(m_texName=="brick.jpg"){
				glNormal3f(normal.x, normal.y, normal.z);
				glTexCoord2f(tex.x*4,tex.y*4); 
				glVertex3f(point.x, point.y, point.z); 
			} else {
				glNormal3f(normal.x, normal.y, normal.z);
				glTexCoord2f(tex.x,tex.y); 
				glVertex3f(point.x, point.y, point.z); 
			}

			
			
		}
		

	
	}

	glEnd();
	glEndList();

	// for(triangle t : m_triangles){

	// 	for(vertex vert : t.v){
	// 		int normal = vert.n;
	// 		int pointID = vert.p;
	// 		//cout << m_normals[normal] << endl <<endl;
	// 	}
	// }

	cout << "Finished creating Poly mesh" << endl;
}



//-------------------------------------------------------------
// [Assignment 1] :
// Fill the following function to create display list
// of the obj file to show it as wireframe model
//-------------------------------------------------------------
void mesh::createDisplayListWire() {
	// Delete old list if there is one
	if (m_displayListWire) glDeleteLists(m_displayListWire, 1);

	// Create a new list
	cout << "Creating Wire mesh" << endl;
	m_displayListWire = glGenLists(1);
	glNewList(m_displayListWire, GL_COMPILE);



	for(triangle t : m_triangles){
	glBegin(GL_QUADS);


		for(vertex tempV : t.v){
			int pointID = tempV.p;
			int normalID = tempV.n;
			vec3 point = m_points[pointID];
			vec3 normal = m_normals[normalID];

			glNormal3f(normal.x, normal.y, normal.z); glVertex3f(point.x, point.y, point.z); 

		}

		
	glEnd();

	
	}

	glEndList();


	cout << "Finished creating Wire mesh" << endl;
}


void mesh::rendermesh(cgra::vec3 trans, int rot) {
	glMatrixMode(GL_MODELVIEW);
	
	//glRotatef(rotateYAmt, 0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslatef(trans.x,trans.y,trans.z);
	glRotatef(rot,0,1,0);

	//glShadeModel(GL_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glCallList(m_displayListPoly);
	glPopMatrix();

	
}

void mesh::rotateModel() {
	
	rotateYAmt = rotateYAmt +15;
	if(rotateYAmt>=360){
		rotateYAmt=0;
	}

}

void mesh::rotateModel(double x, double y) {
	rotateYAmt = rotateYAmt +x;
}


void mesh::toggleWireFrame() {
	m_wireFrameOn = !m_wireFrameOn;
}