/**	\file	VertexClass.cpp
*	Implementation of Vertex class.
*	Contains function definitions associated with Vertex objects.
*/

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include<cassert> 
#include <algorithm> 
#include <stddef.h>

#include "EdgeClass.h"
#include "CellClass.h"
#include "VertexClass.h"
#include "DynamicConstants.h"

using namespace std;

// Tests if two vertices are the same.
// Compares ID of the two vertices and returns 'true' if they are the same, 'false' otherwise. 

//Compute angle theta and Psi (Reid Tang updated 11/4/2025) 
/*void Vertex::ComputeVertexAngles() {
	theta.clear();
	psi.clear();

	// Get the pulling direction from the global variables
	Vect pullingDir(PULLINGDIR_X, PULLINGDIR_Y);
	double pullMag = pullingDir.Magnitude();
	if (pullMag == 0) return;  // avoid division by zero
	Vect pullNorm = pullingDir / pullMag;
	Vect perpDir = pullNorm.FindPerpendicularVector();

	// Get the edges connected to this vertex
	vector<Edge*> evec = GetCWEdges();
	int nEdges = evec.size();
	if (nEdges < 2) return;

	// Compute unit vectors for each edge
	vector<Vect> edgeVectors;
	for (int i = 0; i < nEdges; i++) {
		Vertex* v_other = (*evec[i]).GetOtherVertex(this);
		Vect diff = (*v_other).r - r;
		edgeVectors.push_back(diff / diff.Magnitude());
	}

	// Compute angular openings ? between consecutive edges
	for (int i = 0; i < nEdges; i++) {
		Vect v1 = edgeVectors[i];
		Vect v2 = edgeVectors[(i + 1) % nEdges];
		double cosTheta = v1.DotProduct(v2);
		//cosTheta = max(-1.0, min(1.0, cosTheta));
		double angleTheta = acos(cosTheta);
		theta.push_back(angleTheta);
	}

	// Compute Psi angles for each edge pair
	for (int i = 0; i < nEdges; i++) {
		Vect v1 = edgeVectors[i];
		Vect v2 = edgeVectors[(i + 1) % nEdges];
		Vect bisector = (v1 + v2);
		double anglePsi = 0.0;
		if (bisector.Magnitude() != 0) {
			bisector = bisector / bisector.Magnitude();
			double cosPsi = fabs(bisector.DotProduct(perpDir));
			cosPsi = min(1.0, cosPsi);
			anglePsi = acos(cosPsi);
		}
		psi.push_back(anglePsi);
	}

	// ---- DEBUG OUTPUT ----
	cout << "Vertex " << id << " theta angles: ";
	for (double a : theta) cout << a << " ";
	cout << "\nVertex " << id << " psi angles: ";
	for (double a : psi) cout << a << " ";
	cout << endl;
} */


void Vertex::ComputeVertexAngles() {

	theta.clear();
	psi.clear();

	// Pulling direction
	Vect pullingDir(PULLINGDIR_X, PULLINGDIR_Y);
	double pullMag = pullingDir.Magnitude();
	if (pullMag == 0) return;

	Vect pullNorm = pullingDir / pullMag;
	Vect perpDir = pullNorm.FindPerpendicularVector();

	// Get edges
	vector<Edge*> evec = GetCWEdges();
	int nEdges = evec.size();
	if (nEdges < 2) return;

	vector<pair<Edge*, Vect>> edgeVecs;

	// ---------------------------------------------------
	// Compute unit edge vectors using PBC-aware function
	// ---------------------------------------------------
	vector<Vect> edgeVectors;

	for (int i = 0; i < nEdges; i++) {

		Edge* e = evec[i];
		Vertex* v_other = e->GetOtherVertex(this);

		// Correct periodic displacement
		Vect diff = e->GetVectorDifference(this, v_other);

		double mag = diff.Magnitude();
		if (mag == 0) continue;

		edgeVecs.push_back({e, diff / mag});
	}

	if (edgeVecs.size() < 2) return;

	// ---------------------------------------------------
	// Sort vectors CCW
	// ---------------------------------------------------
    sort(edgeVecs.begin(), edgeVecs.end(),
        [](const pair<Edge*, Vect>& a, const pair<Edge*, Vect>& b) {
            return atan2(a.second.y, a.second.x) < atan2(b.second.y, b.second.x);
        });

	// ---------------------------------------------------
	// Compute interior angles (theta)
	// ---------------------------------------------------
	int n = static_cast<int>(edgeVecs.size());

	for (int i = 0; i < n; i++) {

		Vect v1 = edgeVecs[i].second;
        Vect v2 = edgeVecs[(i + 1) % n].second;

		double cross = v1.x * v2.y - v1.y * v2.x;
		double dot = v1.DotProduct(v2);

		double angleTheta = atan2(cross, dot);

		if (angleTheta < 0)
			angleTheta += 2 * M_PI;

		theta.push_back(angleTheta);
	}

	// ---------------------------------------------------
	// Compute bisector orientation angles (psi)
	// ---------------------------------------------------
	for (int i = 0; i < n; i++) {

		Vect v1 = edgeVecs[i].second;
        Vect v2 = edgeVecs[(i + 1) % n].second;

		Vect bisector = v1 + v2;

		double anglePsi = 0.0;

		if (bisector.Magnitude() != 0) {

			bisector = bisector / bisector.Magnitude();

			double cosPsi = fabs(bisector.DotProduct(perpDir));

			if (cosPsi > 1.0) cosPsi = 1.0;

			anglePsi = acos(cosPsi);
		}

		psi.push_back(anglePsi);
	}

	// ---------------------------------------------------
	// Compute nucleation
	// ---------------------------------------------------

	nucleation.clear();

	for (int i = 0; i < n; i++) {

		Edge* e1 = edgeVecs[i].first;
        Edge* e2 = edgeVecs[(i + 1) % n].first;
		Edge* e3 = edgeVecs[(i + 2) % n].first;

		//cout << e1->GetTension_ForceOnVertex(this).Magnitude() << endl;

		if(NucleationHelper(e1, e2, e3)){
			nucleation.push_back(1);
		}
		else{
			nucleation.push_back(0);
		}
		
	}


	// ---------------------------------------------------
	// Debug output
	// ---------------------------------------------------
	//cout << "Vertex " << id << " theta angles: ";
	//for (double a : theta) cout << a << " ";

	//cout << "\nVertex " << id << " psi angles: ";
	//for (double a : psi) cout << a << " ";

	//cout << "\nVertex " << id << " nucleation ";
	//for (double a : nucleation) cout << a << " ";

	//cout << endl;
}

bool Vertex::NucleationHelper(Edge* e1, Edge* e2, Edge* e3) {
	double forceFromEdge1 = e1->GetTension_ForceOnVertex(this).Magnitude();
	double forceFromEdge2 = e2->GetTension_ForceOnVertex(this).Magnitude();
	double forceFromEdge3 = e3->GetTension_ForceOnVertex(this).Magnitude();
	
	bool useThreeEdge = NUCLEATIONBASECRITERIA;
	double threshold = NUCLEATIONTHRESHOLD;



	if(useThreeEdge){
		if(forceFromEdge1 > threshold && forceFromEdge2 > threshold && forceFromEdge3 < threshold){
		return 1;
		}
		return 0;
	}
	
	if(forceFromEdge1 > threshold && forceFromEdge2 > threshold){
		return 1;
	}
	return 0;
}



bool Vertex::operator==(const Vertex& right) const {
	//check that the id's are the same
	if (id == right.id)
		return true;
	else
		return false;
}

// Returns the number of edges meeting at the vertex.
int Vertex::GetEdgeNumber() {
	return edges.size();
}

// Check if the vertex has exactly 4 edges.
bool Vertex::CheckFourfold() {
	if (edges.size() == 4)
		return 1;
	//else if ( edges.size() >= 4) 
		//cout << "Vertex " << (*this).id << " is a five fold vertex!" << endl;
	else
		return 0;
}

// Removes an edge from the list of edges.
void Vertex::RemoveEdge(Edge* eptr) {
	unsigned int i;
	for (i = 0; i < edges.size(); i++) {
		if ((*edges[i]) == (*eptr)) {
			edges.erase(edges.begin() + i);
		}
	}
}

// Returns a pointer to the vertex on the other end of the specified edge.
Vertex* Vertex::GetOtherVertex(Edge* edg) {
	long int findid;
	int i;
	vector<long int> edgeids;
	//first check that edge is actually a neighbor of vertex
	findid = (*edg).id;
	for (i = 0; i < (*this).edges.size(); i++) {
		edgeids.push_back((*edges[i]).id);
	}


	assert(find(edgeids.begin(), edgeids.end(), findid) != edgeids.end() && "ERROR Vertex::GetOtherVertex given a bad edge");

	if ((*(*edg).vertex1).id == id)
		return (*edg).vertex2;
	else
		return (*edg).vertex1;


}

// Returns a vector of pointers to all vertices connected to this vertex.
vector<Vertex*> Vertex::GetAllOtherVertex() {
	int N = GetEdgeNumber();
	std::vector<Vertex*> verts(N);		//initialize vector verts of size N
	for (int i = 0; i < N; i++) {
		verts[i] = (*edges[i]).GetOtherVertex(this);
	}
	return verts;
}

//GetCWCells returns a list of the cells around the vertex in CW order. The cells have the same starting point as the edges in GetCWEdges if if you walked in a CW circle around the vertex starting at e0 you would see: e0,c0,e1,c1,e2,c2,e3,c3,e0
vector<Cell*> Vertex::GetCWCells() {
	vector<Cell*> orderedcells;
	vector<Edge*> orderededges = (*this).GetCWEdges();
	Edge* startedge = orderededges[0];

	int i = 0;
	Edge* e1 = orderededges[i];			//get the first two edges
	Edge* e2 = orderededges[i + 1];

	do {
		// Find the cell between e1 and e2
		if ((*e1).cell1 == (*e2).cell1)
			orderedcells.push_back((*e1).cell1);
		else if ((*e1).cell1 == (*e2).cell2)
			orderedcells.push_back((*e1).cell1);
		else if ((*e1).cell2 == (*e2).cell1)
			orderedcells.push_back((*e1).cell2);
		else if ((*e1).cell2 == (*e2).cell2)
			orderedcells.push_back((*e1).cell2);

		//update e1 and e2
		i++;
		e1 = orderededges[i % orderededges.size()];
		e2 = orderededges[(i + 1) % orderededges.size()];

	} while ((*e1) != (*startedge));  //while e1 is not the starting edge

	return orderedcells;
}

//GetEdgeBetween returns a pointer to the edge between this vertex and the specified vertex
Edge* Vertex::GetEdgeBetween(Vertex* vrt) {
	vector<Edge*> vrtedges;
	unsigned int i, j;
	Edge* returnedge{};
	vrtedges = (*vrt).edges;
	//look through all of the edges for both vertices
	for (i = 0; i < edges.size(); i++) {
		for (j = 0; j < vrtedges.size(); j++) {
			//when you find a match return it
			if ((*edges[i]).id == (*vrtedges[j]).id) {
				returnedge = edges[i];
				i = edges.size();
				j = vrtedges.size();
			}
			//if you get to the end and never find a match give an error
			else if (((*edges[i]).id != (*vrtedges[j]).id) && (i == (edges.size() - 1)) && (j == (vrtedges.size() - 1))) {
				cout << "ERROR: GetEdgeBetween given an unacceptable vertex for edge" << id << endl;
				returnedge = NULL;
			}
		}
	}
	return returnedge;
}

// Returns a vector of pointers to neighboring cells.
vector<Cell*> Vertex::GetCells() {
	unsigned int i, j;
	vector<Cell*> cells;							//vector of cell pointers used as intermediate storage
	vector<Cell*> returncells;						//return a pointer to a vector

	for (i = 0; i < edges.size(); i++) {					//for each neighboring edge
		cells.push_back((*edges[i]).cell1);			//add the first cell to the vector
		cells.push_back((*edges[i]).cell2);			//add the second cell to the vector
	}

	//Now remove the duplicate cells in the vector
	returncells.push_back(cells[0]);				//start by coppying over the first cell so the list to return has an element
	for (j = 0; j < cells.size(); j++) {					//for each cell in cells
		if (find(returncells.begin(), returncells.end(), cells[j]) == returncells.end()) {			//if the curent edge is NOT in the list of returncells
			returncells.push_back(cells[j]);														//add it to the return list
		}
	}
	return returncells;
}

// Returns a list of the edges around a vertex in CW or CCW order.
vector<Edge*> Vertex::GetOrderedEdges() {
	vector<Edge*> returnlist;
	Edge e;
	Cell c;
	int i, j;

	returnlist.push_back(edges[0]);		//initialize list
	c = (*(*edges[0]).cell1);

	for (i = 0; i < edges.size() - 1; i++) {
		e = (*returnlist.back());      //get the next edge
		if ((*e.cell1) == c)		//get the next cell
			c = (*e.cell2);
		else
			c = (*e.cell1);
		for (j = 0; j < edges.size(); j++) {  //look through the list of edges and get the next one that bounds cell c as well
			if (((*(*edges[j]).cell1) == c || (*(*edges[j]).cell2) == c) && (*edges[j]) != e) {
				returnlist.push_back(edges[j]);
			}
		}
	}
	return returnlist;
}

//returns a list of edges around a vertex in CW order
vector<Edge*> Vertex::GetCWEdges() {
	//Declare and initialize vertex pointers.
	Vertex* v0 = 0;
	Vertex* v1 = 0;
	vector<Edge*> ordered = Vertex::GetOrderedEdges();	//get the ordered edges
	Edge* e0 = ordered[0];			//edge0
	Edge* e1 = ordered[1];			//edge1
	v0 = (*e0).GetOtherVertex(this);		//vertex on the other side of edge0
	v1 = (*e1).GetOtherVertex(this);		//vertex on the other side of edge1
	Vect vect0 = (*e0).GetVectorDifference(this, v0);	//vector of edge0
	Vect vect1 = (*e1).GetVectorDifference(this, v1);		//vector of edge1
	int sign;

	//take the cross product of vect0 X vect1
	sign = vect0.SignCrossProduct(vect1);

	if (sign == -1) {
		return ordered;		//they were alread ordered CW so return the origonal order
	}
	else {
		reverse(ordered.begin(), ordered.end());
		return ordered;
	}
}

//changes all of the flags on the edges atached to the vertex to be the oposite of what they currently are (needed for moving vertices across the boundry)
void Vertex::ChangeXEdgeFlags() {
	int i;
	for (i = 0; i < edges.size(); i++) {
		if ((*edges[i]).xflag == 0)
			(*edges[i]).xflag = 1;
		else
			(*edges[i]).xflag = 0;
	}
}

// Changes all y-flags on the edges atached to the vertex to be the oposite of what they currently are (needed for moving vertices across the boundary).
void Vertex::ChangeYEdgeFlags() {
	int i;
	for (i = 0; i < edges.size(); i++) {
		if ((*edges[i]).yflag == 0)
			(*edges[i]).yflag = 1;
		else
			(*edges[i]).yflag = 0;
	}
}


//updates the vertices position and cahnges the apropreate edge flags
bool Vertex::UpdatePosition() {
	bool VertexHasCrossed = false;
	double newx = (*this).rnew.x;
	double newy = (*this).rnew.y;

	//look at the x component and update and change flags
	if (newx > LX) {
		(*this).r.x = newx - LX;
		(*this).ChangeXEdgeFlags();
		VertexHasCrossed = true;
	}
	else if (newx < 0) {
		(*this).r.x = LX + newx;
		(*this).ChangeXEdgeFlags();
		VertexHasCrossed = true;
	}
	else {
		(*this).r.x = newx;
	}
	//look at the y component and update and change flags
	if (newy > LY) {
		(*this).r.y = newy - LY;
		(*this).ChangeYEdgeFlags();
		VertexHasCrossed = true;
	}
	else if (newy < 0) {
		(*this).r.y = LY + newy;
		(*this).ChangeYEdgeFlags();
		VertexHasCrossed = true;
	}
	else {
		(*this).r.y = newy;
	}
	return VertexHasCrossed;
}