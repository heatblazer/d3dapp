#include "Mesh.h"

void Mesh::Load(const char* fname)
{
	String txt{ilian::readfile(fname).c_str()};
	Vector<String> vs;
	viki::ssplit<String>(txt.data(), "\n", vs);
	if (txt.Length() > 1) {
		Vector<String> t;
		viki::ssplit<String>(vs[0].data(), " ", t);
		m_data.vcount = ilian::alphatoint(t[1].data());
		m_data.iedges = ilian::alphatoint(t[2].data());
		m_data.faces = ilian::alphatoint(t[3].data());
	}

	for (int i = 1; i < vs.Count(); i++) //skip first line
	{
		Vector<String> t;
		const char* temp = vs[i].data();
		viki::ssplit<String>(temp, " ", t);
		if (t.Count() == 3) { //vertex data
			m_data.m_verts.PushBack((float)ilian::alphatof(t[0].data()));
			m_data.m_verts.PushBack((float)ilian::alphatof(t[1].data()));
			m_data.m_verts.PushBack((float)ilian::alphatof(t[2].data()));
			m_data.m_verts.PushBack(0.0f);
			m_data.m_verts.PushBack(0.0f);
			m_data.m_verts.PushBack(1.0f);

		}
		else if (t.Count() == 4) { // index data and omit first one
			m_data.m_edges.PushBack(ilian::alphatoint(t[1].data()));
			m_data.m_edges.PushBack(ilian::alphatoint(t[2].data()));
			m_data.m_edges.PushBack(ilian::alphatoint(t[3].data()));
		}
	}	
}

void Mesh::Normalize()
{
	float p = 0.0f;
	for (int i = 0; i < m_data.m_verts.Count(); i++)
	{
		p += m_data.m_verts[i] * m_data.m_verts[i];
	}

	p = ::sqrt(p);

	for (int i = 0; i < m_data.m_verts.Count(); i++)
	{
		m_data.m_norms.PushBack(m_data.m_verts[i] / p);
	}

}
