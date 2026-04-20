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
    if (f == NULL) return;
    if (f->adjacent_halfedge == NULL) return;

    vector<myHalfedge*> border;
    myHalfedge* current = f->adjacent_halfedge;

    do {
        border.push_back(current);
        current = current->next;
    } while (current != f->adjacent_halfedge);

    int n = (int)border.size();

    myVertex* center = new myVertex();
    center->point = new myPoint3D(0, 0, 0);

    for (int i = 0; i < n; i++) {
        center->point->X += border[i]->source->point->X;
        center->point->Y += border[i]->source->point->Y;
        center->point->Z += border[i]->source->point->Z;
    }

    center->point->X /= n;
    center->point->Y /= n;
    center->point->Z /= n;

    vertices.push_back(center);

    for (unsigned int i = 0; i < faces.size(); i++) {
        if (faces[i] == f) {
            faces.erase(faces.begin() + i);
            break;
        }
    }

    vector<myHalfedge*> edgeFromCenter(n);
    vector<myHalfedge*> edgeToCenter(n);

    for (int i = 0; i < n; i++) {
        edgeFromCenter[i] = new myHalfedge();
        edgeToCenter[i] = new myHalfedge();

        halfedges.push_back(edgeFromCenter[i]);
        halfedges.push_back(edgeToCenter[i]);

        edgeFromCenter[i]->source = center;
        edgeToCenter[i]->source = border[i]->source;

        edgeFromCenter[i]->twin = edgeToCenter[i];
        edgeToCenter[i]->twin = edgeFromCenter[i];
    }

    center->originof = edgeFromCenter[0];

    for (int i = 0; i < n; i++) {
        myFace* newFace = new myFace();
        faces.push_back(newFace);

        myHalfedge* h0 = border[i];
        myHalfedge* h1 = edgeToCenter[(i + 1) % n];
        myHalfedge* h2 = edgeFromCenter[i];

        h0->next = h1;
        h1->next = h2;
        h2->next = h0;

        h0->prev = h2;
        h1->prev = h0;
        h2->prev = h1;

        h0->adjacent_face = newFace;
        h1->adjacent_face = newFace;
        h2->adjacent_face = newFace;

        newFace->adjacent_halfedge = h0;
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

void myMesh::triangulateEarClipping()
{
	vector<myFace *> snapshot = faces;
	for (unsigned int i = 0; i < snapshot.size(); i++)
		triangulateEarClipping(snapshot[i]);
}

void myMesh::triangulateEarClipping(myFace *f)
{
	const double eps = 1e-10;

	if (f == NULL || f->adjacent_halfedge == NULL) return;

	int n = 0;
	myHalfedge *start = f->adjacent_halfedge;
	myHalfedge *e = start;
	do {
		n++;
		e = e->next;
	} while (e != start);

	if (n <= 3) return;

	vector<myHalfedge *> hedges;
	vector<myVertex *> verts;

	e = start;
	do {
		hedges.push_back(e);
		verts.push_back(e->source);
		e = e->next;
	} while (e != start);

	myVector3D faceN(0, 0, 0);
	for (int i = 0; i < n; i++) {
		myPoint3D *a = verts[i]->point;
		myPoint3D *b = verts[(i + 1) % n]->point;

		faceN.dX += (a->Y - b->Y) * (a->Z + b->Z);
		faceN.dY += (a->Z - b->Z) * (a->X + b->X);
		faceN.dZ += (a->X - b->X) * (a->Y + b->Y);
	}

	if (faceN.length() < eps) return;

	vector<int> nextIndex(n);
	vector<int> prevIndex(n);

	for (int i = 0; i < n; i++) {
		nextIndex[i] = (i + 1) % n;
		prevIndex[i] = (i - 1 + n) % n;
	}

	int remaining = n;
	int current = 0;

	while (remaining > 3) {
		bool earFound = false;
		int first = current;

		do {
			int p = prevIndex[current];
			int nx = nextIndex[current];

			myVector3D v1 = *verts[current]->point - *verts[p]->point;
			myVector3D v2 = *verts[nx]->point - *verts[current]->point;

			if (v1.crossproduct(v2) * faceN > 0) {
				bool isEar = true;

				int test = nextIndex[nx];
				while (test != p) {
					myVector3D c0 = (*verts[current]->point - *verts[p]->point).crossproduct(*verts[test]->point - *verts[p]->point);
					myVector3D c1 = (*verts[nx]->point - *verts[current]->point).crossproduct(*verts[test]->point - *verts[current]->point);
					myVector3D c2 = (*verts[p]->point - *verts[nx]->point).crossproduct(*verts[test]->point - *verts[nx]->point);

					if (c0 * faceN > eps && c1 * faceN > eps && c2 * faceN > eps) {
						isEar = false;
						break;
					}

					test = nextIndex[test];
				}

				if (isEar) {
					myHalfedge *diagIn = new myHalfedge();
					myHalfedge *diagOut = new myHalfedge();

					diagIn->source = verts[nx];
					diagOut->source = verts[p];

					diagIn->twin = diagOut;
					diagOut->twin = diagIn;

					halfedges.push_back(diagIn);
					halfedges.push_back(diagOut);

					myFace *newFace = new myFace();
					faces.push_back(newFace);
					newFace->adjacent_halfedge = hedges[p];

					hedges[p]->next = hedges[current];
					hedges[current]->next = diagIn;
					diagIn->next = hedges[p];

					hedges[p]->prev = diagIn;
					hedges[current]->prev = hedges[p];
					diagIn->prev = hedges[current];

					hedges[p]->adjacent_face = newFace;
					hedges[current]->adjacent_face = newFace;
					diagIn->adjacent_face = newFace;

					hedges[p] = diagOut;

					nextIndex[p] = nx;
					prevIndex[nx] = p;

					remaining--;
					current = nx;
					earFound = true;
					break;
				}
			}

			current = nextIndex[current];

		} while (current != first);

		if (!earFound) break;
	}

	int i0 = current;
	int i1 = nextIndex[i0];
	int i2 = nextIndex[i1];

	f->adjacent_halfedge = hedges[i0];

	hedges[i0]->next = hedges[i1];
	hedges[i1]->next = hedges[i2];
	hedges[i2]->next = hedges[i0];

	hedges[i0]->prev = hedges[i2];
	hedges[i1]->prev = hedges[i0];
	hedges[i2]->prev = hedges[i1];

	hedges[i0]->adjacent_face = f;
	hedges[i1]->adjacent_face = f;
	hedges[i2]->adjacent_face = f;
}