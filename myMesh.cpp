#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <vector>
#include <GL/glew.h>
#include "myVector3D.h"
#include "myPoint3D.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	// vector<int> faceids;
	// myHalfedge **hedges;
	// map<pair<int, int>, myHalfedge *>::iterator it;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;

	while (getline(fin, s))
	{
		stringstream myline(s);
		if (!(myline >> t))
			continue;

		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			myVertex *vx = new myVertex();
			vx->point = new myPoint3D(x, y, z);
			vertices.push_back(vx);
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			vector<int> faceids;
			while (myline >> u)
			{
				string tok = u.substr(0, u.find("/"));
				int objIdx = atoi(tok.c_str());
				faceids.push_back(objIdx - 1);
			}
			int n = (int)faceids.size();

			myFace *face = new myFace();
			faces.push_back(face);

			vector<myHalfedge *> he(n);
			for (int k = 0; k < n; k++)
			{
				myHalfedge *h = new myHalfedge();
				halfedges.push_back(h);
				he[k] = h;

				int i = faceids[k];
				int j = faceids[(k + 1) % n];

				h->source = vertices[i];
				h->adjacent_face = face;
				if (vertices[i]->originof == NULL)
					vertices[i]->originof = h;

				pair<int, int> forward(i, j);
				pair<int, int> backward(j, i);
				if (twin_map.count(backward))
				{
					myHalfedge *tw = twin_map[backward];
					h->twin = tw;
					tw->twin = h;
				}
				twin_map[forward] = h;
			}
			for (int k = 0; k < n; k++)
			{
				he[k]->next = he[(k + 1) % n];
				he[k]->prev = he[(k - 1 + n) % n];
			}
			face->adjacent_halfedge = he[0];
		}
	}

	checkMesh();
	normalize();
	return true;
}








void myMesh::computeNormals()
{
	for (unsigned int i = 0; i < faces.size(); i++)
		faces[i]->computeNormal();
	for (unsigned int i = 0; i < vertices.size(); i++)
		vertices[i]->computeNormal();
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}

void myMesh::simplify()
{
	/**** TODO ****/
}

void myMesh::simplify(myVertex *)
{
	/**** TODO ****/
}

void myMesh::triangulate()
{
	std::vector<myFace *> snapshot = faces;
	for (unsigned int i = 0; i < snapshot.size(); i++)
		triangulate(snapshot[i]);
}

// return false if already triangle, true otherwise.
bool myMesh::triangulate(myFace *f)
{
	if (f == NULL || f->adjacent_halfedge == NULL)
		return false;

	myHalfedge *start = f->adjacent_halfedge;
	vector<myHalfedge *> bdry;
	myHalfedge *walk = start;
	do {
		bdry.push_back(walk);
		walk = walk->next;
	} while (walk != start);

	int n = (int)bdry.size();
	if (n <= 3)
		return false;

	vector<myVertex *> vlist(n);
	vector<myHalfedge *> nbrTwin(n);
	for (int i = 0; i < n; i++) {
		vlist[i] = bdry[i]->source;
		nbrTwin[i] = bdry[i]->twin;
		if (nbrTwin[i] != NULL)
			nbrTwin[i]->twin = NULL;
	}

	for (int i = 0; i < n; i++) {
		for (unsigned int j = 0; j < halfedges.size(); j++) {
			if (halfedges[j] == bdry[i]) {
				halfedges.erase(halfedges.begin() + j);
				break;
			}
		}
		delete bdry[i];
	}

	for (unsigned int j = 0; j < faces.size(); j++) {
		if (faces[j] == f) {
			faces.erase(faces.begin() + j);
			break;
		}
	}
	delete f;

	double cx = 0.0, cy = 0.0, cz = 0.0;
	for (int i = 0; i < n; i++) {
		cx += vlist[i]->point->X;
		cy += vlist[i]->point->Y;
		cz += vlist[i]->point->Z;
	}
	cx /= n;
	cy /= n;
	cz /= n;

	myVertex *C = new myVertex();
	C->point = new myPoint3D(cx, cy, cz);
	vertices.push_back(C);

	vector<myHalfedge *> h0n(n), h1n(n), h2n(n);

	for (int i = 0; i < n; i++) {
		myFace *nf = new myFace();
		faces.push_back(nf);

		myHalfedge *h0 = new myHalfedge();
		myHalfedge *h1 = new myHalfedge();
		myHalfedge *h2 = new myHalfedge();
		halfedges.push_back(h0);
		halfedges.push_back(h1);
		halfedges.push_back(h2);

		myVertex *Vi = vlist[i];
		myVertex *Vj = vlist[(i + 1) % n];

		h0->source = C;
		h1->source = Vi;
		h2->source = Vj;

		h0->adjacent_face = nf;
		h1->adjacent_face = nf;
		h2->adjacent_face = nf;

		h0->next = h1;
		h1->next = h2;
		h2->next = h0;
		h0->prev = h2;
		h1->prev = h0;
		h2->prev = h1;

		nf->adjacent_halfedge = h0;

		if (nbrTwin[i] != NULL) {
			h1->twin = nbrTwin[i];
			nbrTwin[i]->twin = h1;
		}

		h0n[i] = h0;
		h1n[i] = h1;
		h2n[i] = h2;
	}

	for (int i = 0; i < n; i++) {
		int prev = (i - 1 + n) % n;
		h0n[i]->twin = h2n[prev];
		h2n[prev]->twin = h0n[i];
	}

	C->originof = h0n[0];
	for (int i = 0; i < n; i++)
		vlist[i]->originof = h1n[i];

	return true;
}

