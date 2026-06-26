// guard
#ifndef __AGTISSUE_H_INCLUDED__
#define __AGTISSUE_H_INCLUDED__

// Forward declared dependencies
class Tissue;
class Cell;
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
#include "AGConstants.h"
#include "EdgeClass.h"
#include "CellClass.h"
#include "AGCell.h"
#include "VertexClass.h"
#include "TissueClass.h"

using namespace std;

class AGTissue : public Tissue{
public:
	//ADDITIONAL DATA									// deformation gradient tensor component (22)
	double LX0;									// initial box length in x dimension
	double LY0;			
	double equilibriumtolerance = .0000000000010; // change in dimensionless energy at every timestep has to be less than this to define equilibrium
	bool equilibriumChecker;      		// determines whether packing is equilibrated or not
	double currentFreeEnergy;			// used to determine equilibrium
	double pastFreeEnergy;				// used to determine equilibrium
	double C4; // for the quartic potential
	//NOTE: the list cells that is instantiated in the TissueClass will contain AGCells when AGVertex Model code
	// is being used 
	//CONSTRUCTOR
	AGTissue()
	{
		currentFreeEnergy = 0;
		pastFreeEnergy = 0;
	};

	//DESTRUCTOR
	virtual ~AGTissue(){};

	//ADDITIONAL FUNCTIONS
	
	//*********************************************************************************************************************
	//******************* Helper Functions *********************************
	//*********************************************************************************************************************
	// Updates read input function to accept new global variables 
	void HelperReadInputData_SetGlobalValues(ifstream* datafile) override;

	//Updates read input function to create new objects (AGCells)
	void HelperReadInputData_CreateObjects(ifstream* datafile) override;

	//! Writes data into file specified by 'datafile'
	void HelperWriteOutputData_Write2File(ofstream* datafile, bool includeExtraInfo) override;

	// Finds the number of X - crossing edges
	double CountNumberOfXCrossingEdges();

	// Finds the number of Y - crossing edges
	double CountNumberOfYCrossingEdges();

	//Get vector of X - crossing edges
	vector<Cell*> GetXCrossingCellVector();

	//Get vector of Y - crossing edges
	vector<Cell*> GetYCrossingCellVector();
	
	//*********************************************************************************************************************
	//******************* FUNCTIONS FOR PREFORMING TOPOLOGICAL CHANGES (Mostly Overloads) *********************************
	//*********************************************************************************************************************
	// Do mitosis on cells. Picks cells for mitosis based on certain criteria and calls HelperMitosis(Cell * parentcell) to perform mitosis on those cells.
	bool Mitosis() override;

	//IS THIS PROBLEMATIC? NO!
	//virtual void DestroyCell(Cell* cptr);
	virtual Cell* CreateCell(int typ, vector<Edge*> edgs, string potential, double tcjstrength, double thetaan, double anstrenght);

	//! This is the helper function that actually do mitosis on cells.
	void HelperMitosis(Cell * parentcell, double divisionangle) override;

	//! Called by HelperResolveVertices_ResolveFourfoldVertex to check the stability of one configuration
	//! Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors
	//! returns the magnutude of Force-Forcenewedge and changes the value of Force
	double HelperResolveVertices_CheckStability(Vertex* v, Edge* e1, Edge* e2, Edge* e3, Edge* e4, Cell* c1, Cell* c2, Cell* c3, Cell* c4, Vect *Force) override;

	//*********************************************************************************************************************
	//***************** FUNCTIONS FOR PERFORMING NUMERICAL INTEGRATION TO EVOLVE THE TISSUE ******* ************************
	//*********************************************************************************************************************

	//! Evolves the tissue one time interval taking steps no larger than dt and updates the vertex positions
	void Evolve(double interval, double *stepsize, ofstream* errorfile) override;

	//! Calculates the force on every vertex and stores the result in vertex.
	void ForceFunction(ofstream* errorfile) override;

	//! Calculates the x component of the internal stress of the tissue (so sigma_xx)
	virtual double GetInternalTissueStressX(ofstream* errorfile);

	//! Calculates the y component of the internal stress of the tissue (so sigma_yy)
	virtual double GetInternalTissueStressY(ofstream* errorfile);

	//Integrates the forcefunction by one euiler step 
	void RK4Helper_EuilerStep(double dt, ofstream* errorfile) override;

	//*********************************************************************************************************************
	//***************** Other Functions ******* ************************
	//*********************************************************************************************************************
	
	//! Writes to output file after topology updates/numerical integration - overload will help manage verbosity of the output
	void WriteOutputDataAppend(ofstream* datafile) override;

	// checks if packing is equilibrated
	virtual bool IsEquilibrated();

	//calculates the free energy of a packing 
	virtual double CalculateFreeEnergy();

	//deforms the tissue by defining a deformation gradient tensor
	virtual void DeformTissue(double Fxx, double Fyy);

	// calculates the non dimensionalized polydispersity of the packing using sum over (Ai - <A>)^2 / (N A0^2) for cells i
	virtual double calculatePolydispersity();

	// calculates the third centered area moment divided by the second centered area moment (variance)
	virtual double calculatePressureFunctionQuadraticError();

	// calculates the nondimensionalized third area moment 
	virtual double calculateThirdAreaMoment();

	//calculate the energy of the box
	virtual double CalculateBoxEnergy();

	//calculates the area of the box
	virtual double GetBoxArea();

	//Tests if the packing is equilibrated using a stricter criterion than IsEquilibrated()
	bool StrictIsEquilibrated();
};
#endif