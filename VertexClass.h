// include guard
#ifndef __VERTEXCLASS_H_INCLUDED__
#define __VERTEXCLASS_H_INCLUDED__

//Forward declared dependences
class Edge;
class Cell;

//includes
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <math.h>
#include <stddef.h>
#include <unordered_map>

#include "VectClass.h"		

using namespace std;


class Vertex {
public:
	//************************************************************************************************************
	//*************************************************** DATA ***************************************************
	//************************************************************************************************************
	unsigned long int id;				//!< ID of a vertex. Used to identify if two vertices are equal.
	Vect r;								//!< Current x and y position of the vertex.
	Vect rnew;							//!< Future x and y position of the vertex. 
	Vect rold;							// Position of the vertex just before an integration step is taken 
	vector<Edge*> edges;				//!< Vector containing pointers to Edge objects meeting at a vertex.
	Vect SavedInfoForEvolve_MechForce;		//Place to store the force on the vertex calculated durring evolve
	Vect SavedInfoForEvolve_TotalForce;		//Place to store the force on the vertex calculated durring evolve



	//************************************************************************************************************
	//******************************* FUNDEMENTAL FUNCTIONS ******************************************************
	//************************************************************************************************************

	//Add the the angle computation (Reid Tang at 11/4/2025)
	vector<double> theta;  // opening angles between edges (usually 3 values)
	vector<double> psi;    // Psi angles relative to pulling direction

	void ComputeVertexAngles();

	Vertex() {}
	virtual ~Vertex() {};						//destructor: does not need to be called by you unless you created the object using the word "new" otherwise the compiler will take care of this for you
	//!	Constructs a vertex of id 'idnumb' at position 'x' and 'y'.
	Vertex(unsigned long int idnumb, double x, double y) {
		id = idnumb;
		r.x = x;
		r.y = y;
		SavedInfoForEvolve_MechForce.x = NAN;
		SavedInfoForEvolve_MechForce.y = NAN;
	}

	//!	Constructs a vertex of id 'idnumb' at position 'x' and 'y', and with edges given in 'edg'.
	Vertex(unsigned long int idnumb, double x, double y, vector<Edge*> edg) {
		id = idnumb;
		r.x = x;
		r.y = y;
		edges = edg;
		SavedInfoForEvolve_MechForce.x = NAN;
		SavedInfoForEvolve_MechForce.y = NAN;
	}

	//! Tests if two vertices are the same.
	bool operator==(const Vertex&) const;

	//! Tests if two vertices are NOT the same.
	bool operator!=(const Vertex& right) const { return !(*this == right); }

	//************************************************************************************************************
	//******************* FUNCTIONS FOR CALCULATING VERTEX PROPERTIES **********************************************
	//************************************************************************************************************
		//! Returns the number of edges meeting at the vertex.
	int GetEdgeNumber();

	//! Check if the vertex has exactly 4 edges.
	bool CheckFourfold();

	//************************************************************************************************************
	//******************* FUNCTIONS FOR FINDING NEIGHBORING OBJECTS **********************************************
	//************************************************************************************************************
		//! Returns a pointer to the vertex on the other end of the specified edge.
	Vertex* GetOtherVertex(Edge* edg);

	//! Returns a pointer to the edge between 'this' vertex and the specified vertex.
	Edge* GetEdgeBetween(Vertex* vrt);

	//! Returns a vector of pointers to all vertices connected to this vertex.
	vector<Vertex*> GetAllOtherVertex();

	//! Removes an edge from the list of edges.
	void RemoveEdge(Edge* eptr);

	//! GetCWCells returns a list of the cells around the vertex in CW order. The cells have the same starting point as the edges in GetCWEdges if if you walked in a CW circle around the vertex starting at e0 you would see: e0,c0,e1,c1,e2,c2,e3,c3,e0
	vector<Cell*> GetCWCells();

	//! Returns a list of edges around a vertex in CW order
	vector<Edge*> GetCWEdges();

	//************************************************************************************************************
	//******************* ADDITIONAL FUNCTIONS *******************************************************************
	//************************************************************************************************************
		//updates the vertices position and cahnges the apropreate edge flags
	bool UpdatePosition();

	//************************************************************************************************************
	//******************* HELPER FUNCTIONS *******************************************************************
	//************************************************************************************************************
		//! Returns a vector of pointers to neighboring cells.
	vector<Cell*> GetCells();

	//! Returns a list of the edges around a vertex in CW or CCW order.
	vector<Edge*> GetOrderedEdges();

	//! Changes all x-flags on the edges atached to the vertex to be the oposite of what they currently are (needed for moving vertices across the boundary).
	void ChangeXEdgeFlags();

	//! Changes all y-flags on the edges atached to the vertex to be the oposite of what they currently are (needed for moving vertices across the boundary).
	void ChangeYEdgeFlags();

};


#endif	