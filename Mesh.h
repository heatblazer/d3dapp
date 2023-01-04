#pragma once

#include "iUtils.h"
#include "utils.h"
#include "Vector.h"
#include "String.h"

class Mesh
{
	struct {
		int vcount;
		int iedges;
		int faces;
		Vector<float> m_verts;
		Vector<float> m_norms;
		Vector<int> m_edges;
	} m_data;

public:

	Mesh() = default;

	~Mesh() = default; 

	void Load(const char* fname);
	
	void Normalize();

	float* norms()  { return m_data.m_norms.data(); }

	int Vertices() const { return m_data.m_verts.Count();  }

	int Edges() const { return m_data.m_edges.Count();  }

	float* vdata() { return m_data.m_verts.data();  }

	int* edata() { return m_data.m_edges.data();  }

	void scale(float x, float y, float z);


};

