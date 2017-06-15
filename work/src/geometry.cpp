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
//
//			Modified for COMP 308 Final Project by Jiaheng Wang
//
//----------------------------------------------------------------------------

#include <cmath>
#include <iostream> // input/output streams
#include <fstream>  // file streams
#include <sstream>  // string streams
#include <string>
#include <stdexcept>
#include <vector>

#include "cgra_math.hpp"
#include "geometry.hpp"

using namespace std;
using namespace cgra;

float* Geometry::getModel(string filename) {
	std::vector<cgra::vec3> m_points;	// Point list
	std::vector<cgra::vec2> m_uvs;		// Texture Coordinate list
	std::vector<cgra::vec3> m_normals;	// Normal list
	std::vector<triangle> m_triangles;	// Triangle/Face list


	// Make sure our geometry information is cleared
	m_points.clear();
	m_uvs.clear();
	m_normals.clear();
	m_triangles.clear();

	// Load dummy points because OBJ indexing starts at 1 not 0
	m_points.push_back(vec3(0, 0, 0));
	m_uvs.push_back(vec2(0, 0));
	m_normals.push_back(vec3(0, 0, 1));

	ifstream objFile(filename);

	if (!objFile.is_open()) {
		cerr << "Error reading " << filename << endl;
		throw runtime_error("Error :: could not open file.");
	}

	//cout << "Reading file " << filename << endl;

	// good() means that failbit, badbit and eofbit are all not set
	while (objFile.good()) {

		// Pull out line from file
		string line;
		std::getline(objFile, line);
		istringstream objLine(line);

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
			}
			else if (mode == "vn") {
				vec3 vn;
				objLine >> vn.x >> vn.y >> vn.z;
				m_normals.push_back(vn);

			}
			else if (mode == "vt") {
				vec2 vt;
				objLine >> vt.x >> vt.y;
				m_uvs.push_back(vt);

			}
			else if (mode == "f") {

				vector<vertex> verts;
				while (objLine.good()) {
					vertex v;
					objLine >> v.p;		// Scan in position index
					if (m_normals.size() > 1) {
						objLine.ignore(1);	// Ignore the '/' character
						if (m_uvs.size() > 1) {
							objLine >> v.t;		// Scan in uv (texture coord) index
						}
						objLine.ignore(1);	// Ignore the '/' character
						objLine >> v.n;		// Scan in normal index
					}
					verts.push_back(v);
				}

				// IFF we have 3 verticies, construct a triangle
				if (verts.size() == 4) {
					triangle tri;
					tri.v[0] = verts[0];
					tri.v[1] = verts[1];
					tri.v[2] = verts[2];
					m_triangles.push_back(tri);
				}
			}
		}
	}

	float *model = new float[m_triangles.size() * 4*8];
	int i = 0;
	for (triangle t : m_triangles) {
		for (vertex vert : t.v) {
			vec3 v = m_points[vert.p];
			*(model + (i++)) = v.x;
			*(model + (i++)) = v.y;
			*(model + (i++)) = v.z;
			vec3 n = m_normals[vert.n];
			*(model + (i++)) = n.x;
			*(model + (i++)) = n.y;
			*(model + (i++)) = n.z;
			vec2 t = m_uvs[vert.t];
			*(model + (i++)) = t.x;
			*(model + (i++)) = t.y;
		}
	}
	return model;
}