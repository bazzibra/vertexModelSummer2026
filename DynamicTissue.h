/**	\file	DynamicTissueClass.h
*	Header of DynamicTissue class.
*	Contains data and function declarations associated with DynamicTissue objects.
*/


// guard
#ifndef __DYNAMICTISSUE_H_INCLUDED__
#define __DYNAMICTISSUE_H_INCLUDED__

// Forward declared dependencies
//class Tissue;
class AGTissue;
//class Cell;
class AGCell;
class Edge;
class Vertex;
class Vect;

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <unordered_map>
#include <stddef.h>

#include "VectClass.h"
#include "Constants.h"
//#include "EdgeClass.h"
#include "DynamicEdge.h"
//#include "CellClass.h"
#include "AGCell.h"
#include "VertexClass.h"
//#include "TissueClass.h"
#include "AGTissue.h"
// Does this matter?
#include "AGConstants.h"
#include "DynamicConstants.h"


using namespace std;

class Tissue_Dynamic : public AGTissue {
public:
	//************************************************************************************************************
	//*************************************************** DATA ***************************************************
	//************************************************************************************************************
	//CONSTRUCTOR
	Tissue_Dynamic() {
		currentFreeEnergy = 0;
		pastFreeEnergy = 0;
	};

	//DESTRUCTOR
	virtual ~Tissue_Dynamic() {};

//************************************************************************************************************
//******************* FUNCTIONS FOR EVOLVING THE TISSUE BY NUMERIC INTEGRATION ******************************************
//************************************************************************************************************

	// OVERWRITTEN IN DYNAMIC -> NEED TO do this to store rest lengths before step -- no need to have separate _Dynamic functions
	// THIS ONE IS FINE
	//! Evolves the tissue one time interval taking steps no larger than dt and updates the vertex positions
	void Evolve(double interval, double* stepsize, ofstream* errorfile) override;

	//! Integrates the force function by a 4th order Runge-Kutta algorithem
	// THIS ONE IS FINE
	// assuming all good, follows structure of previous code
	void Integrate_RK4(double dt, ofstream* errorfile) override;

	//! Adaptive evolver for mechanical variables (enforcing given error tolerances). Only evolve to time t1.
	// THIS ONE IS FINE
	// assuming all good, follows structure of previous code
	int Integrate_RK4_Adaptive(double* step_size, const double error_abs_tolerance, double* t, double t1, ofstream* errorfile) override;

	//! Calculates changes in edge rest length 
	// THIS ONE IS FINE
	virtual void LengthFunction(ofstream* errorfile);

	//! Integrates the forcefunction by one euiler step 
	// THIS ONE IS NOT! FINE
	void Integrate_Euiler(double dt, ofstream* errorfile) override;

	//************************************************************************************************************
	//******************* FUNCTIONS FOR PREFORMING TOPOLOGICAL CHANGES ******************************************
	//************************************************************************************************************

	//! Looks through all of the edges and find any that are to small and have them undergo T1 transitions, returns TRUE if successful.
	// THIS ONE IS FINE; calls the right function versions, probably not here
	bool T1(ofstream* errorfile) override;

	/////////////////////////ADDITIONAL FUNCTIONS/////////////////////////////////////////

	//calculates the free energy of a packing 
	// THIS ONE IS PROBABLY FINE
	double CalculateFreeEnergy() override;


	//Updates read input function to read in new variables
	// OVERWRITTEN IN DYNAMIC --> for good reason
	// THIS ONE IS FINE
	void HelperReadInputData_SetGlobalValues(ifstream* datafile) override;
	// OVERWRITTEN IN DYNAMIC --> for good reason, just l_naught
	// THIS ONE IS FINE
	void HelperReadInputData_CreateObjects(ifstream* datafile) override;


	//************************************************************************************************************
	//******************* FUNCTIONS FOR ADDING/SUBTRACTING VERTS EDGES AND CELLS *********************************
	//************************************************************************************************************
	//! Creates a new edge and addes it to the list of edges, returns a pointer to that edge.
	// THIS ONE IS FINE
	virtual Edge* CreateEdge(Edge* subclass, bool xflg, bool yflg, Vertex* v1, Vertex* v2, Cell* c1, Cell* c2, double* ptr_savedRestLength_T1);


	//************************************************************************************************************
	//************************************* Helper Functions******************************************************
	//************************************************************************************************************

	//! Build array of vertex positions (x and y) as well as LX and LY based on current values.
	// THIS ONE IS FINE
	void Store_Initial_Condition(vector<Vect>& y, vector<double>& q);

	//! Store the updated positions (from any of the mechanical position evolvers).
	// THIS ONE IS FINE
	void Store_New_Positions(vector<Vect> & y, vector<double>& q);

	//! Update rold with specified value.
	// THIS ONE IS FINE
	void Update_Old_Mechanical_Variables(vector<Vect>& y, vector<double>& q);

	//! Moves the vertices to their new positions and updates the approproate flags if they cross a boundary.
	// THIS ONE IS FINE
	bool HelperEvolve_UpdatePosition() override;

	//Integrates the forcefunction by one euiler step 
	// OVERWRITTEN IN DYNAMIC -> probably necessary for storing / updating rest lengths
	// THIS ONE IS FINE -- I THINK IT IS FIXED NOW
	void RK4Helper_EuilerStep(double dt, ofstream* errorfile) override;
	// virtual void RK4Helper_EuilerStep(double dt, ofstream* errorfile);

	// OVERWRITTEN IN DYNAMIC --> slight but necessary changes
	//! Writes data into file specified by 'datafile'
	// THIS ONE IS FINE
	void HelperWriteOutputData_Write2File(ofstream* datafile, bool includeExtraInfo) override;

	//! RemoveEdge deletes an edge and merges the two vertices to create a fourfold vertex
	// JUST SAVES TENSION AND REST LENGTH, PROBABLY OKAY
	virtual Vertex* HelperT1_RemoveEdge(Edge* deledge, ofstream* errorfile, int* nT1, double* ptr_savedTension_T1, double* ptr_savedRestLength_T1);


	//! Resolves a forfold vertex into 2 threefold vertices if it is energeticaly favorable to do so, returns TRUE if the resolve is successful.
	// JUST USES TENSION AND REST LENGTH, PROBABLY OKAY
	bool HelperResolveVertices_ResolveFourfoldVertex(Vertex* v, double* ptr_savedTension_T1, double* ptr_savedRestLength_T1);

	//! Called by HelperResolveVertices_ResolveFourfoldVertex to check the stability of one configuration
	//! Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors
	//! returns the magnitude of Force-Forcenewedge and changes the value of Force
	//! CHECK!!!
	// JUST USES TENSION AND REST LENGTH, PROBABLY OKAY
	virtual double HelperResolveVertices_CheckStability(Vertex* v, Edge* e1, Edge* e2, Edge* e3, Edge* e4, Cell* c1, Cell* c2, Cell* c3, Cell* c4, Vect* Force, double* ptr_savedTension_T1);


	//! Called by HelperResolveVertices_ResolveFourfoldVertex to update the pointers when the vertex needs to resolve, Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors 
	// JUST USES TENSION AND REST LENGTH, PROBABLY OKAY
	Edge* HelperResolveVertices_UpdatePtrs(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force, double* ptr_savedRestLength_T1);

	//! Step size control mechanical evolution
	virtual bool Double_Step_Size_Control(const vector<Vect>& y_error, const vector<double>& rest_error, const double error_tolerance_absolute, double* initial_step_size, double consistency_order);

	bool StrictIsEquilibrated();
};
#endif