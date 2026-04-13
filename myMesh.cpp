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


void myMesh::triangulateBary()
{
	vector<myFace *> snapshot = faces;
	for (unsigned int i = 0; i < snapshot.size(); i++)
		triangulateBary(snapshot[i]);
}

void myMesh::triangulateBary(myFace *f)
{
	if (f == NULL || f->adjacent_halfedge == NULL) return;

	vector<myHalfedge *> bdry;
	myHalfedge *walk = f->adjacent_halfedge;
	do { bdry.push_back(walk); walk = walk->next; } while (walk != f->adjacent_halfedge);
	int n = (int)bdry.size();

	myVertex *center = new myVertex();
	center->point = new myPoint3D(0, 0, 0);
	for (int k = 0; k < n; k++) {
		center->point->X += bdry[k]->source->point->X;
		center->point->Y += bdry[k]->source->point->Y;
		center->point->Z += bdry[k]->source->point->Z;
	}
	center->point->X /= n;
	center->point->Y /= n;
	center->point->Z /= n;
	vertices.push_back(center);

	for (unsigned int j = 0; j < faces.size(); j++)
		if (faces[j] == f) { faces.erase(faces.begin() + j); break; }

	// spokes between center and every edge's source vertex
	vector<myHalfedge *> spoke_out(n), spoke_in(n);
	for (int k = 0; k < n; k++) {
		spoke_out[k] = new myHalfedge();
		spoke_in[k]  = new myHalfedge();
		halfedges.push_back(spoke_out[k]);
		halfedges.push_back(spoke_in[k]);
		spoke_out[k]->source = center;
		spoke_in[k]->source  = bdry[k]->source;
		spoke_out[k]->twin   = spoke_in[k];
		spoke_in[k]->twin    = spoke_out[k];
	}
	center->originof = spoke_out[0];

	for (int k = 0; k < n; k++) {
		myFace *nf = new myFace();
		faces.push_back(nf);
		myHalfedge *e0 = bdry[k];
		myHalfedge *e1 = spoke_in[(k + 1) % n];
		myHalfedge *e2 = spoke_out[k];
		e0->next = e1; e1->next = e2; e2->next = e0;
		e0->prev = e2; e1->prev = e0; e2->prev = e1;
		e0->adjacent_face = e1->adjacent_face = e2->adjacent_face = nf;
		nf->adjacent_halfedge = e0;
	}
	delete f;
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

	for (unsigned int j = 0; j < faces.size(); j++) {
		if (faces[j] == f) {
			faces.erase(faces.begin() + j);
			break;
		}
	}

	vector<myHalfedge *> diag_fwd(n - 3), diag_bwd(n - 3);
	for (int k = 0; k < n - 3; k++) {
		diag_fwd[k] = new myHalfedge();
		diag_bwd[k] = new myHalfedge();
		halfedges.push_back(diag_fwd[k]);
		halfedges.push_back(diag_bwd[k]);
		diag_fwd[k]->source = bdry[k + 2]->source;
		diag_bwd[k]->source = bdry[0]->source;
		diag_fwd[k]->twin = diag_bwd[k];
		diag_bwd[k]->twin = diag_fwd[k];
	}

	for (int k = 0; k < n - 2; k++) {
		myFace *nf = new myFace();
		faces.push_back(nf);

		myHalfedge *e0, *e1, *e2;

		if (k == 0) {
			e0 = bdry[0];
			e1 = bdry[1];
			e2 = diag_fwd[0];
		} else if (k == n - 3) {
			e0 = diag_bwd[k - 1];
			e1 = bdry[k + 1];
			e2 = bdry[n - 1];
		} else {
			e0 = diag_bwd[k - 1];
			e1 = bdry[k + 1];
			e2 = diag_fwd[k];
		}

		e0->next = e1;
		e1->next = e2;
		e2->next = e0;
		e0->prev = e2;
		e1->prev = e0;
		e2->prev = e1;

		e0->adjacent_face = nf;
		e1->adjacent_face = nf;
		e2->adjacent_face = nf;

		nf->adjacent_halfedge = e0;
	}

	bdry[0]->source->originof = bdry[0];

	delete f;
	return true;
}

