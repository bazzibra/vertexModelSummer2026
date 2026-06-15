/**	\file	EdgeClass.h
*	Header of Edge class.
*	Contains data and function declarations associated with Edge objects.
*/

// include guard
#ifndef __EDGECLASS_H_INCLUDED__
#define __EDGECLASS_H_INCLUDED__

//Forward declared dependences
class Cell;
class Vertex;
class Vect;

//includes
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <stddef.h>

using namespace std;


class Edge {
public:
	//************************************************************************************************************
	//*************************************************** DATA ***************************************************
	//************************************************************************************************************
	unsigned long int id;						//!< ID of an edge. Used to determine if two edges are equal.
	bool xflag;
	bool yflag;									//!< Flag that indicates if the edge wraps around the boundary (periodic boundary condition/PBC) on the x- and y-direction.
	Vertex* vertex1;
	Vertex* vertex2;							//!< Pointers to the two verticies that define the edge (no specific ordering is assumed between vertex1 and vertex2).
	Cell* cell1;
	Cell* cell2;								//!< pointers to the two cells on either side of the edge (no specific ordering is assumed between cell1 and cell2).
	int numbT1;									//Number of T1s 
	int IOVertexID[2];							//!< Ids of the vertices that the edge has as neighbors (<em>only used for input/output purposes!</em>).

	//************************************************************************************************************
	//******************************* FUNDEMENTAL FUNCTIONS ******************************************************
	//************************************************************************************************************
	Edge() {}
	virtual ~Edge() {};						//destructor: does not need to be called by you unless you created the object using the word "new" otherwise the compiler will take care of this for you
	Edge* Clone() { return new Edge(*this); }

	//! Constructs an edge with id 'idnumb', vertices 'v1' and 'v2', and flags 'xflag', 'yflag'.
	Edge(unsigned long int idnumb, int v1, int v2, bool flagx, bool flagy) {
		id = idnumb;
		yflag = flagy;
		xflag = flagx;
		vertex1 = NULL;
		vertex2 = NULL;
		cell1 = NULL;
		cell2 = NULL;
		numbT1 = 0;
		IOVertexID[0] = v1;
		IOVertexID[1] = v2;
	}

	//! Constructs an edge with id 'idnumb', vertices 'v1' & 'v2', flags 'xflag' & 'yflag', and cells 'c1' & 'c2'.
	Edge(unsigned long int idnum, bool flagx, bool flagy, Vertex* v1, Vertex* v2, Cell* c1, Cell* c2) {
		id = idnum;
		yflag = flagy;
		xflag = flagx;
		vertex1 = v1;
		vertex2 = v2;
		cell1 = c1;
		cell2 = c2;
		numbT1 = 0;
	}

	//! Tests if two edges are the same.
	//! Compares ID of the two edges and returns 'true' if they are the same, 'false' otherwise.
	bool operator==(const Edge&) const;

	//! Tests if two edges are NOT the same.
	//! Compares ID of the two edges and returns 'true' if they are NOT the same, 'false' otherwise.
	bool operator!=(const Edge& right) const {
		return !(*this == right);
	}

	//************************************************************************************************************
	//******************* FUNCTIONS FOR CALCULATING EDGE PROPERTIES **********************************************
	//************************************************************************************************************
	//! Calculates the length of the edge, respecting PBC. Calls GetLength(double x1, double y1, double x2, double y2).
	double GetLength();

	//! Calculates the length of the edge from the new positions, respecting PBC. Calls GetLength_New(double x1, double y1, double x2, double y2).
	double GetLength_New();

	//! Calculates the length of the edge from the old positions, respecting PBC. Calls the static function GetLength(double x1, double y1, double x2, double y2).
	double GetLength_Old();

	//! Calculates the center of the edge, respecting PBC.
	Vect GetCenter();

	//! Calculates angle of the edge at vertex 'verts', respecting PBC. Angle follows standard convention (i.e. positive value is CCW with respect to positive x-axis). Actual implementation is in the static function GetAngle(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
	double GetAngle(Vertex* vert);

	//Returns the magnitude of the tension on the edge (this is implemented differently in the edge subclasses) 
	virtual double GetTension_Magnitude();

	//************************************************************************************************************
	//******************* FUNCTIONS FOR FINDING NEIGHBORING OBJECTS **********************************************
	//************************************************************************************************************
	//Returns true if this edge and the given edge e share a vertex 
	bool ShareVertex(Edge* e);

	//! Takes a pointer to a vertex and returns a pointer to the other vertex on the edge.
	Vertex* GetOtherVertex(Vertex* vert);

	//! Takes a pointer to a cell and returnes a pointer to the cell on the other side of the edge.
	Cell* GetOtherCell(Cell* cell);

	//! Replaces cell 'currentcell' with 'newcell'.
	void UpdateCell(Cell* currentcell, Cell* newcell);

	//! Replaces vertex 'curentv' with 'newv'.
	void UpdateVertex(Vertex* currentv, Vertex* newv);

	//Gets a list of the (usualy 4) neighboring edges 
	vector<Edge*> GetNeighboringEdges();

	////Returns the (generaly 2) edges that neighbor this edge and are in cell 1 or 2 i must be 1 or 2
	vector<Edge*> GetNeighboringEdgesFromCell(int i);

	//************************************************************************************************************
	//******************* ADDITIONAL FUNCTIONS *******************************************************************
	//************************************************************************************************************
	//Returns a vector of the force on the vertex 
	virtual Vect GetTension_ForceOnVertex(Vertex* v);

	//! Makes rnew of the two vertices to be at the center of the edge. Note that the new position may be outside the tissue boundary, so when the actual position of vertices is updated by Tissue::HelperEvolve_UpdatePositions(), the flags are automatically updated. This function is mainly used in T1 (collapsing an edge to a point).
	void MoveVertsToCenter();

	//! Returns a unit vector along the edge from vertex 'v1'.
	Vect MakeUnitVector(Vertex* v1);

	//************************************************************************************************************
	//******************* HELPER FUNCTIONS *******************************************************************
	//************************************************************************************************************
	//! Takes a pointer to a cell and returns 1 if the cell is listed as edge.cell1 or 2 if the cell is listed as edge.cell2
	int GetThisCellNumber(Cell* cell);

	//! Same as GetLength() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2). Calls GetLength(double x1, double y1, double x2, double y2, bool xflag, bool yflag). Actual implementation of algorithm is in the static function GetLength(double x1, double y1, double x2, double y2, bool xflag, bool yflag). This function is useful in calculating the RHS of ODE for integration, where the positions of the two vertices are variable and are not necessarily equal to the current ones.
	double GetLength(double x1, double y1, double x2, double y2);

	//! Same as GetAngle(), but with positions of vertices 1 and 2 is given as (x1,y1) & (x2,y2).Actual implementation is in the static function GetAngle(double x1, double y1, double x2, double y2, bool xflag, bool yflag). This function is useful in calculating the RHS of ODE for integration, where the positions of the two vertices are variable and are not necessarily equal to the current ones.
	double GetAngle(double x1, double y1, double x2, double y2);

	//! Calculates vector from v1 to v2 (i.e. r2 - r1), taking PBC into account. Note that v1 and v2 MUST be connected by the edge (*this). Actual implementation of the algorithm is in the static function GetVectorDifference(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
	Vect GetVectorDifference(Vertex* v1, Vertex* v2);

};

//************************************************************************************************************
//******************* STATIC FUNCTIONS *******************************************************************
//************************************************************************************************************

//! Same as GetLength() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2) and PBC flags given by xflag & yflag. This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration. This function contains the actual implementation of GetLength. All other GetLength functions should call this function to prevent multiple implementations.
static double GetLengthStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag);

//! Same as GetLength_New() but with position of the vertices 1 and 2 given as (x1,y1) & (x2,y2) and PBC flags given by xflag & yflag. This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration. This calculates the length based on the positions after evolution (rnew, LXnew, LYnew). \attention This function contains the actual implementation of GetLength. All other GetLength functions should call this function to prevent multiple implementations.
static double GetLength_NewStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag);

//! Same as GetAngle(), but with positions of vertices 1 and 2 is given as (x1,y1) & (x2,y2) and flags given by xflag & yflag. This is the general case where the two vertices need NOT be connected by an edge, hence the 'static' declaration. This function contains the actual implementation of GetAngle. All other GetAngle functions should call this function to prevent multiple implementations.
static double GetAngleStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag);

//! Same as above but with the flags given by 'xflag' & 'yflag'. Note that the two vertices need NOT be connected by an edge, hence the 'static' declaration. Actual implementation of the algorithm is in the static function GetVectorDifference(double x1, double y1, double x2, double y2, bool xflag, bool yflag).
static Vect GetVectorDifferenceStatic(Vertex* v1, Vertex* v2, bool xflag, bool yflag);

//! Same as above but with the position of the vertices given in (x1,y1) & (x2,y2). Note that the two vertices need NOT be connected by an edge, hence the 'static' declaration. This function contains the actual implementation of GetVectorDifference. All other GetVectorDifference functions should call this function to prevent multiple implementations.
static Vect GetVectorDifferenceStatic(double x1, double y1, double x2, double y2, bool xflag, bool yflag);



#endif
