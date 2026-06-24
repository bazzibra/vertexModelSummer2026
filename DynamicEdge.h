/**	\file	DynamicEdgeClass.h
*	Header of DynamicEdge class.
*	Contains data and function declarations associated with DynamicEdge objects.
*/

// include guard
#ifndef __DYNAMICEDGE_H_INCLUDED__
#define __DYNAMICEDGE_H_INCLUDED__

//Forward declared dependences
class AGCell;
class Vertex;
class Vect;

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <stddef.h>
#include <unordered_map>

#include "Constants.h"
#include "AGConstants.h"
#include "DynamicConstants.h"
#include "EdgeClass.h"
//#include "CellClass.h"
#include "AGCell.h"
#include "VertexClass.h"
//#include "TissueClass.h" 
#include "AGTissue.h" 

using namespace std;

class Edge_Dynamic : public Edge {
public:
	//ADDITIONAL DATA ()
	double L0;									//!< set the rest length of the edge when it is first created // WHY DO THIS?
	double restLengthOld;							//!< length on an edge immediately before integration step
	double restLengthNew;							//!< future length of an edge
	double currentRestLength;								//!< Current length of the edge
	double SavedInfoForEvolve_RestLength;		//Place to store the length of the edge calculated during evolve


	//CONSTRUCTORS AND DESTRUCTORS
	Edge_Dynamic() : L0(0), restLengthOld(0), restLengthNew(0), currentRestLength(0), SavedInfoForEvolve_RestLength(0) {};
	virtual ~Edge_Dynamic() {};
	// WHY NO CELL POINTERS AS INPUT?
	// why no x flag or y flag?
	// Do we actually even use this constructor?
	Edge_Dynamic(unsigned long int idnum, bool flagx, bool flagy, Vertex* v1, Vertex* v2) {
		id = idnum;
		yflag = 0;
		xflag = 0;
		vertex1 = v1;
		vertex2 = v2;
		//L0 = L0;
	}

//************************************************************************************************************
//******************* FUNCTIONS FOR CALCULATING EDGE PROPERTIES **********************************************
//************************************************************************************************************
	
	//Returns the magnitude of the tension on the edge 
	double GetTension_Magnitude() override;

	//Returns the magnitude of the current rest edge length.
	double GetRestLength();


//************************************************************************************************************
//******************* ADDITIONAL FUNCTIONS *******************************************************************
//************************************************************************************************************

	//Returns a vector of the force on the vertex 
	virtual Vect GetTension_ForceOnVertex(Vertex* v, double* ptr_savedTension_T1);

};

#endif

