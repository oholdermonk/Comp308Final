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

#pragma once

#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "cgra_math.hpp"
#include "opengl.hpp"


struct vertex {
	int p = 0; // index for point in m_points
	int t = 0; // index for uv in m_uvs
	int n = 0; // index for normal in m_normals
};

struct triangle {
	vertex v[3]; //requires 3 verticies
};

class mesh {
private:

	// Feilds for storing raw obj information
	

	bool m_wireFrameOn = false;
	double rotateYAmt = 0.0;

	// IDs for the display list to render
	GLuint m_displayListPoly = 0; // DisplayList for Polygon
	GLuint m_displayListWire = 0; // DisplayList for Wireframe

	void readOBJ(std::string);

	void createNormals();

	void createDisplayListPoly();
	void createDisplayListWire();


public:
	std::string m_texName;

	std::string m_filename;
	std::vector<cgra::vec3> m_points;	// Point list
	std::vector<cgra::vec2> m_uvs;		// Texture Coordinate list
	std::vector<cgra::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;	// Triangle/Face list

	mesh(std::string);
	~mesh();
	bool exists (const std::string&); 
	void rendermesh(cgra::vec3, int);
	void toggleWireFrame();
	void rotateModel();
	void rotateModel(double, double);
	
};