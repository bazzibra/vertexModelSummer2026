 // include guard
#ifndef __TISSUECLASS_H_INCLUDED__
#define __TISSUECLASS_H_INCLUDED__

//Forward declared dependences
class Edge;
class Cell;
class Vertex;
class Vect;

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <unordered_map>
#include <random>
#include <chrono>								//temporary, for measuring time and debugging evolve().

#include "VectClass.h"

using namespace std;

//! Class for tissue object.
class Tissue{
public:
	//************************************************************************************************************
	//*************************************************** DATA ***************************************************
	//************************************************************************************************************
	list<Vertex*> vertices;								//!< List of all vertex objects (not pointers!) in the simulation. This is where the real objects are stored.			
	list<Edge*>  edges;									//!< List of all edge objects (not pointers!) in the simulation. This is where the real objects are stored.	
	list<Cell*> cells;									//!< List of all cell objects (not pointers!) in the simulation. This is where the real objects are stored.	
	double LXold;										//Place to store changing boxsize info during evolve
	double LYold;
	double ForceLX;
	double ForceLY;
	list<int> vertexIDs;								//lists of all of the ID lables ever used for the vertices edges and cells so new lables can be made unique
	list<int> edgeIDs;
	list<int> cellIDs;

	//************************************************************************************************************
	//******************************* FUNDEMENTAL FUNCTIONS ******************************************************
	//************************************************************************************************************
	Tissue(){};							//!< Default constructor. 
	virtual ~Tissue(){};				//!< destructor: does not need to be called by you unless you created the object using the word "new" otherwise the compiler will take care of this for you \todo Remove! 


	//************************************************************************************************************
	//******************* FUNCTIONS FOR ADDING/SUBTRACTING VERTS EDGES AND CELLS *********************************
	//************************************************************************************************************
	//! Creates a new vertex and adds it to the list of vertices, returns a pointer to that vertex.
	virtual Vertex* CreateVertex(Vect position, vector<Edge*> edgs);

	//! Removes vertex 'vert' from the master list.
	virtual void DestroyVertex(Vertex* vert);

	//! Creates a new edge and addes it to the list of edges, returns a pointer to that edge.
	virtual Edge* CreateEdge(Edge* subclass, bool xflg, bool yflg, Vertex* v1, Vertex* v2, Cell* c1, Cell* c2);

	//! Removes the vertex from the master list.
	virtual void DestroyEdge(Edge* eptr);

	//! Creates a new cell and addes it to the list, returns a pointer to that cell.
	virtual Cell* CreateCell(int typ, vector<Edge*> edgs);

	//! Removes the cell from the master list.
	virtual void DestroyCell(Cell* cptr);

	//************************************************************************************************************
	//******************* FUNCTIONS FOR EVOLVING THE TISSUE BY NUMERIC INTEGRATION ******************************************
	//************************************************************************************************************

	//! Evolves the tissue one time interval taking steps no larger than dt and updates the vertex positions
	virtual void Evolve(double interval, double *stepsize, ofstream* errorfile);

	//! Calculates the force on every vertex and stores the result in vertex.
	virtual void ForceFunction(ofstream* errorfile);

	//! Integrates the force function by a 4th order Runge-Kutta algorithem 
	virtual void Integrate_RK4(double dt, ofstream* errorfile);

	//! Adaptive evolver for mechanical variables (enforcing given error tolerances). Only evolve to time t1.
	virtual int Integrate_RK4_Adaptive(double* step_size, const double error_abs_tolerance, double* t, double t1, ofstream* errorfile);

	//! Integrates the forcefunction by one euiler step 
	virtual void Integrate_Euiler(double dt, ofstream* errorfile);

	//! Calculate Tissue Stress in both directions for evolving step size.
	// WHY ARE AG TISSUE VERSIONS CALLED SOMETHING DIFFERENT
	virtual double CalculateTissueStressX(double cutline, ofstream* errorfile);
	virtual double CalculateTissueStressY(double cutline, ofstream* errorfile);


	//************************************************************************************************************
	//******************* FUNCTIONS FOR PREFORMING TOPOLOGICAL CHANGES ******************************************
	//************************************************************************************************************
	//! Do mitosis on cells. Picks cells for mitosis based on certain criteria and calls HelperMitosis(Cell * parentcell) to perform mitosis on those cells.
	virtual bool Mitosis();

	//! Looks thourgh all of the cells in the tissue and removes any that are to small, returns TRUE if at least 1 apoptosis is successful.
	virtual bool Apoptosis(ofstream* errorfile);

	//! Looks through all of the edges and find any that are to small and have them undergo T1 transitions, returns TRUE if successful.
	virtual bool T1(ofstream* errorfile);

	//! Looks through all the fourfold vertices and splits them along the lines of force if they are unstable (or keeps them if they are stable), returns TRUE if any changes happened.
	bool ResolveVertices(ofstream* errorfile);

	//************************************************************************************************************
	//******************* INPUT OUTPUT HANDELING  ****************************************************************
	//************************************************************************************************************

	//! Reads and constructs simulation from file. The file must be formatted like InputTemplate.txt. This function parses the input file then initializes all the objects and pointers.
	virtual bool ReadInputData(string InputFileName);

	//! Similar to WriteOutputData(), but with additional informations (cell area, pressure, etc) appended to create interactive/clickable plot in MATLAB. Actual implementation is in HelperWriteOutputData_Write2File().
	virtual void WriteOutputData4Matlab(string fileName);

	//! Similar to WriteOutputData(), but writes/appends to an existing file instead of opening a new one. Useful for output over regular interval, e.g. for making movie in MATLAB.
	virtual void WriteOutputDataAppend(ofstream* datafile);

	

	//************************************************************************************************************
	//******************* HELPER FUNCTIONS ****************************************************************
	//************************************************************************************************************

	//Integrates the forcefunction by one euiler step 
	virtual void RK4Helper_EuilerStep(double dt, ofstream* errorfile);
	
	//! Reads the data and sets all of the global variables.
	//! Starts at current position of *datafile and look for keyword 'Global', reads until keyword 'endGlobal'.
	virtual void HelperReadInputData_SetGlobalValues(ifstream* datafile);

	//! Make all of the objects with the correct starting positions and flags and recordds their ids in the tissue's list of ids. Starts at current position of *datafile and reads until keyword 'End'.
	virtual void HelperReadInputData_CreateObjects(ifstream* datafile);
	 
	//! Initializes all of the pointers so that each object has pointers to it's neighbors.
	virtual void HelperReadInputData_CreatePointers();

	//! Writes data into file specified by 'datafile'
	virtual void HelperWriteOutputData_Write2File(ofstream* datafile, bool includeExtraInfo);

	//! RemoveEdge deletes an edge and merges the two vertices to create a fourfold vertex
	virtual Vertex* HelperT1_RemoveEdge(Edge* deledge, ofstream* errorfile, int* nT1);

	//! Removes a cell from the tissue and creates a new vertex at its center which is attached to the propper edges.
	//! Returns false if cell is not deleted due to 5-fold vertices, true otherwise.
	bool HelperApoptosis_DeleteCell(Cell* delcell);

	//! Moves the vertices to their new positions and updates the approproate flags if they cross a boundary.
	virtual bool HelperEvolve_UpdatePosition();

	//! Build array of vertex positions (x and y) as well as LX and LY based on current values.
	void Store_Initial_Condition(vector<Vect> &y);

	//! Update rold with specified value.
	void Update_Old_Mechanical_Variables(vector<Vect> &y);

	//! Store the updated positions (from any of the mechanical position evolvers).
	void Store_New_Positions(vector<Vect> &y);

	//! Step size control mechanical evolution
	virtual bool Double_Step_Size_Control(const vector<Vect> &y_error, const double error_tolerance_absolute, double* initial_step_size, double consistency_order);

	//! Does the same thing as HelperEvolve_UpdatePosition but to a single vertex.
	void HelperResolve_UpdatePosition(Vertex* vptr);

	//! This is the helper function that actually do mitosis on cells.
	virtual void HelperMitosis(Cell * parentcell, double divisionangle);

	//! Resolves a forfold vertex into 2 threefold vertices if it is energeticaly favorable to do so, returns TRUE if the resolve is successful.
	bool HelperResolveVertices_ResolveFourfoldVertex(Vertex* v);

	//! Called by HelperResolveVertices_ResolveFourfoldVertex to check the stability of one configuration
	//! Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors
	//! returns the magnutude of Force-Forcenewedge and changes the value of Force
	virtual double HelperResolveVertices_CheckStability(Vertex* v, Edge* e1, Edge* e2, Edge* e3, Edge* e4, Cell* c1, Cell* c2, Cell* c3, Cell* c4, Vect *Force);

	//! Called by HelperResolveVertices_ResolveFourfoldVertex to update the pointers when the vertex needs to resolve, Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors 
	Edge* HelperResolveVertices_UpdatePtrs(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force);

	//! Writes the current state of the simulation to file. The file is formatted like InputTemplate.txt and thus can be readily read by ReadInputData(). Actual implementation is in HelperWriteOutputData_Write2File().
	//! \note This function will only write the essential data, enough for ReadInputData() to reconstruct the simulation state. All other information (e.g. cell area, pressure, etc) can be calculated from the data.
	//! \note For cases where this extra information is needed (e.g. for visualization with MATLAB, debugging, etc) use function WriteOutputData4Matlab(string fileName) instead.
	virtual void WriteOutputData(string fileName);

	//Assigns the cells in the tissue A0 based on an inverse Gaussian Distrabution with skew lambda
	void AssignA0_InverseGaussian(double lambda); 

	//Returns the average edge length 
	double GetAvgEdgeLength(); 
};

double inverseGaussian(double mu, double lambda);

void calculateexternalstress_linear(double maxtime); 


#endif