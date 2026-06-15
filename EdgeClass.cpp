/**	\file	EdgeClass.cpp
*	Implementation of Edge class.
*	Contains function definitions associated with Edge objects.
*/

#include <iostream>
#include <string>
#include <cmath>
#include <math.h> 
#include <cstdlib>
#include <list>
#include <vector>
#include <cassert>
#include <stddef.h>

#include "VertexClass.h"
#include "CellClass.h"
#include "EdgeClass.h"
#include "Constants.h"

using namespace std;

// Tests if two edges are the same.
// Compares ID of the two edges and returns 'true' if they are the same, 'false' otherwise.
bool Edge::operator==(const Edge& right) const {
	//check that the id's are the same
	if (id == right.id)
		return true;
	else
		return false;
}


// Calculates the length of the edge, respecting PBC. Calls GetLength(double x1, double y1, double x2, double y2).
// Actual implementation of algorithm is in the static function GetLength(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
double Edge::GetLength() {
	//first get the cordinates of the ends of the edge
	double x1, x2, y1, y2;
	x1 = (*vertex1).r.x;
	y1 = (*vertex1).r.y;
	x2 = (*vertex2).r.x;
	y2 = (*vertex2).r.y;

	return GetLengthStatic(x1, y1, x2, y2, xflag, yflag);
}

//! Calculates the length of the edge from the new positions, respecting PBC. Calls GetLength_New(double x1, double y1, double x2, double y2).
//! Actual implementation of algorithm is in the static function GetLength_New(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
double Edge::GetLength_New() {
	//first get the cordinates of the ends of the edge
	double x1, x2, y1, y2;
	x1 = (*vertex1).rnew.x;
	y1 = (*vertex1).rnew.y;
	x2 = (*vertex2).rnew.x;
	y2 = (*vertex2).rnew.y;

	return GetLength_NewStatic(x1, y1, x2, y2, xflag, yflag);
}

//! Calculates the length of the edge from the old positions, respecting PBC. Calls the static function GetLength(double x1, double y1, double x2, double y2).
double Edge::GetLength_Old() {
	//first get the cordinates of the ends of the edge
	double x1, x2, y1, y2;
	x1 = (*vertex1).rold.x;
	y1 = (*vertex1).rold.y;
	x2 = (*vertex2).rold.x;
	y2 = (*vertex2).rold.y;

	return GetLength_NewStatic(x1, y1, x2, y2, xflag, yflag);
}

// Same as GetLength() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2). 
// Calls GetLength(double x1, double y1, double x2, double y2, bool xflag, bool yflag). Actual implementation of algorithm is in the static function GetLength(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
// \note This function is useful in calculating the RHS of ODE for integration, where the positions of the two vertices are variable and are not necessarily equal to the current ones.
double Edge::GetLength(double x1, double y1, double x2, double y2) {
	return GetLengthStatic(x1, y1, x2, y2, xflag, yflag);			//take xflag and yflag from edge (*this) data-member.
}

// Same as GetLength() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2) and PBC flags given by xflag & yflag.
// This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration.
// \attention This function contains the actual implementation of GetLength. All other GetLength functions should call this function to prevent multiple implementations.
double GetLengthStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag) { //I added the static keyword to this function so that it fufilled the GetLengthStatic() that the EdgeClass.h file declared
	double length;

	//Then calculate the length noting that the edge may go across the boundary
	if (!xflag && !yflag) {										//if the edge does not cross the boundary
		length = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		return length;
	}
	else if (xflag && !yflag) {									//if the edge only crosses the x boundary
		length = sqrt((LX - abs(x1 - x2)) * (LX - abs(x1 - x2)) + (y1 - y2) * (y1 - y2));
		return length;
	}
	else if (!xflag && yflag) {									//if the edge only crosses the y boundary
		length = sqrt((x1 - x2) * (x1 - x2) + (LY - abs(y1 - y2)) * (LY - abs(y1 - y2)));
		return length;
	}
	else if (xflag && yflag) {									//if the edge crosses both boundaries
		length = sqrt((LX - abs(x1 - x2)) * (LX - abs(x1 - x2)) + (LY - abs(y1 - y2)) * (LY - abs(y1 - y2)));
		return length;
	}
	else {
		cout << "SOMETHING WRONG IN GET LENGTH EDGE FLAGS NONSENSE";
		return 0;
	}
}

//! Same as GetLength_New() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2) and PBC flags given by xflag & yflag.
//! This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration.
//! This calculates the length based on the positions after evolution (rnew, LXnew, LYnew).
//! \attention This function contains the actual implementation of GetLength. All other GetLength functions should call this function to prevent multiple implementations.
double GetLength_NewStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag) {
	double length;

	//Then calculate the length noting that the edge may go across the boundary
	if (!xflag && !yflag) {										//if the edge does not cross the boundary
		length = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	}
	else if (xflag && !yflag) {									//if the edge only crosses the x boundary
		length = sqrt((LXnew - abs(x1 - x2)) * (LXnew - abs(x1 - x2)) + (y1 - y2) * (y1 - y2));
		return length;
	}
	else if (!xflag && yflag) {									//if the edge only crosses the y boundary
		length = sqrt((x1 - x2) * (x1 - x2) + (LYnew - abs(y1 - y2)) * (LYnew - abs(y1 - y2)));

	}
	else if (xflag && yflag) {									//if the edge crosses both boundaries
		length = sqrt((LXnew - abs(x1 - x2)) * (LXnew - abs(x1 - x2)) + (LYnew - abs(y1 - y2)) * (LYnew - abs(y1 - y2)));
	}
	return length;
}

// Calculates the center of the edge, respecting PBC.
Vect Edge::GetCenter() {
	Vect center;
	double avgx;
	double avgy;
	//first get the coordinates of the ends of the edge
	double x1, x2, y1, y2;
	x1 = (*vertex1).r.x;
	y1 = (*vertex1).r.y;
	x2 = (*vertex2).r.x;
	y2 = (*vertex2).r.y;

	if (!xflag && !yflag) {
		center.x = abs(x1 + x2) / 2.0;
		center.y = abs(y1 + y2) / 2.0;
	}
	else if (xflag && !yflag) {
		if (x1 < x2) {
			avgx = ((x1 + LX) + x2) / 2.0;
			if (avgx > LX) { avgx = avgx - LX; }
		}
		else {
			avgx = ((x2 + LX) + x1) / 2.0;
			if (avgx > LX) { avgx = avgx - LX; }
		}
		center.x = avgx;
		center.y = abs(y1 + y2) / 2.0;
	}
	else if (!xflag && yflag) {
		if (y1 < y2) {
			avgy = ((y1 + LY) + y2) / 2.0;
			if (avgy > LY) { avgy = avgy - LY; }
		}
		else {
			avgy = ((y2 + LY) + y1) / 2.0;
			if (avgy > LY) { avgy = avgy - LY; }
		}
		center.y = avgy;
		center.x = abs(x1 + x2) / 2.0;
	}
	else if (xflag && yflag) {
		if (x1 < x2) {
			avgx = ((x1 + LX) + x2) / 2.0;
			if (avgx > LX) { avgx = avgx - LX; }
		}
		else {
			avgx = ((x2 + LX) + x1) / 2.0;
			if (avgx > LX) { avgx = avgx - LX; }
		}
		if (y1 < y2) {
			avgy = ((y1 + LY) + y2) / 2.0;
			if (avgy > LY) { avgy = avgy - LY; }
		}
		else {
			avgy = ((y2 + LY) + y1) / 2.0;
			if (avgy > LY) { avgy = avgy - LY; }
		}
		center.x = avgx;
		center.y = avgy;
	}
	return center;
}


// Makes rnew of the two vertices to be at the center of the edge. 
// Note that the new position may be outside the tissue boundary, so when the actual position of vertices is updated by Tissue::HelperEvolve_UpdatePositions(), the flags are automatically updated.
// This function is mainly used in T1 (collapsing an edge to a point).
void Edge::MoveVertsToCenter() {
	double avgx;
	double avgy;
	Vertex* vxlarge, * vxsmall, * vylarge, * vysmall;  //pointers to the vertices on the ends of the edge based on which one is larger

	//first get the cordinates of the ends of the edge
	double x1, x2, y1, y2;
	x1 = (*vertex1).r.x;
	y1 = (*vertex1).r.y;
	x2 = (*vertex2).r.x;
	y2 = (*vertex2).r.y;

	//now determine which vertex is at a larger position independently in x and y
	if (x1 > x2) {
		vxlarge = &(*vertex1);
		vxsmall = &(*vertex2);
	}
	else {
		vxlarge = &(*vertex2);
		vxsmall = &(*vertex1);
	}
	if (y1 > y2) {
		vylarge = &(*vertex1);
		vysmall = &(*vertex2);
	}
	else {
		vylarge = &(*vertex2);
		vysmall = &(*vertex1);
	}

	//no x flag
	if (!xflag) {
		(*vertex1).rnew.x = abs(x1 + x2) / 2.0;
		(*vertex2).rnew.x = abs(x1 + x2) / 2.0;
	}

	//xflag
	else {
		avgx = (x1 + LX + x2) / 2.0;
		if (avgx < LX) {
			(*vxlarge).rnew.x = avgx;
			(*vxsmall).rnew.x = avgx - LX;
		}
		else {
			(*vxlarge).rnew.x = avgx;
			(*vxsmall).rnew.x = avgx - LX;
		}
	}

	//no y flag
	if (!yflag) {
		(*vertex1).rnew.y = abs(y1 + y2) / 2.0;
		(*vertex2).rnew.y = abs(y1 + y2) / 2.0;
	}

	//yflag
	else {
		avgy = (y1 + LY + y2) / 2.0;
		if (avgy < LY) {
			(*vylarge).rnew.y = avgy;
			(*vysmall).rnew.y = avgy - LY;
		}
		else {
			(*vylarge).rnew.y = avgy;
			(*vysmall).rnew.y = avgy - LY;
		}
	}
}

// Calculates angle of the edge at vertex 'verts', respecting PBC. Angle follows standard convention (i.e. positive value is CCW with respect to positive x-axis).
// Actual implementation is in the static function GetAngle(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
double Edge::GetAngle(Vertex* v1) {
	//Declare and initialize pointer.
	Vertex* v2 = 0;
	v2 = GetOtherVertex(v1);
	Vect r;
	double x1, x2, y1, y2;

	//first get the cordinates of the ends of the edge
	x1 = (*v1).r.x;
	y1 = (*v1).r.y;
	x2 = (*v2).r.x;
	y2 = (*v2).r.y;

	return GetAngleStatic(x1, y1, x2, y2, xflag, yflag);
}

// Same as GetAngle(), but with positions of vertices 1 and 2 is given as (x1,y1) & (x2,y2).
// Actual implementation is in the static function GetAngle(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
// \note This function is useful in calculating the RHS of ODE for integration, where the positions of the two vertices are variable and are not necessarily equal to the current ones.
double Edge::GetAngle(double x1, double y1, double x2, double y2) {
	return GetAngleStatic(x1, y1, x2, y2, xflag, yflag);		//take xflag and yflag from edge data member
}


// Same as GetAngle(), but with positions of vertices 1 and 2 is given as (x1,y1) & (x2,y2) and flags given by xflag & yflag.
// This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration.
// \attention This function contains the actual implementation of GetAngle. All other GetAngle functions should call this function to prevent multiple implementations.
double GetAngleStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag) {
	double dx, dy;

	if (!xflag && !yflag) {											//if the edge does not cross the boundary
		dx = x2 - x1;
		dy = y2 - y1;
	}
	else if (xflag && !yflag) {										//if the edge only crosses the x boundary
		dx = (x2 - x1) + LX * copysign(1.0, (x1 - x2));					//GSL_SIGN(x) is a macro (defined in gsl/gsl_math.h) that will be replaced with ((x) >= 0 ? 1 : -1) by the preprocessor
		dy = (y2 - y1);
	}
	else if (!xflag && yflag) {										//if the edge only crosses the y boundary
		dx = (x2 - x1);
		dy = (y2 - y1) + LY * copysign(1.0, (y1 - y2));
	}
	else if (xflag && yflag) {										//if the edge crosses both boundaries
		dx = (x2 - x1) + LX * copysign(1.0, (x1 - x2));
		dy = (y2 - y1) + LY * copysign(1.0, (y1 - y2));
	}
	return atan2(dy, dx);
}


// Calculates vector from v1 to v2 (i.e. r2 - r1), taking PBC into account.
// Note that v1 and v2 MUST be connected by the edge (*this).
// Actual implementation of the algorithm is in the static function GetVectorDifference(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
Vect Edge::GetVectorDifference(Vertex* v1, Vertex* v2) {
	Edge* e = (*v1).GetEdgeBetween(v2);
	double x1, y1, x2, y2;
	x1 = (*v1).r.x;
	y1 = (*v1).r.y;
	x2 = (*v2).r.x;
	y2 = (*v2).r.y;

	if (*this != *e) { cout << "ERROR: Edge::GetVectorDifference given unacceptable edge and/or vertices!" << endl; }
	return GetVectorDifferenceStatic(x1, y1, x2, y2, xflag, yflag);
}


// Same as above but with the flags given by 'xflag' & 'yflag'.
// Note that the two vertices need NOT be connected by an edge, hence the 'static' declaration.
// Actual implementation of the algorithm is in the static function GetVectorDifference(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
Vect GetVectorDifferenceStatic(Vertex* v1, Vertex* v2, bool xflag, bool yflag) {
	double x1, y1, x2, y2;
	x1 = (*v1).r.x;
	y1 = (*v1).r.y;
	x2 = (*v2).r.x;
	y2 = (*v2).r.y;

	return GetVectorDifferenceStatic(x1, y1, x2, y2, xflag, yflag);
}


// Same as above but with the position of the vertices given in (x1,y1) & (x2,y2).
// Note that the two vertices need NOT be connected by an edge, hence the 'static' declaration.
// \attention This function contains the actual implementation of GetVectorDifference. All other GetVectorDifference functions should call this function to prevent multiple implementations.
Vect GetVectorDifferenceStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag) {
	double angle = GetAngleStatic(x1, y1, x2, y2, xflag, yflag);			//calculate angle at v1, taking into account PBC
	double length = GetLengthStatic(x1, y1, x2, y2, xflag, yflag);
	Vect origin(0.0, 0.0);
	return Vect(origin, angle, length);
}


// Takes a pointer to a vertex and returns a pointer to the other vertex on the edge.
Vertex* Edge::GetOtherVertex(Vertex* vert) {
	if (vert == vertex1) { return vertex2; }
	else if (vert == vertex2) { return vertex1; }
	else {
		cout << "ERROR in GetOtherVertex from vertex " << id << endl;
		return NULL;
	}
}

// Takes a pointer to a cell and returnes a pointer to the cell on the other side of the edge.
Cell* Edge::GetOtherCell(Cell* cell) {
	if (cell == cell1) { return cell2; }
	else if (cell == cell2) { return cell1; }
	else {
		cout << "ERROR in GetOtherCell" << endl;
		return 0;
	}
}

// Replaces cell 'currentcell' with 'newcell'.
void Edge::UpdateCell(Cell* curentcell, Cell* newcell) {
	if ((*cell1).id == (*curentcell).id) { cell1 = newcell; }
	else if ((*cell2).id == (*curentcell).id) { cell2 = newcell; }
	else { cout << "ERROR in UpdateCell" << endl; }
}

// Replaces vertex 'curentv' with 'newv'.
void Edge::UpdateVertex(Vertex* currentv, Vertex* newv) {
	if ((*vertex1) == (*currentv)) { vertex1 = newv; }
	else if ((*vertex2) == (*currentv)) { vertex2 = newv; }
	else { cout << "ERROR in UpdateVERTEX" << endl; }
}

//NOTE: MUST DEFINE GET TENSION MAGNITUDE WITHIN SUBCLASS IF YOU REQUIRE FANCY MAGNITUDE->CALLED INDEPENDENTLY WITHIN CALCULATETISSUESTRESS
double Edge::GetTension_Magnitude() {
	return GAMMA;
}

//Returns a vector of the force on the vertex 
Vect Edge::GetTension_ForceOnVertex(Vertex* v) {
	double angle = (*this).GetAngle(v);
	Vect origin(0.0, 0.0);						//define the origin
	double mag = (*this).GetTension_Magnitude();			//get the magnitude of the tension
	Vect ret = Vect(origin, angle, mag);
	return ret;				//same magnitude for all tension
}

// Returns a unit vector along the edge from vertex 'v1'.
Vect Edge::MakeUnitVector(Vertex* v1) {
	double angle = Edge::GetAngle(v1);
	Vect origin(0.0, 0.0); //define the origin
	return Vect(origin, angle, 1);
}

//! Takes a pointer to a cell and returns 1 if the cell is listed as edge.cell1 or 2 if the cell is listed as edge. cell2
int Edge::GetThisCellNumber(Cell* c) {
	if ((*c).id == (*cell1).id) {
		return 1;
	}
	else if ((*c).id == (*cell2).id) {
		return 2;
	}
	else {
		assert((false) && "ERROR in GetThisCellNumber, edge does not contain this cell");
	}
}

//Returns true if this edge and the given edge e share a vertex 
bool Edge::ShareVertex(Edge* e) {
	int v11 = (*(*this).vertex1).id;
	int v12 = (*(*this).vertex2).id;
	int v21 = (*(*e).vertex1).id;
	int v22 = (*(*e).vertex2).id;

	if (v11 != v21 && v11 != v22 && v12 != v21 && v12 != v22) return false;
	else return true;
}


//Gets a list of the (usualy 4) neighboring edges 
vector<Edge*> Edge::GetNeighboringEdges() {
	vector<Edge*> list;

	//Get the edge of the first vertex 
	for (int i = 0; i < (*vertex1).edges.size(); i++) {
		if (id != (*(*vertex1).edges[i]).id)
			list.push_back(&(*(*vertex1).edges[i]));
	}

	//Get the edge of the second vertex 
	for (int i = 0; i < (*vertex2).edges.size(); i++) {
		if (id != (*(*vertex2).edges[i]).id)
			list.push_back(&(*(*vertex2).edges[i]));
	}

	return list;
}


//Returns the (generaly 2) edges that neighbor this edge and are in cell 1 or 2 i must be 1 or 2
vector<Edge*> Edge::GetNeighboringEdgesFromCell(int n) {
	vector<Edge*> list;
	int idcell;
	Edge* e;

	//Get the id of cell i 
	if (n == 1) {
		idcell = (*cell1).id;
	}
	else if (n == 2) {
		idcell = (*cell2).id;
	}
	else {
		cout << "ERROR IN GetNeighboringEdgesFromCell n MUST BE 1 OR 2" << endl;
	}

	//Get the edge of the first vertex 
	for (int i = 0; i < (*vertex1).edges.size(); i++) {
		e = (*vertex1).edges[i];
		if (id != (*e).id)
			if (idcell == (*(*e).cell1).id || idcell == (*(*e).cell2).id)
				list.push_back(&(*e));
	}

	//Get the edge of the second vertex 
	for (int i = 0; i < (*vertex2).edges.size(); i++) {
		e = (*vertex2).edges[i];
		if (id != (*e).id)
			if (idcell == (*(*e).cell1).id || idcell == (*(*e).cell2).id)
				list.push_back(&(*e));
	}

	return list;
}