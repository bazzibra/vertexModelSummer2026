/**	\file	TissueClass.cpp
*	Implementation of Tissue class.
*	Contains function definitions associated with Tissue object.
*/

#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector> 
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <time.h>
#include <chrono>
#include <random>
#include <ctime>
//#include <omp.h>
#include <typeinfo>
#include <cassert> 
#include <stddef.h>

#include "Constants.h"
#include "VertexClass.h"
#include "CellClass.h"
#include "EdgeClass.h"
#include "TissueClass.h"

using namespace std;

// Creates a new vertex and adds it to the list of vertices, returns a pointer to that vertex.
Vertex* Tissue::CreateVertex(Vect position, vector<Edge*> edgs){
	int largestID, newid;

	//Get the id
	largestID = (*max_element(vertexIDs.begin(), vertexIDs.end()));			//first look through all of the current vertex ids and get the highest
	newid = largestID + 1;														//Now make a new ID that is unique
	if (newid > 1000000000) { cout << "ERROR to many vertices" << endl; }

	//create the vertex and add the vertex to the lists
	Vertex* vptr = new Vertex;
	(*vptr).id = newid;
	(*vptr).r = position;
	(*vptr).edges = edgs;
	(vertices).push_back(vptr);
	vertexIDs.push_back(newid);

	return vptr;
}

// Removes vertex 'vert' from the master list.
void Tissue::DestroyVertex(Vertex* vptr){
	list<Vertex*>::iterator vit;
	Vertex v;

	for (vit = vertices.begin(); vit != vertices.end(); vit++){		//loop through all of the vertices in the tissue
		if ((*(*vit)).id == (*vptr).id){												//if the curent vertex is one of the vertices to be removed
			delete (*vit);
			vertices.erase(vit);													//remove it
			break;
		}
	}
}

// Creates a new edge and addes it to the list of edges, returns a pointer to that edge.
Edge* Tissue::CreateEdge(Edge* subclass, bool xflg, bool yflg, Vertex* v1, Vertex* v2, Cell* c1, Cell* c2){
	int largestID, newid;
	Edge* newedge{};
	//Get the id
	largestID = (*max_element(edgeIDs.begin(), edgeIDs.end()));			//first look through all of the current vertex ids and get the highest
	newid = largestID + 1;								//Now make a new ID that is unique

	if (newid > 1000000000) { 
		cout << "ERROR to many edges" << endl; 
		return NULL; 
	}
	//create the vertex and add the vertex to the lists
	Edge* eptr = new Edge;
	(*eptr).id = newid;
	(*eptr).xflag = xflg;
	(*eptr).yflag = yflg;
	(*eptr).vertex1 = v1;
	(*eptr).vertex2 = v2;
	(*eptr).cell1 = c1;
	(*eptr).cell2 = c2;
	(edges).push_back(eptr);
	edgeIDs.push_back(newid);

	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		if ((*(*eit)).id == newid){
			//return &(*(*eit));
			newedge = &(*(*eit));
			break;
		}
	}
	return newedge; 
}

// Removes the vertex from the master list.
void Tissue::DestroyEdge(Edge* eptr){
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){			//loop through all of the edges in the tissue
		if ((*(*eit)).id == (*eptr).id){										//if the curent vertex is one of the vertices to be removed
			delete (*eit);
			edges.erase(eit);												//remove it
			break;
		}
	}
}

// Creates a new cell and addes it to the list, returns a pointer to that cell.
Cell* Tissue::CreateCell(int typ, vector<Edge*> edgs){
	int largestID, newid;
	list<Cell*>::iterator cit;
	Cell* newcell{};
	//Get the id
	largestID = (*max_element(cellIDs.begin(), cellIDs.end()));			//first look through all of the current vertex ids and get the highest
	newid = largestID + 1;								//Now make a new ID that is unique

	if (newid > 1000000000) { cout << "ERROR to many cells" << endl; }
	//create the cell and add the cell to the lists
	Cell* cptr = new Cell;
	(*cptr).id = newid;
	(*cptr).type = typ;
	(*cptr).edges = edgs;
	(cells).push_back(cptr);
	cellIDs.push_back(newid);

	for (cit = cells.begin(); cit != cells.end(); cit++){
		if ((*(*cit)).id == newid){
			newcell = &(*(*cit));
			break;
		}
	}
	return newcell; 
}

// Removes the cell from the master list.
void Tissue::DestroyCell(Cell* cptr){
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){			//loop through all of the cells in the tissue
		if ((*(*cit)).id == (*cptr).id){							//if the curent cell is one of the cells to be removed
			delete (*cit); 
			cells.erase(cit);								//remove it
			break;
		}
	}
}

// Do mitosis on cells. Picks cells for mitosis based on certain criteria and calls HelperMitosis(Cell * parentcell) to perform mitosis on those cells.
bool Tissue::Mitosis(){
	vector<Cell*> mitcells;
	int flag = 0;
	int i;
	uniform_real_distribution<double> dist(0.0, 1.0);
	double probmitosis, randnumb; 
	double carea; 
	double angle; 
	uniform_real_distribution<double> angledist(0.0, 2 * PI);

	//Here is where we will later insert some criteria for mitosis
	//for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){
	//	carea = (*(*cit)).GetArea((*(*cit)).SavedInfoForEvolve_CWVerts); 
	//	probmitosis = 0.5*INITIALSTEPSIZE*exp(15*((carea/(1.25*A0))-1)); //probability of mitosis is based on your size
	//	randnumb = dist(generator); 
	//	if ( randnumb < probmitosis ) {
	//		//cout << "pmitosis  " << probmitosis  <<  "   rand numb   " << randnumb << endl; 
	//		mitcells.push_back(&(*(*cit)));
	//		flag++;
	//	}
	//}
	if (CURRENTTIME<20)
		probmitosis = 0; //probability of mitosis
	else
		probmitosis = 16 * INITIALSTEPSIZE;
	randnumb = dist(generator);
	if (randnumb < probmitosis) {
		for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){
		//Get the first cell that has not yet devided 
			if ((*(*cit)).type == 0){
				mitcells.push_back(&(*(*cit)));
				flag++;
				break;
			}	
		}
	}
	if (flag == 0){
		return false;
	}
	else{
		//Output the cell id and area for all cells undergoing Mitosis
		for (i = 0; i<mitcells.size(); i++){
			//NOTE: only perform mitosis on cells that are convex in order to avoid cutting more than two edges.
			if (CheckConvex((*mitcells[i]).HelperOrderVerticesPos()) == true){
				angle = angledist(generator);
				(*this).HelperMitosis(mitcells[i], angle);
			}
		}
		return true;
	}
}

// Looks thourgh all of the cells in the tissue and removes any that are to small, returns TRUE if at least 1 apoptosis is successful.
/*!
\par Pseudocode
\code{.cpp}
startfcn:
for each cell, do:
if the cell area < minimum area
Record the cell
Set flag = TRUE
endif
endfor

if flag = FALSE
return FALSE
else
for each recorded cell, do
call HelperApoptosis_DeleteCell()
endfor
endif

if at least one apoptosis was successful:
return TRUE
else
return FALSE

\endcode
*/
//TEMP (MONITOR APOPTOSIS WHEN RELAXING CIRCULAR PACKING)
//Note: to debug, take as input ofstream* datafile, then use (*this).WriteOutputDataAppend(datafile); before and after HelperApoptosis_DeleteCell
bool Tissue::Apoptosis(ofstream* errorfile) {
	int flag = 0;
	int i;
	vector<Cell*> apopcells;
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){			//look through all of the cells
		if ((*(*cit)).GetArea() < MINAREA){
			apopcells.push_back(&(*(*cit)));
			cout << "APOPTOSIS OF CELL " << (*(*cit)).id << endl;
			flag++;
		}
	}
	if (flag == 0)
		return false;
	else{
		//(*this).WriteOutputDataAppend(errorfile);
		bool isApoptosisSuccessful = false;
		for (i = 0; i<apopcells.size(); i++){
			bool status = HelperApoptosis_DeleteCell(apopcells[i]);
			isApoptosisSuccessful = isApoptosisSuccessful || status;
		}
		//(*this).WriteOutputDataAppend(errorfile);
		if (isApoptosisSuccessful){ return true; }
		else { return false; }
	}
}

// Looks through all of the edges and find any that are to small and have them undergo T1 transitions, returns TRUE if successful.
/*!
\par Pseudocode
\code{.cpp}
startfcn:
for each edge in the tissue, do:
if length < minlength, then:
record the edge
set the flag = TRUE
endif
endfor

if flag is FALSE, then:
return FALSE
else
for each recorded edge, do:
call HelperT1_RemoveEdge()
endfor

if the T1 is successful
return TRUE
else
return FALSE
endif
endif

endfcn
\endcode
*/
//Note: to debug, take as input ofstream* datafile, then use (*this).WriteOutputDataAppend(datafile); before and after HelperT1_RemoveEdge

bool Tissue::T1(ofstream* errorfile){
	int flag = 0;
	int i;
	vector<Edge*> t1edges;								//list to store the edges to t1 in (you must put them in a seprate list and then t1 them or the number of edges will change and the itterator will not function properly
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		if ((*(*eit)).GetLength() < MINLENGTH){
			t1edges.push_back(&(*(*eit)));
			flag++;
		}
	}
	if (flag == 0){
		return false;
	}
	else{
		//Output state of system before and after T1.
		//(*this).WriteOutputDataAppend(errorfile);
		for (i = 0; i<t1edges.size(); i++){
			cout << "T1 of Edge " << (*t1edges[i]).id << "   at location   (" << (*t1edges[i]).GetCenter().x << " , " << (*t1edges[i]).GetCenter().y<< ")" << endl;
			int nT1; 
			Vertex* v = (*this).HelperT1_RemoveEdge(t1edges[i],errorfile, &nT1);
		   //(*this).WriteOutputDataAppend(errorfile);
			HelperResolveVertices_ResolveFourfoldVertex(v); 
		}
		// (*this).WriteOutputDataAppend(errorfile);
		return true; 
	}
}

// Looks through all the fourfold vertices and splits them along the lines of force if they are unstable (or keeps them if they are stable), returns TRUE if any changes happened.
//Note: to debug, take as input ofstream* datafile, then use (*this).WriteOutputDataAppend(datafile); before and after HelperResolveVertices_ResolveFourfoldVertex
bool Tissue::ResolveVertices(ofstream* errorfile){
	int flag = 0;
	int flg2 = 0;
	int i;
	vector<Vertex*> fourfoldresolved;
	list<Vertex*>::iterator vit;

	for (vit=vertices.begin(); vit!=vertices.end();  vit++){
		if( (*(*vit)).CheckFourfold() ){
			fourfoldresolved.push_back( *vit );
			flag++;
		}
	}
	if (flag == 0)
		return false;
	else{
		//(*this).WriteOutputDataAppend(errorfile);
		for (i = 0; i<fourfoldresolved.size(); i++){
			cout << "Resolving Vertex " << (*fourfoldresolved[i]).id << endl;
			flg2 = flg2 + (*this).HelperResolveVertices_ResolveFourfoldVertex(fourfoldresolved[i]);
		}
		//(*this).WriteOutputDataAppend(errorfile);
		if (flg2>0)
			return true;
		else
			return false;
	}
}

// Reads and constructs simulation from file. The file must be formatted like InputTemplate.txt. This function parses the input file then initializes all the objects and pointers.
bool Tissue::ReadInputData(string InputFileName){
	cout << "Entered RedInputData" << endl; 
	ifstream datafile;
	datafile.open(InputFileName);											//open file
	if (!datafile.is_open()){											//check if open file successful
		cout << "ERROR cannot open file: " << InputFileName << endl;
		return false; 
	}
	cout << InputFileName << " file opened sucessfully" << endl;
	(*this).HelperReadInputData_SetGlobalValues(&datafile);
	cout << "Global values Set" << endl; 
	(*this).HelperReadInputData_CreateObjects(&datafile);
	(*this).HelperReadInputData_CreatePointers();

	datafile.close();
	return true; 
}

// Similar to WriteOutputData(), but with additional informations (cell area, pressure, etc) appended to create interactive/clickable plot in MATLAB. Actual implementation is in HelperWriteOutputData_Write2File().
void Tissue::WriteOutputData4Matlab(string fileName){
	ofstream datafile;
	datafile.open(fileName);
	(*this).HelperWriteOutputData_Write2File(&datafile, true);
	datafile.close();

}

// Similar to WriteOutputData(), but writes/appends to an existing file instead of opening a new one. Useful for output over regular interval, e.g. for making movie in MATLAB.
// Actual implementation is in HelperWriteOutputData_Write2File().
void Tissue::WriteOutputDataAppend(ofstream* datafile){
	(*this).HelperWriteOutputData_Write2File(datafile, true);
}

////WARNING!!! This function assumes that cell.SavedInfoForEvolve_CWVerts has been updated correctly
//Calculates the force on every vertex and stores the result in vertex.Tissue subclasses have their own implementation

void Tissue::ForceFunction(ofstream* errorfile){
	uniform_real_distribution<double> distribution(0.0, 1.0);
	double area, deltaP;
	vector<vector<int>> cwflags;
	vector<Vertex*> cwverts;
	int i, cellnumber;
	Edge* eptr;
	double Pmag, angle;
	Vect FT;
	//Declare and initialize pointers.
	Vertex* v1 = 0;
	Vertex* v2 = 0;
	Cell* c1;
	Vect zerovec(0.0, 0.0);

	//First loop through all the cells and find the pressure WARNING!! cell.SavedInfoForEvolve_CWVerts must already be correctly assigned
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++)
	{
		cwverts = (*(*cit)).SavedInfoForEvolve_CWVerts;							//Get the cw vertices in the cell from the preassigned list 
		area = (*(*cit)).GetArea(cwverts);										//Get the area quickly using the pre assigned cw vertices
		(*(*cit)).SavedInfoForEvolve_Pressure = (*(*cit)).GetPressure(area);	//Get the pressure
	}

	//Calculate the force on each vertex from the pressure and tension
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).SavedInfoForEvolve_MechForce = zerovec; //first set the force to zero 
		//For each edge
		for (i = 0; i < (*(*vit)).edges.size(); i++){
			eptr = &(*(*(*vit)).edges[i]);
			//Get the force from the tension on the edge
			FT = (*eptr).GetTension_ForceOnVertex((*vit));
			//Get the force from the pressure difference across the edge
			v1 = (*eptr).GetOtherVertex((*vit));
			c1 = &(*(*eptr).cell1);
			v2 = (*c1).GetNextCWVert(*vit, (*c1).SavedInfoForEvolve_CWVerts);
			//Gets the CW ordering of cells 1 and 2 across eptr
			if ((*v1).id == (*v2).id)
			{
				deltaP = (*(*eptr).cell2).SavedInfoForEvolve_Pressure - (*(*eptr).cell1).SavedInfoForEvolve_Pressure; 
			}
			else
			{ 
				deltaP = (*(*eptr).cell1).SavedInfoForEvolve_Pressure - (*(*eptr).cell2).SavedInfoForEvolve_Pressure; 
			}
			Pmag = -0.5 * deltaP * (*eptr).GetLength();   		//magnitude of the force due to the pressure difference
			angle = (*eptr).GetAngle((*vit)) + PI / 2;	 	//get the angle zXl
			Vect FP(zerovec, angle, Pmag);
			//Add the forces on this edge to the vertex
			(*(*vit)).SavedInfoForEvolve_MechForce = (*(*vit)).SavedInfoForEvolve_MechForce + FT + FP;
		}
	}

	//Calculate the tissue stress
	double GammaX = 0; 
	double GammaY = 0; 
	// calculateexternalstress_linear(TAU_STRESS); 

 	double cutlineX = distribution(generator) * LX; //Continue to generate random numbers until you cut no concave cells
	GammaX = TISSUE_EXT_STRESS_X + (*this).CalculateTissueStressX(cutlineX, errorfile);
		
	double cutlineY = distribution(generator) * LY; //Continue to generate random numbers until you cut no concave cells
	GammaY = TISSUE_EXT_STRESS_Y + (*this).CalculateTissueStressY(cutlineY, errorfile);


	//Add the force from the tissue stress to every vertex
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).SavedInfoForEvolve_TotalForce.x = (*(*vit)).SavedInfoForEvolve_MechForce.x + (alphaM / TISSUE_ALPHA) * (*(*vit)).r.x * GammaX;
		(*(*vit)).SavedInfoForEvolve_TotalForce.y = (*(*vit)).SavedInfoForEvolve_MechForce.y + (alphaM / TISSUE_ALPHA) * (*(*vit)).r.y * GammaY;
	}

	
	//Update LY and LX
	ForceLX = LX * GammaX;
	ForceLY = LY * GammaY;
}

//Integrates the forcefunction by one euiler step 
void Tissue::Integrate_Euiler(double dt, ofstream* errorfile){
	//Calculate the force on every vertex 
	(*this).ForceFunction(errorfile);

	//loop through all the vertices and set rnew
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_TotalForce * (dt / alphaM) + (*(*vit)).rold;
	}

	//Set LX and LY new
	LXnew = (dt / TISSUE_ALPHA) * ForceLX + LXold;
	LYnew = (dt / TISSUE_ALPHA) * ForceLY + LYold;
}

//Integrates the force function by a 4th order Runge-Kutta algorithem 
void Tissue::Integrate_RK4(double dt, ofstream* errorfile){
	//We will take advantage of the fact that the vertex list does not change (in size or order) durring integration to store a list of the intermediate positions of the vertices 
	vector<Vect> k1, k2, k3, k4;
	double k1LX, k1LY, k2LX, k2LY, k3LX, k3LY, k4LX, k4LY;
	int i;

	//Make sure that you start with .r == .rold
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold;
		i++;
	}
	LX = LXold;
	LY = LYold;

	//k1=hf(y)
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		k1.push_back((*(*vit)).rnew);
	} 
	k1LX = LXnew;
	k1LY = LYnew;

	//k2=hf(y+k1/2)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold + k1[i] / 2.0;
		i++;
	}
	LX = LXold + k1LX / 2.0;
	LY = LYold + k1LY / 2.0;
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		k2.push_back((*(*vit)).rnew);
	}
	k2LX = LXnew;
	k2LY = LYnew;

	//k3=hf(y+k2/2)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold + k2[i] / 2.0;
		i++;
	}
	LX = LXold + k2LX / 2.0;
	LY = LYold + k2LY / 2.0;
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		k3.push_back((*(*vit)).rnew);
	}
	k3LX = LXnew;
	k3LY = LYnew;

	//k4=hf(y+k3)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold + k3[i];
		i++;
	}
	LX = LXold + k3LX;
	LY = LYold + k3LY;
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		k4.push_back((*(*vit)).rnew);
	}
	k4LX = LXnew;
	k4LY = LYnew;

	//Get rnew
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).rnew = (*(*vit)).rold + k1[i] / 6. + k2[i] / 3. + k3[i] / 3. + k4[i] / 6.;
		i++;
	}
	LXnew = LXold + k1LX / 6. + k2LX / 3. + k3LX / 3. + k4LX / 6.;
	LYnew = LYold + k1LY / 6. + k2LY / 3. + k3LY / 3. + k4LY / 6.;

	//save the old values again: in case step fails.
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold;
		i++;
	}
	LX = LXold;
	LY = LYold;
}

//! Adaptive evolver for mechanical variables (enforcing given error tolerances). Only evolve to time t1.
int Tissue::Integrate_RK4_Adaptive(double* step_size, const double error_abs_tolerance, double* t, double t1, ofstream* errorfile){
	//dimension of mechanical systems
	int dimension_mechanical = vertices.size() + 1;
	//Store the current step size (make sure the total scale factor is not out of range).
	double current_step_size = *step_size;
	//Define the size of a half step.
	double half_step = current_step_size / 2.;
	//Initialize the error check for entering while loop (step_error_check -> step needs to be scaled down).
	bool step_error_check = true;
	//These vectors are used for both half and full steps below as well as error tolerance and estimated error.
	vector<Vect> initial_condition;
	vector<Vect> y_error_tolerance;
	vector<Vect> error_vector;
	vector<Vect> full_step_solution;
	vector<Vect> second_half_step_solution;
	vector<Vect> first_half_step_solution;
	initial_condition.resize(dimension_mechanical);
	y_error_tolerance.resize(dimension_mechanical);
	error_vector.resize(dimension_mechanical);
	full_step_solution.resize(dimension_mechanical);
	second_half_step_solution.resize(dimension_mechanical);
	first_half_step_solution.resize(dimension_mechanical);

	//Store initial condition.
	(*this).Store_Initial_Condition(initial_condition);

	//The consistency order affects how the step size is scaled with observed error.
	double consistency_order = 4.0;

	while (step_error_check == true){
		//if the step will take you outside of the specified time, scale the stepsize down.
		if (*t + current_step_size > t1){
			current_step_size = t1 - *t;
		}

		//Reinitialize half_step_size (upon entering loop)
		half_step = current_step_size / 2.;

		//Take the first half step.
		(*this).Integrate_RK4(half_step, errorfile);
		//Store the solution from the first half step.
		(*this).Store_New_Positions(first_half_step_solution);

		//Make the first half step solution the initial condition for the second half step.
		(*this).Update_Old_Mechanical_Variables(first_half_step_solution);

		//Take the second half step.
		(*this).Integrate_RK4(half_step, errorfile);
		//Store the solution from the second half step.
		(*this).Store_New_Positions(second_half_step_solution);

		//restore initial conditions:
		(*this).Update_Old_Mechanical_Variables(initial_condition);

		//take a full step
		(*this).Integrate_RK4(current_step_size, errorfile);
		//store the full step solution.
		(*this).Store_New_Positions(full_step_solution);

		//restore initial conditions:
		(*this).Update_Old_Mechanical_Variables(initial_condition);

		//Estimate error by subtracting two half steps from full step.
		error_vector.resize(dimension_mechanical);
		for (int index = 0; index != dimension_mechanical; index++){
			error_vector.at(index) = full_step_solution.at(index) - second_half_step_solution.at(index);
			error_vector.at(index) = error_vector.at(index).Abs();
		}

		//Check whether step size is too large. (if it is too large, this function will return step_error_check != 0)
		step_error_check = Double_Step_Size_Control(error_vector, error_abs_tolerance, &current_step_size, consistency_order);

		//if the step size was scaled down, reinitialize initial_step_size for reentering while loop.
		if (step_error_check == true){
			*step_size = current_step_size;
		}
	}

	//Current step size may have been scaled up by the adaptive step function; actual step used is 2 * half_step.
	double used_step_size = 2 * half_step;

	//Increment the time by the utilized step size (once the step is successful).
	if (step_error_check == 0){
		//It is important to increment the time by two half steps (because the variable current_step_size may have been scaled up).
		*t += used_step_size;
	}

	//Store the current_step_size in step size if it exceeds the step size.
	if (current_step_size > *step_size) *step_size = current_step_size;

	//Return success code.
	return step_error_check;
}

// s the tissue one time interval and updates the vertex positions
void Tissue::Evolve(double interval, double* stepsize, ofstream* errorfile){
	//find the end point of this evolve
	double endtime = CURRENTTIME + interval;
	bool reinitialize = true;
	bool flag0, flag1, flag2, flag3, flag4;
	int count = 0;

	while (CURRENTTIME < endtime){
		count++;
		//STEP 1: If the topology has changed get a list of the clockwise vertices in each cell
		if (reinitialize){																	//If their has been any topological change OR vertices have crossed a boundary 
			for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){		//Loop through the cells 
				(*(*cit)).SavedInfoForEvolve_CWVerts = (*(*cit)).GetClockWiseVertices();	//and get the vertices in the correct clockwise order 
				reinitialize = false;
			}
		}

		//STEP 2: Integrate 
		//Loop through the vertices and record the current position in rold 
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) 
		{ 
			(*(*vit)).rold = (*(*vit)).r; 
		}
		LXold = LX;				//Record the old box size
		LYold = LY;

		//******CHOOSE YOUR INTEGRATOR******
		if (INTEGRATORNAME == "RK4_Adaptive"){
			//4th order Runge-Kutta wi th adaptive stepsize
			(*this).Integrate_RK4_Adaptive(stepsize, error_tolerance_absolute_mechanical, &CURRENTTIME, endtime, errorfile); //Evolve the vertices and box size dimensions with the given error tolerance (adaptive step size). time is incremented within this function.
		}
		else if (INTEGRATORNAME == "RK4"){
			//4th order Runge-Kutta with fized stepsize
			(*this).Integrate_RK4(*stepsize, errorfile);
			CURRENTTIME += *stepsize;	//Update the current time
		}
		else if (INTEGRATORNAME == "Euiler"){
			//Simple Euiler with Fixed stepsize
			(*this).Integrate_Euiler(*stepsize, errorfile);
			CURRENTTIME += *stepsize;	//Update the current time
		}
		else{ cout << "ERROR invalid integrator name!"; }

		//Step 3: Update
		LX = LXnew;				//Update the current box size 
		LY = LYnew;
		(*this).HelperEvolve_UpdatePosition();  //Update the vertex positions (this automaticaly updates edge flags if a vertex has crossed the boundary)

		//STEP 4: look for topological changes and set flags if any have occured 
		flag1 = (*this).Apoptosis(errorfile);
		flag2 = (*this).T1(errorfile);
		flag3 = (*this).ResolveVertices(errorfile);
		if (CURRENTTIME > TAU_STRESS){
			flag4 = (*this).Mitosis();
			// THIS ACTIVATES MITOSIS!!!
			//(*this).WriteOutputDataAppend(errorfile);
		}
		else{
			flag4 = false;
		}
		if (flag1 || flag2 || flag3 || flag4) reinitialize = true;  //Determine if we need to reinitialize
	}

	cout << "TISSUE EVOLVED TO TIME " << CURRENTTIME << " in " << count << " steps" << endl;
	cout << "LX = " << LX << "  LY = " << LY << endl;
}

//Calculate Tissue Stress in both directions for evolving step size.
double Tissue::CalculateTissueStressX(double xCut, ofstream* errorfile){
	bool xflag, c1, c2;
	double x1, x2, y1, y2, theta;
	Vertex *v1, *v2;
	vector<Edge*> crossingEdges;									//vector of all crossing edges
	vector<Cell*> crossingCells;									//vector of all crossing cells
	Vect origon(0.0, 0.0);
	int i;
	Cell *cptr;
	double length;

	//Find the edges and cells that cross the cutline
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		v1 = (*(*eit)).vertex1;
		v2 = (*(*eit)).vertex2;
		xflag = (*(*eit)).xflag;
		x1 = (*v1).r.x;
		x2 = (*v2).r.x;
		//test if edge crosses xCut (taking PBC into account)
		bool isCrossing = (!xflag && x1 < xCut && x2 > xCut) || (!xflag && x1 > xCut && x2 < xCut)				//if xflag = false and x1 & x2 are on the OPPOSITE side of xCut
			|| (xflag && x1 < xCut && x2 < xCut) || (xflag && x1 > xCut && x2 > xCut);							//if xflag = true and x1 & x2 are on the SAME side of xCut
		if (isCrossing)
		{
			crossingEdges.push_back(&(*(*eit)));
			//if the cells are not in the list of crossing cells already add them 
			long int c1id = (*(*(*eit)).cell1).id;
			long int c2id = (*(*(*eit)).cell2).id;
			c1 = false;
			c2 = false;
			for (i = 0; i < crossingCells.size(); i++)
			{
				if ((*crossingCells[i]).id == c1id) c1 = true;
				if ((*crossingCells[i]).id == c2id) c2 = true;
			}
			if (!c1) crossingCells.push_back(&(*(*(*eit)).cell1));
			if (!c2) crossingCells.push_back(&(*(*(*eit)).cell2));
		}
	}

	//For each edges in crossingEdges, get x-component of tension and accumulate into Ftension
	double Ftension = 0.0;
	for (i = 0; i < crossingEdges.size(); i++){
		v1 = (*crossingEdges[i]).vertex1;
		v2 = (*crossingEdges[i]).vertex2;
		x1 = (*v1).r.x;
		y1 = (*v1).r.y;
		x2 = (*v2).r.x;
		y2 = (*v2).r.y;
		theta = (*crossingEdges[i]).GetAngle(x1, y1, x2, y2);				//This deals with PBC by getting the flags its self
		Vect Tension(origon, theta, (*crossingEdges[i]).GetTension_Magnitude());
		Ftension += fabs(Tension.x);
	}

	//For each cell in crossingCells:
	double Fpressure = 0.0;									//the total pressure allong this cutline
	for (i = 0; i < crossingCells.size(); i++){				//for each cell
		vector<vector<double>> xypos = (*crossingCells[i]).HelperOrderVerticesPos((*crossingCells[i]).SavedInfoForEvolve_CWVerts);
		length = (*crossingCells[i]).GetLengthVertCut(xCut, xypos);
		Fpressure += length * (*crossingCells[i]).SavedInfoForEvolve_Pressure;
	} //end for each cell in crossingCells

	//Calculate tissue stress
	return (Fpressure - Ftension) / LY;
}

double Tissue::CalculateTissueStressY(double yCut, ofstream* errorfile){
	bool yflag, c1, c2;
	double x1, x2, y1, y2, theta;
	Vertex *v1, *v2;
	vector<Edge*> crossingEdges;									//vector of all crossing edges
	vector<Cell*> crossingCells;									//vector of all crossing cells
	Vect origon(0.0, 0.0);
	int i;
	Cell *cptr;
	double length;

	//Find the edges and cells that cross the cutline
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		v1 = (*(*eit)).vertex1;
		v2 = (*(*eit)).vertex2;
		yflag = (*(*eit)).yflag;
		y1 = (*v1).r.y;
		y2 = (*v2).r.y;
		//test if edge crosses xCut (taking PBC into account)
		bool isCrossing = (!yflag && y1 < yCut && y2 > yCut) || (!yflag && y1 > yCut && y2 < yCut)				//if xflag = false and x1 & x2 are on the OPPOSITE side of xCut
			|| (yflag && y1 < yCut && y2 < yCut) || (yflag && y1 > yCut && y2 > yCut);					//if xflag = true and x1 & x2 are on the SAME side of xCut
		if (isCrossing){
			crossingEdges.push_back(&(*(*eit)));
			//if the cells are not in the list of crossing cells already add them 
			long int c1id = (*(*(*eit)).cell1).id;
			long int c2id = (*(*(*eit)).cell2).id;
			c1 = false;
			c2 = false;
			for (i = 0; i < crossingCells.size(); i++){
				if ((*crossingCells[i]).id == c1id) c1 = true;
				if ((*crossingCells[i]).id == c2id) c2 = true;
			}
			if (!c1) crossingCells.push_back(&(*(*(*eit)).cell1));
			if (!c2) crossingCells.push_back(&(*(*(*eit)).cell2));
		}
	}

	//For each edges in crossingEdges, get x-component of tension and accumulate into Ftension
	double Ftension = 0.0;
	for (i = 0; i < crossingEdges.size(); i++){
		v1 = (*crossingEdges[i]).vertex1;
		v2 = (*crossingEdges[i]).vertex2;
		x1 = (*v1).r.x;
		y1 = (*v1).r.y;
		x2 = (*v2).r.x;
		y2 = (*v2).r.y;
		theta = (*crossingEdges[i]).GetAngle(x1, y1, x2, y2);									//This deals with PBC by getting the flags its self
		Vect Tension(origon, theta, (*crossingEdges[i]).GetTension_Magnitude());
		Ftension += fabs(Tension.y);
	}

	//For each cell in crossingCells:
	double Fpressure = 0.0;									//the total pressure allong this cutline
	for (i = 0; i < crossingCells.size(); i++){				//for each cell
		vector<vector<double>> xypos = (*crossingCells[i]).HelperOrderVerticesPos((*crossingCells[i]).SavedInfoForEvolve_CWVerts);
		length = (*crossingCells[i]).GetLengthHorzCut(yCut, xypos);
		Fpressure += length * (*crossingCells[i]).SavedInfoForEvolve_Pressure;
	} //end for each cell in crossingCells

	//Calculate tissue stress
	return (Fpressure - Ftension) / LX;
}

//HELPER FUNCTIONS

//HELPER CHANGE.
//Integrates the forcefunction by one euiler step 
void Tissue::RK4Helper_EuilerStep(double dt, ofstream* errorfile){
	//Calculate the force on every vertex 
	(*this).ForceFunction(errorfile);

	//loop through all the vertices and set rnew
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_MechForce * dt;
	}

	//Set LX and LY new
	LXnew = dt * ForceLX;
	LYnew = dt * ForceLY;

}

// RemoveEdge deletes an edge and merges the two vertices to create a fourfold vertex
Vertex* Tissue::HelperT1_RemoveEdge(Edge* deledge, ofstream* errorfile, int* nT1){
	int newfold;					//is the new vertex 4 or 5 fold
	int i, j, k, l, m;
	Vertex *v1ptr, *v2ptr, *vnewptr;			//pointers to the two merging vertices
	Cell *c1ptr, *c2ptr;			//pointers to the two cells that will loose an edge (the other cells remain uneffected b/c their edges do not change and they do not know their vertices)
	vector<Edge*> newedgelist;

	v1ptr = (*deledge).vertex1;
	v2ptr = (*deledge).vertex2;
	c1ptr = (*deledge).cell1;
	c2ptr = (*deledge).cell2;

	//If this will create a 5 fold vertex give an error
	newfold = (*v1ptr).GetEdgeNumber() + (*v2ptr).GetEdgeNumber() - 2;
	if (newfold>4){
				cout << "ERROR: DID NOT COLLAPSE AN EDGE TO GET A 5FOLD VERTEX " << endl;
		return NULL;
	}

	//If this will create a cell with only two edges give an error (ie if the cell on either side of the deleted edge is a triangle)
	if ((*c1ptr).GetVertexNumber()<4 || (*c2ptr).GetVertexNumber()<4){
				cout << "ERROR: DID NOT COLLAPSE AN EDGE THAT IS PART OF A TRIANGLE" << endl;
		return NULL;
	}

	//If there are no errors continue

	//Record the number of t1s
	*nT1 = (*deledge).numbT1; 

	//fill the new edge list
	for (i = 0; i<(*v1ptr).edges.size(); i++){					//get the edges from vertex1
		if ((*deledge) != (*(*v1ptr).edges[i])){
			newedgelist.push_back((*v1ptr).edges[i]);
		}
	}
	for (j = 0; j<(*v2ptr).edges.size(); j++){					//get the edges from vertex 2
		if ((*deledge) != (*(*v2ptr).edges[j])){
			newedgelist.push_back((*v2ptr).edges[j]);
		}
	}

	//FLAGS SET**********************************************************
	//find the new position and move the vertices there
	(*deledge).MoveVertsToCenter();
	(*this).HelperResolve_UpdatePosition(v1ptr);
	(*this).HelperResolve_UpdatePosition(v2ptr);

	//make the new vertex
	vnewptr = (*this).CreateVertex((*deledge).GetCenter(), newedgelist);

	//go to the four edges and change their ptrs from the old vertices to the new four fold vertex
	for (k = 0; k< newedgelist.size(); k++){
		if ((*(*newedgelist[k]).vertex1).id == (*v1ptr).id){
			(*newedgelist[k]).vertex1 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex1).id == (*v2ptr).id){
			(*newedgelist[k]).vertex1 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex2).id == (*v1ptr).id){
			(*newedgelist[k]).vertex2 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex2).id == (*v2ptr).id){
			(*newedgelist[k]).vertex2 = vnewptr;
		}
		else { cout << "ERROR IN DELETE EDGE" << endl; }
	}

	//Remove the old vertices from the master list
	(*this).DestroyVertex(v1ptr);
	(*this).DestroyVertex(v2ptr);

	//Remove the edge from the 2 cell lists of edges
	for (l = 0; l<(*c1ptr).edges.size(); l++){
		if ((*(*c1ptr).edges[l]).id == (*deledge).id) {
			(*c1ptr).edges.erase((*c1ptr).edges.begin() + l);
			break;
		}
	}
	for (m = 0; m<(*c2ptr).edges.size(); m++){
		if ((*(*c2ptr).edges[m]).id == (*deledge).id) {
			(*c2ptr).edges.erase((*c2ptr).edges.begin() + m);
			break;
		}
	}


	//Remove the edge from the master list
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){							//loop through all of the edges in the tissue
		if ((*(*eit)).id == (*deledge).id){													//if the curent edge is to be removed
			DestroyEdge(*eit);
			break;
		}
	}
	
	//return a pointer to the vertex
	return vnewptr; 
}

//! Removes a cell from the tissue and creates a new vertex at its center which is attached to the propper edges.
//! Returns false if cell is not deleted due to 5-fold vertices, true otherwise.
bool Tissue::HelperApoptosis_DeleteCell(Cell* delcell){
	int i, j, k, l, m, n, q, u, w;				//counters
	Vect center;
	Vertex* vptr;					//pointer to the new vertex
	vector<Vertex*> cellverts;		//the vertices in the cell
	vector<int> cellvertsid;		//the ids of the vertices in the cell
	vector<Edge*> keepedges;		//list of edges normal to the cell to be attached to the new vert
	int v1id, v2id;					//ids of the two vertices on an edge
	Edge* eptr;
	Vect predefinedcenter;
	list<Vertex*>::iterator vit;


	//first get a list of all of the edges normal to the cell (ie the ones that will remain once the cell is removed)
	cellverts = (*delcell).GetVertices();
	for (i = 0; i< cellverts.size(); i++){						//for all of the vertices in the cell
		cellvertsid.push_back((*cellverts[i]).id);			//first record the id for latter use
		for (j = 0; j < (*cellverts[i]).edges.size(); j++){		//look at all of the edges
			keepedges.push_back((*cellverts[i]).edges[j]);	//and add them to the list
		}
	}																	//now we need to remove the edges that make up the cell from keepedges 
	for (k = 0; k< (*delcell).edges.size(); k++){							//for all of the endges in the cell
	startloop:
		for (l = 0; l < keepedges.size(); l++){							//look at all of the edges in keep edges
			if (*(*delcell).edges[k] == *keepedges[l]){				//if they are the same edge remove the edge from keepedges
				keepedges.erase(keepedges.begin() + l);
				goto startloop;
			}
		}
	}


	//If a fivefold vertex will be created give an error (b/c currently we have no way of resolving these vertices) note that the function it's self is happy to remove a cell of any shape
	if (keepedges.size() > 4) {
		cout << "ERROR: A CELL IS BEING REMOVED TO FORM A FIVE FOLD VERTEX" << endl;
		return false;
	}


	predefinedcenter = (*delcell).GetCenter();
	//move all of the vertices in the cell to the center so that the edge flags will update apropreatly
	(*delcell).MoveVertsToCenter();
	for (w = 0; w<cellverts.size(); w++){
		(*this).HelperResolve_UpdatePosition(cellverts[w]);
	}


	//make a new vertex with these edges at the center of the cell
	vptr = (*this).CreateVertex(predefinedcenter, keepedges);

	//change the edge pointers to point to the new vertex
	for (m = 0; m< keepedges.size(); m++){
		v1id = (*(*keepedges[m]).vertex1).id;
		v2id = (*(*keepedges[m]).vertex2).id;
		if (find(cellvertsid.begin(), cellvertsid.end(), v1id) != cellvertsid.end()){			 //if vertex1 is in the cell 
			(*keepedges[m]).vertex1 = vptr;														//replace it with the new vertex
		}
		else if (find(cellvertsid.begin(), cellvertsid.end(), v2id) != cellvertsid.end()){			 //if vertex2 is in the cell 
			(*keepedges[m]).vertex2 = vptr;														//replace it with the new vertex
		}
		else { cout << "ERROR IN REMOVE CELL" << endl; }
	}

	//remove the cell's vertices from the master list
	for (n = 0; n<cellverts.size(); n++){
		for (vit = vertices.begin(); vit != vertices.end(); vit++){
			if ((*(*vit)) == (*cellverts[n])){
				DestroyVertex((*vit));
				break;
			}
		}
	}
	//remove the cell's edges from the master list and also from the cell on the other side of the edge
	for (q = 0; q<(*delcell).edges.size(); q++){
		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
			eptr = &(*(*eit));
			if (*eptr == (*(*delcell).edges[q])){								//if the edge is to be deleted				
				if (*(*(*eit)).cell1 == *delcell) {						//determine which cell is on the other side of the edge
					(*(*eptr).cell2).HelperRemoveEdge(eptr);							//and erase the edge from the cell2 or cell 1
				}
				else if (*(*(*eit)).cell2 == *delcell){
					(*(*eptr).cell1).HelperRemoveEdge(eptr);
				}
				DestroyEdge(*eit);
				break;
			}
		}
	}
	//remove the cell from the master list
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){						//loop through all of the cells in the tissue
		if ((*(*cit)) == (*delcell)){													//if the curent cell is to be removed
			(*this).DestroyCell((*cit));															//remove it
			break;
		}
	}
	//Call resolve on the vertex if needed
	if ((*vptr).CheckFourfold()) { (*this).HelperResolveVertices_ResolveFourfoldVertex(vptr); }

	return true;
}

// Reads the data and sets all of the global variables
// Starts at current position of *datafile and look for keyword 'Global', reads until keyword 'endGlobal'.
void Tissue::HelperReadInputData_SetGlobalValues(ifstream* datafile){

	string line, word, key, value;		//temporary variables to store lines and words 
	string type;						//what type of file is it (in case latter versions need to do slightly different things based on type)
	stringstream ssline;				//ssline is a string stream so we can comvert strings to stringstreams
	map <string, string> globals;



	//******ADD MORE GLOBAL VARIABLES HERE ALSO ADD THEM TO THE CONSTANTS.H FILE AND GIVE THEM A DEFAULT VALUE IN MAIN*********************//

	//Read until you get to the word Global signifying the begining of the global variables
	while (line.compare("Global:") != 0){
		getline(*datafile, line);
		cout << line << endl; 
	}

	//Now read the global variables and make a map with the keywords
	while (line.compare("endGlobal") != 0){
		getline(*datafile, line);							//get the next line
		ssline.clear();
		ssline << line;										//put it into a stream
		ssline >> key;										//the first word is the key
		ssline >> value;									//the second is the value of the key
		if (value != ""){									//if the item actually has a value
			globals[key] = value;								//stick the item and it's value in the globals map
		}
	}

	//Now assign the constants their values
	//makes sure the key is actually part of the map before giving the constant a value if the key is not defined it will have the value given at the begining of main
	// the atof is there to convert the string to a double
	if (globals.count("LX") == 1){ LX = atof(globals["LX"].c_str()); }
	else if (globals.count("LX") == 0){ cout << "WARNING: LX is not given in input file, used default value " << LX << endl; }
	else { cout << "ERROR: Global variable LX has multiple definition in input file, used default value LX = " << LX << endl; }

	if (globals.count("LY") == 1){ LY = atof(globals["LY"].c_str()); }
	else if (globals.count("LY") == 0){ cout << "WARNING: LY is not given in input file, used default value " << LY << endl; }
	else { cout << "ERROR: Global variable LX has multiple definition in input file, used default value LY = " << LY << endl; }

	if (globals.count("MINAREA") == 1){ MINAREA = atof(globals["MINAREA"].c_str()); }
	else if (globals.count("MINAREA") == 0){ cout << "WARNING: MINAREA is not given in input file, used default value " << MINAREA << endl; }
	else { cout << "ERROR: Global variable MINAREA has multiple definition in input file, used default value MINAREA = " << MINAREA << endl; }

	if (globals.count("MINLENGTH") == 1){ MINLENGTH = atof(globals["MINLENGTH"].c_str()); }
	else if (globals.count("MINLENGTH") == 0){ cout << "WARNING: LX is not given in input file, used default value " << MINLENGTH << endl; }
	else { cout << "ERROR: Global variable MINLENGTH has multiple definition in input file, used default value MINLENGTH = " << MINLENGTH << endl; }

	if (globals.count("NEWEDGELENGTH") == 1){ NEWEDGELENGTH = atof(globals["NEWEDGELENGTH"].c_str()); }
	else if (globals.count("NEWEDGELENGTH") == 0){ cout << "WARNING: NEWEDGELENGTH is not given in input file, used default value " << NEWEDGELENGTH << endl; }
	else { cout << "ERROR: Global variable NEWEDGELENGTH has multiple definition in input file, used default value NEWEDGELENGTH = " << NEWEDGELENGTH << endl; }

	if (globals.count("GAMMA") == 1){ GAMMA = atof(globals["GAMMA"].c_str()); }
	else if (globals.count("GAMMA") == 0){ cout << "WARNING: GAMMA is not given in input file, used default value " << GAMMA << endl; }
	else { cout << "ERROR: Global variable GAMMA has multiple definition in input file, used default value GAMMA = " << GAMMA << endl; }

	if (globals.count("K") == 1){ K = atof(globals["K"].c_str()); }
	else if (globals.count("K") == 0){ cout << "WARNING: K is not given in input file, used default value " << K << endl; }
	else { cout << "ERROR: Global variable K has multiple definition in input file, used default value K = " << K << endl; }

	if (globals.count("SEED") == 1){ SEED = atof(globals["SEED"].c_str()); }
	else if (globals.count("SEED") == 0){ cout << "WARNING: SEED is not given in input file, used default value " << SEED << endl; }
	else { cout << "ERROR: Global variable SEED has multiple definition in input file, used default value SEED = " << SEED << endl; }

	if (globals.count("INITIALSTEPSIZE") == 1){ INITIALSTEPSIZE = atof(globals["INITIALSTEPSIZE"].c_str()); }
	else if (globals.count("INITIALSTEPSIZE") == 0){ cout << "WARNING: INITIALSTEPSIZE is not given in input file, used default value " << INITIALSTEPSIZE << endl; }
	else { cout << "ERROR: Global variable INITIALSTEPSIZE has multiple definition in input file, used default value INITIALSTEPSIZE = " << INITIALSTEPSIZE << endl; }

	if (globals.count("TIMEINTERVAL") == 1){ TIMEINTERVAL = atof(globals["TIMEINTERVAL"].c_str()); }
	else if (globals.count("TIMEINTERVAL") == 0){ cout << "WARNING: TIMEINTERVAL is not given in input file, used default value " << TIMEINTERVAL << endl; }
	else { cout << "ERROR: Global variable TIMEINTERVAL has multiple definition in input file, used default value TIMEINTERVAL = " << TIMEINTERVAL << endl; }

	if (globals.count("TOTALTIME") == 1){ TOTALTIME = atof(globals["TOTALTIME"].c_str()); }
	else if (globals.count("TOTALTIME") == 0){ cout << "WARNING: TOTALTIME is not given in input file, used default value " << TOTALTIME << endl; }
	else { cout << "ERROR: Global variable TOTALTIME has multiple definition in input file, used default value TOTALTIME = " << TOTALTIME << endl; }

	if (globals.count("CURRENTTIME") == 1){ CURRENTTIME = atof(globals["CURRENTTIME"].c_str()); }
	else if (globals.count("CURRENTTIME") == 0){ cout << "WARNING: CURRENTTIME is not given in input file, used default value " << CURRENTTIME << endl; }
	else { cout << "ERROR: Global variable CURRENTTIME has multiple definition in input file, used default value CURRENTTIME = " << CURRENTTIME << endl; }

	if (globals.count("error_tolerance_absolute_mechanical") == 1){ error_tolerance_absolute_mechanical = atof(globals["error_tolerance_absolute_mechanical"].c_str()); }
	else if (globals.count("error_tolerance_absolute_mechanical") == 0){ cout << "WARNING: error_tolerance_absolute_mechanical is not given in input file, used default value " << error_tolerance_absolute_mechanical << endl; }
	else { cout << "ERROR: Global variable error_tolerance_absolute_mechanical has multiple definition in input file, used default value error_tolerance_absolute_mechanical = " << error_tolerance_absolute_mechanical << endl; }

	if (globals.count("ALPHAM") == 1){ alphaM = atof(globals["ALPHAM"].c_str()); }
	else if (globals.count("ALPHAM") == 0){ cout << "WARNING: ALPHAM is not given in input file, used default value " << alphaM << endl; }
	else { cout << "ERROR: Global variable ALPHAM has multiple definition in input file, used default value ALPHAM = " << alphaM << endl; }

	if (globals.count("TISSUE_ALPHA") == 1){ TISSUE_ALPHA = atof(globals["TISSUE_ALPHA"].c_str()); }
	else if (globals.count("TISSUE_ALPHA") == 0){ cout << "WARNING: TISSUE_ALPHA is not given in input file, used default value " << TISSUE_ALPHA << endl; }
	else { cout << "ERROR: Global variable TISSUE_ALPHA has multiple definition in input file, used default value TISSUE_ALPHA = " << TISSUE_ALPHA << endl; }
	
	if (globals.count("TISSUE_EXT_STRESS") == 1){ TISSUE_EXT_STRESS= atof(globals["TISSUE_EXT_STRESS"].c_str()); }
	else if (globals.count("TISSUE_EXT_STRESS") == 0){ cout << "WARNING: TISSUE_EXT_STRESS is not given in input file, used default value " << TISSUE_EXT_STRESS << endl; }
	else { cout << "ERROR: Global variable TISSUE_EXT_STRESS has multiple definition in input file, used default value TISSUE_EXT_STRESS = " << TISSUE_EXT_STRESS << endl; }

	if (globals.count("TISSUE_EXT_STRESS_X") == 1){ TISSUE_EXT_STRESS_X = atof(globals["TISSUE_EXT_STRESS_X"].c_str()); }
	else if (globals.count("TISSUE_EXT_STRESS_X") == 0){ cout << "WARNING: TISSUE_EXT_STRESS_X is not given in input file, used default value " << TISSUE_EXT_STRESS_X << endl; }
	else { cout << "ERROR: Global variable TISSUE_EXT_STRESS_X has multiple definition in input file, used default value TISSUE_EXT_STRESS_X = " << TISSUE_EXT_STRESS_X << endl; }

	if (globals.count("TISSUE_EXT_STRESS_Y") == 1){ TISSUE_EXT_STRESS_Y = atof(globals["TISSUE_EXT_STRESS_Y"].c_str()); }
	else if (globals.count("TISSUE_EXT_STRESS_Y") == 0){ cout << "WARNING: TISSUE_EXT_STRESS_Y is not given in input file, used default value " << TISSUE_EXT_STRESS_Y << endl; }
	else { cout << "ERROR: Global variable TISSUE_EXT_STRESS_Y has multiple definition in input file, used default value TISSUE_EXT_STRESS_Y = " << TISSUE_EXT_STRESS_Y << endl; }

	//******ADD MORE GLOBAL VARIABLES HERE ALSO ADD THEM TO THE CONSTANTS.H FILE AND GIVE THEM A DEFAULT VALUE IN MAIN*********************//
	if (globals.count("PULLINGDIR_X") == 1) {
		PULLINGDIR_X = atof(globals["PULLINGDIR_X"].c_str());
	}
	else if (globals.count("PULLINGDIR_X") == 0) {
		cout << "WARNING: PULLINGDIR_X is not given in input file, using default value " << PULLINGDIR_X << endl;
	}
	else {
		cout << "ERROR: Global variable PULLINGDIR_X has multiple definitions in input file, using default value " << PULLINGDIR_X << endl;
	}

	if (globals.count("PULLINGDIR_Y") == 1) {
		PULLINGDIR_Y = atof(globals["PULLINGDIR_Y"].c_str());
	}
	else if (globals.count("PULLINGDIR_Y") == 0) {
		cout << "WARNING: PULLINGDIR_Y is not given in input file, using default value " << PULLINGDIR_Y << endl;
	}
	else {
		cout << "ERROR: Global variable PULLINGDIR_Y has multiple definitions in input file, using default value " << PULLINGDIR_Y << endl;
	}
}

// Make all of the objects with the correct starting positions and flags and recordds their ids in the tissue's list of ids.
// Starts at current position of *datafile and reads until keyword 'End'.
void Tissue::HelperReadInputData_CreateObjects(ifstream* datafile){

	string line, key, value, edg;			//temporary variables to store lines and words 
	string type;							//what type of file is it (in case latter versions need to do slightly different things based on type)
	stringstream ssline, ssedges;			//ssline is a string stream so we can comvert strings to stringstreams
	vector<string> keywords;				//vector of strings to hold the keywords
	vector<int> listedges;
	int i;
	map <string, string> objectInfo;		//map to hold the keywords and their data
	int newid, v1, v2, enumb, typenumb;					//temp varables to store the object info
	double x, y;
	bool xf, yf;
	double A0;

	cout << "Making a base class tissue" << endl; 

	//VERTICES
	//read the file untill you get to the word vertices
	while (line.compare("Vertices") != 0){
		getline(*datafile, line);
	}
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')){
		keywords.push_back(key);
	}
	//now recorde the data and make the vertex object untill you get to the word edges signifying the end of the data
	while(true){
		getline(*datafile, line);							//get the next line
		if ((line.compare("Edges") == 0)){ break; }
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')){				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//make the object and as it to the linked list of vertices also record it's id in the master id list
		newid = atoi(objectInfo["id"].c_str());						//extract the data from the map
		x = atof(objectInfo["x"].c_str());
		y = atof(objectInfo["y"].c_str());
		Vertex* vptr = new Vertex;									// Create a vertes with an id and x,y pos and put it in the master list of all veritces
		(*vptr).id = newid;
		(*vptr).r.x = x;
		(*vptr).r.y = y;
		(vertices).push_back(vptr);
		vertexIDs.push_back(newid);								//and add its id to the list of ids
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*vertices.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}

	//EDGES
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')){
		keywords.push_back(key);
	}
	//now recorde the data and make the edge object untill you get to the word cells signifying the end of the data
	while(true){
		getline(*datafile, line);							//get the next line
		if (line.compare("Cells") == 0){ break; }
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')){				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//make the object and as it to the linked list of vertices also record it's id in the master id list
		newid = atoi(objectInfo["id"].c_str());						//extract the data from the map
		v1 = atoi(objectInfo["vertex1"].c_str());
		v2 = atoi(objectInfo["vertex2"].c_str());
		xf = atoi(objectInfo["xflag"].c_str());
		yf = atoi(objectInfo["yflag"].c_str());
		Edge* eptr = new Edge;
		(*eptr).id = newid;
		(*eptr).IOVertexID[0] = v1;
		(*eptr).IOVertexID[1] = v2;
		(*eptr).xflag = xf;
		(*eptr).yflag = yf; 
		(*eptr).cell1 = NULL;
		(*eptr).cell2 = NULL;
		(edges).push_back( eptr );		// Create a vertes with an id and x,y pos and put it in the master list of all veritces
		edgeIDs.push_back( newid );								//and add its id to the list of ids
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*edges.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}

	//CELLS
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')){
		keywords.push_back(key);
	}
	//now recorde the data and make the cell object untill you get to the word end signifying the end of the data
	while (true){
		getline(*datafile, line);							//get the next line
		if (line.compare("End") == 0 || line.compare("Fibers") == 0){ break; }				//break if keyworkd 'End' is found
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')){				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//extract the data from the map
		newid = atoi(objectInfo["id"].c_str());
		enumb = atoi(objectInfo["EdgeNumb"].c_str());
		if (objectInfo.count("Type")){
			typenumb = atoi(objectInfo["Type"].c_str());
		}
		else{ typenumb = 0;}
		if (objectInfo.count("A0") ){
			A0 = atof(objectInfo["A0"].c_str());
		}
		ssedges.clear();
		ssedges << objectInfo["EdgeList"];
		//now worry about the edge list
		listedges.clear();
		while (getline(ssedges, edg, ',')){
			listedges.push_back(atoi(edg.c_str()));
		}
		//make the cell object
		Cell* cptr = new Cell;
		(*cptr).id = newid;
		(*cptr).type = typenumb;
		(*cptr).A0 = A0;
		(*cptr).IOEdgeID = listedges;
		cells.push_back(cptr);
		cellIDs.push_back(newid);
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*cells.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}
}

//! Initializes all of the pointers so that each object has pointers to it's neighbors.
void Tissue::HelperReadInputData_CreatePointers(){
	list<Vertex*>::iterator vit;
	cout << "Making cell <-> edge pointers!" << endl; 
	cout <<"Objects in vertices: " <<  vertices.size() << endl;
	//first make sure pointers are null (if using subclasses this might not have happened corectly) 
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		(*(*eit)).cell1 = NULL; 
		(*(*eit)).cell2 = NULL; 
	}

	//Give the cells pointers to their edges and the edges pointers back to the cells
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){				//for all of the cells
		vector<int> eIDs;																		//make a vector to store the edgeIDs
		eIDs = (*(*cit)).IOEdgeID;															//get the IDs of the initial edges

		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){				//look through all edges
			if (find(eIDs.begin(), eIDs.end(), (*(*eit)).id) != eIDs.end()){				//if the curent edge id is in the list of ids
				((*(*cit)).edges).push_back(&(*(*eit)));											//add edge pointer to cell

				if ((*(*eit)).cell1 == NULL){															//add cell ptr to the edge
					(*(*eit)).cell1 = &(*(*cit));
				}
				else{
					(*(*eit)).cell2 = &(*(*cit));
				}
			}
		}
	}

	cout << "Making vertex <-> edge pointers!" << endl;
	//Give the edges pointers to their vertices and the vertices pointers back to the edges
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){							//for all of the edges
		//cout << "eid c1 c2 " << (*(*eit)).id  << " " << (*(*(*eit)).cell1).id << " " << (*(*(*eit)).cell2).id << endl;
		for (vit = vertices.begin(); vit != vertices.end(); vit++){			//and all of the vertices

			if ((*(*vit)).id == (*(*eit)).IOVertexID[0]){											//if the vertex id matches set up pointers from the edge to the vertex and vice versa
				(*(*eit)).vertex1 = &(*(*vit));
				((*(*vit)).edges).push_back(&(*(*eit)));
			}

			if ((*(*vit)).id == (*(*eit)).IOVertexID[1]){
				(*(*eit)).vertex2 = &(*(*vit));
				((*(*vit)).edges).push_back(&(*(*eit)));
			}
		}
	}
}

// Writes data into file specified by 'datafile'. If flag 'includeExtraInfo' is true, then additional information (such as tension, cell area, etc) is included.
// The extra information is non-essential in reconstructing the simulation state, and is just provided for convenience (e.g. for display in MATLAB).
void Tissue::HelperWriteOutputData_Write2File(ofstream* datafile, bool includeExtraInfo){
	cout << "Issue Here" << endl;
	time_t rawtime;					//to deal with writing the date out
	struct tm * timeinfo;
	cout << "Inside of HelperWriteOutputData" << endl;
	*datafile << "INTEGRATORNAME\t" << INTEGRATORNAME << endl;

	//Now write the global variables you used
	*datafile << "CURRENTTIME\t" << CURRENTTIME << endl;
	*datafile << "Global:" << endl;
	*datafile << "SEED\t" << SEED << endl;
	*datafile << "TIMEINTERVAL\t" << TIMEINTERVAL << endl;
	*datafile << "LX\t" << LX << endl;
	*datafile << "LY\t" << LY << endl;
	*datafile << "MINAREA\t" << MINAREA << endl;
	*datafile << "MINLENGTH\t" << MINLENGTH << endl;
	*datafile << "NEWEDGELENGTH\t" << NEWEDGELENGTH << endl;
	*datafile << "K\t" << K << endl;
	*datafile << "GAMMA\t" << GAMMA << endl;
	*datafile << "TOTALTIME\t" << TOTALTIME << endl;
	*datafile << "INITIALSTEPSIZE\t" << INITIALSTEPSIZE << endl;
	*datafile << "error_tolerance_absolute_mechanical\t" << error_tolerance_absolute_mechanical << endl;
	*datafile << "TISSUE_ALPHA\t" << TISSUE_ALPHA << endl;
	*datafile << "TISSUE_EXT_STRESS_X\t" << TISSUE_EXT_STRESS_X << endl;
	*datafile << "TISSUE_EXT_STRESS_Y\t" << TISSUE_EXT_STRESS_Y << endl;
	//******ADD NEW OUTPUT HERE******//
	*datafile << "endGlobal" << endl;

	if (!includeExtraInfo)											// if flag is false, include only essential information to capture the state of the simulation
	{
		//print the vertex info
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty" << endl;
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){			//loop through all of the vertices in the tissue
			*datafile << (*(*vit)).id << "\t" << (*(*vit)).r.x << "\t" << (*(*vit)).r.y << endl;						//write it's info to the file
		}

		//print the edge info
		*datafile << "Edges" << endl;
		*datafile << "id\tvertex1\tvertex2\txflag\tyflag" << endl;
		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){			//loop through all of the edges in the tissue
			Vertex* v1 = (*(*eit)).vertex1;
			Vertex* v2 = (*(*eit)).vertex2;
			*datafile << (*(*eit)).id << "\t" << (*v1).id << "\t" << (*v2).id << "\t" << (*(*eit)).xflag << "\t" << (*(*eit)).yflag  << endl;			//write its info to the file
		}

		//print the cell info
		int i;
		*datafile << "Cells" << endl;
		*datafile << "id\tA0\tEdgeNumb\tEdgeList\tType" << endl;
		for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){			//loop through all of the cells in the tissue
			stringstream ssEdgeList;
			ssEdgeList.clear();
			for (i = 0; i < (*(*cit)).edges.size(); i++){				//make comma-separated list of edges
				ssEdgeList << (*(*(*cit)).edges[i]).id << ",";
			}

			*datafile << (*(*cit)).id << "\t" << (*(*cit)).A0 << "\t" << (*(*cit)).edges.size() << "\t" << ssEdgeList.str() << "\t" << (*(*cit)).type << endl;
		}
	}

	else															// if flag is true, include only essential information to capture the state of the simulation
	{
		//print the vertex info with additional information
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty\t_EdgesNumber" << endl;
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){			//loop through all of the vertices in the tissue
			*datafile << (*(*vit)).id << "\t" << (*(*vit)).r.x << "\t" << (*(*vit)).r.y << "\t" << (*(*vit)).edges.size() << endl;						//write it's info to the file
		}

		//print the edge info
		*datafile << "Edges" << endl;
		*datafile << "id\tvertex1\tvertex2\txflag\tyflag\ttension\t_cell1\t_cell2" << endl;
		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){			//loop through all of the edges in the tissue
			Vertex* v1 = (*(*eit)).vertex1;
			Vertex* v2 = (*(*eit)).vertex2;
			*datafile << (*(*eit)).id << "\t" << (*v1).id << "\t" << (*v2).id << "\t" << (*(*eit)).xflag << "\t" << (*(*eit)).yflag << "\t" <<(*(*eit)).GetTension_Magnitude() << "\t" << (*(*(*eit)).cell1).id << "\t" << (*(*(*eit)).cell2).id << endl;
		}

		//print the cell info
		int i;
		*datafile << "Cells" << endl;
		*datafile << "id\tA0\tEdgeNumb\tType\t_xflag\t_yflag\t_Area\t_Pressure\t_Center_x\t_Center_y\tEdgeList\t_CWVertices" << endl;
		for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){			//loop through all of the cells in the tissue
			stringstream ssEdgeList;
			ssEdgeList.clear();
			for (i = 0; i < (*(*cit)).edges.size(); i++){				//make comma-separated list of edges
				ssEdgeList << (*(*(*cit)).edges[i]).id << ",";
			}

			vector<Vertex*> CWVertices = (*(*cit)).GetClockWiseVertices();
			stringstream ssCWVertices;
			ssCWVertices.clear();
			for (i = 0; i < CWVertices.size(); i++){			//make comma-separated list of vertices ordered clockwise
				ssCWVertices << (*CWVertices[i]).id << ",";
			}
			Vect centroid = (*(*cit)).GetCenter();
			*datafile << (*(*cit)).id << "\t" << (*(*cit)).A0 << "\t" << (*(*cit)).edges.size() << "\t" << (*(*cit)).type << "\t" << (*(*cit)).GetXFlag() << "\t" << (*(*cit)).GetYFlag() << "\t"
				<< (*(*cit)).GetArea() << "\t" << (*(*cit)).GetPressure((*(*cit)).GetArea()) << "\t" << centroid.x << "\t" << centroid.y << "\t" << ssEdgeList.str() << "\t" << ssCWVertices.str() << endl;
		}
	}


	*datafile << "End" << endl << endl;
}


// This is the helper function that actually do mitosis on cells.
void Tissue::HelperMitosis(Cell * parentcell, double divisionangle) {
	int i;										//for loops
	Vect center, p1, p2, q1, q2;				//Used to find newcenteredge
	Vect rnewvtx;								//the positions of the new vertices
	vector<Edge*> empty;
	vector<Edge*> deletededges;					//vector of edge pointers to the two edges to remove
	vector<Edge*> newedges;						//vector to hold the four new edges to replace the two that are being removed
	vector<Vertex*> newvertices;				//vector of pointers to the two new vertices
	vector<Vertex*> cwverts;					//parent cell verts in clockwise order
	Edge* newedgecenter = 0;					//ptr to the edge through the center of the cell
	Vertex *v1ptr, *v2ptr, *v3ptr, *v4ptr;
	double angle;								//angle at which to cut through the cell
	vector<vector<double>> vertposforflag;		//used to define vertex positions if any flags are set
	bool newxflag = false;						//flags for which side of boundary newvtxr is on
	bool newyflag = false;						//flags for which side of boundary newvtxr is on					
	vector<Vertex*> vertsordered;				//list of pointers to vertices used for the case of flags
	double xpossum = 0;							//sum of x positions of vertices in cell
	double ypossum = 0;							//sum of y positions of vertices in cell
	double centerposx = 0;						//used for center calculation done within such that the center is consistent with the vertposforflag
	double centerposy = 0;						//used for center calculation done within such that the center is consistent with the vertposforflag
	vector<int> edgeflagstestind, edgeflagstestx, edgeflagstesty;	//this is used to test for flags on four newly created edges (not one created in center); stores i and two flags
	int vertnumb;								//lists number of vertices
	vector<int> newflagstestx, newflagstesty;	//this is used to flag the newedgecenter
	Cell* c1ptr = NULL;
	Cell* c2ptr = NULL;
	Cell* daughter1ptr = NULL;
	Cell* daughter2ptr = NULL; 
	
	vertsordered = (*parentcell).HelperOrderVertices();  //stores vertex information to be called after vertposforflag is checked
	vertposforflag = (*parentcell).HelperOrderVerticesPos(); //allows positions of vertices outside of domain
	vertnumb = (*parentcell).GetVertexNumber(); //stores number of vertices in cell

	//This is the geometrical center of the polygonal cell (This center can be outside of the boundaries depending on HelperVerticesPos() values).
	center = GetGeoCenter(vertposforflag[0], vertposforflag[1]);


	//get the angle at which to put the new edge
	angle = divisionangle;
	q1 = Vect(center, angle, LX);      //make two points that define a line at the given angle through the center of the cell
	q2 = Vect(center, angle, -LX);

	//create two new vertices where this angle intersects the edges of the cell with if statement that tests for flag and directs accordingly
	if ((*parentcell).GetFlag() == 0) {
		for (i = 0; i < (*parentcell).edges.size(); i++){								//look at each edge
			p1 = (*(*(*parentcell).edges[i]).vertex1).r;								//get it's end points and put them into p1 and p2
			p2 = (*(*(*parentcell).edges[i]).vertex2).r;
			if (test_for_Intersect(q1, q2, p1, p2, &rnewvtx)){							//if it crosses the selected angle
				deletededges.push_back((*parentcell).edges[i]);								//record the edge
				newvertices.push_back((*this).CreateVertex(rnewvtx, empty));				//make a new vertex at the interscetion and record its ptr
			}
		}

		//create a new edge between the two new vertices (must have no xflag and no yflag)
		newedgecenter = (*this).CreateEdge(newedgecenter, 0, 0, newvertices[0], newvertices[1], parentcell, parentcell);
	}

	//This is for the case where a cell crosses any boundary (WE LOOP THROUGH AND THEN PAIR THE END WITH THE BEGINNING)
	if ((*parentcell).GetFlag() == 1) {
		for (i = 0; i < vertnumb - 1; i++){ //look at each vertex
			p1.x = (vertposforflag[0])[i]; //get it's end points and put them into p1 and p2 (THESE POSITIONS CAN BE OUT OF BOUNDS)
			p1.y = (vertposforflag[1])[i];
			p2.x = (vertposforflag[0])[i + 1];
			p2.y = (vertposforflag[1])[i + 1];
			if (test_for_Intersect(q1, q2, p1, p2, &rnewvtx)){				//if it crosses the selected angle
				deletededges.push_back((*vertsordered[i]).GetEdgeBetween(vertsordered[i + 1]));								//record the edge			
				if (rnewvtx.x>LX || rnewvtx.x<0){ newxflag = 1; } //These six lines used later to create flags for newedgecenter (stores information about which side of boundary for each newvertex)
				else { newxflag = 0; }
				if (rnewvtx.y>LY || rnewvtx.y<0){ newyflag = 1; }
				else { newyflag = 0; }
				newflagstestx.push_back(newxflag);
				newflagstesty.push_back(newyflag);
				edgeflagstestind.push_back(i); //This whole sections helps us later when trying to set flags for other four newly created edges (TESTS WHICH SIDE OF THE BOUNDARY ARE THE FOUR VERTICES OF DELETED EDGES)
				if (p1.x>LX || p1.x < 0) { edgeflagstestx.push_back(1); }
				else { edgeflagstestx.push_back(0); }
				if (p1.y>LY || p1.y<0)  { edgeflagstesty.push_back(1); }
				else { edgeflagstesty.push_back(0); }
				edgeflagstestind.push_back(i + 1);
				if (p2.x>LX || p2.x < 0) { edgeflagstestx.push_back(1); }
				else { edgeflagstestx.push_back(0); }
				if (p2.y>LY || p2.y < 0)  { edgeflagstesty.push_back(1); }
				else { edgeflagstesty.push_back(0); } //Conclusion of section used for setting flags for the other four newly created edges
				if (rnewvtx.x>LX){ rnewvtx.x = rnewvtx.x - LX; } //This part puts the newvtx within the boundaries appropriately
				if (rnewvtx.x<0){ rnewvtx.x = rnewvtx.x + LX; }
				if (rnewvtx.y>LY){ rnewvtx.y = rnewvtx.y - LY; }
				if (rnewvtx.y<0){ rnewvtx.y = rnewvtx.y + LY; }
				newvertices.push_back((*this).CreateVertex(rnewvtx, empty));
			}
		}
		p1.x = (vertposforflag[0])[vertnumb - 1]; //get it's end points and put them into p1 and p2 FOR THE CASE WHERE THE BOTTOM ELEMENT IS PAIRED WITH THE TOP ELEMENT
		p1.y = (vertposforflag[1])[vertnumb - 1];
		p2.x = (vertposforflag[0])[0];
		p2.y = (vertposforflag[1])[0];
		if (test_for_Intersect(q1, q2, p1, p2, &rnewvtx)){				//if it crosses the selected angle
			deletededges.push_back((*(vertsordered[vertnumb - 1])).GetEdgeBetween(vertsordered[0]));								//record the edge
			if (rnewvtx.x>LX || rnewvtx.x<0){ newxflag = 1; } //These six lines used to store information about flags for newedgecenter (stores information about which side of boundary for each newvertex)
			else { newxflag = 0; }
			if (rnewvtx.y>LY || rnewvtx.y<0){ newyflag = 1; }
			else { newyflag = 0; }
			newflagstestx.push_back(newxflag);
			newflagstesty.push_back(newyflag);
			edgeflagstestind.push_back(vertnumb - 1);
			if (p1.x > LX || p1.x < 0) { edgeflagstestx.push_back(1); } //This whole sections helps us later when trying to set flags for four other newly created edges (TESTS WHICH SIDE OF THE BOUNDARY ARE THE FOUR VERTICES OF DELETED EDGES)
			else { edgeflagstestx.push_back(0); }
			if (p1.y>LY || p1.y<0)  { edgeflagstesty.push_back(1); }
			else { edgeflagstesty.push_back(0); }
			edgeflagstestind.push_back(0);
			if (p2.x>LX || p2.x < 0) { edgeflagstestx.push_back(1); }
			else { edgeflagstestx.push_back(0); }
			if (p2.y>LY || p2.y < 0)  { edgeflagstesty.push_back(1); }
			else { edgeflagstesty.push_back(0); } //Conclusion of section used for setting flags for four other newly created edges
			if (rnewvtx.x>LX){ rnewvtx.x = rnewvtx.x - LX; }//This part puts the newvtx within the boundaries appropriately
			if (rnewvtx.x<0){ rnewvtx.x = rnewvtx.x + LX; }
			if (rnewvtx.y>LY){ rnewvtx.y = rnewvtx.y - LY; }
			if (rnewvtx.y<0){ rnewvtx.y = rnewvtx.y + LY; }
			newvertices.push_back((*this).CreateVertex(rnewvtx, empty));
		}

		//create a new edge between the two new vertices with appropriate flags
		//test whether new vertices are on opposite side of a boundary
		if (newflagstestx[0] != newflagstestx[1] && newflagstesty[0] != newflagstesty[1])
		{
			newedgecenter = (*this).CreateEdge(newedgecenter,1, 1, newvertices[0], newvertices[1], parentcell, parentcell);
		}
		else if (newflagstestx[0] == newflagstestx[1] && newflagstesty[0] != newflagstesty[1])
		{
			newedgecenter = (*this).CreateEdge(newedgecenter,0, 1, newvertices[0], newvertices[1], parentcell, parentcell);
		}
		else if (newflagstestx[0] != newflagstestx[1] && newflagstesty[0] == newflagstesty[1])
		{
			newedgecenter = (*this).CreateEdge(newedgecenter,1, 0, newvertices[0], newvertices[1], parentcell, parentcell);
		}
		else if (newflagstestx[0] == newflagstestx[1] && newflagstesty[0] == newflagstesty[1])
		{
			newedgecenter = (*this).CreateEdge(newedgecenter,0, 0, newvertices[0], newvertices[1], parentcell, parentcell);
		}
	}

	//Assign vertex ids to newedgecenter for output purposes
	(*newedgecenter).IOVertexID[0] = (*(newvertices[0])).id;
	(*newedgecenter).IOVertexID[1] = (*(newvertices[1])).id;

	//This is just used to warn you if for some reason more than two edges of the cell are intersected
	if (deletededges.size() != 2) {
		cout << "ERROR: The Mitosis function had number of intersected edges different than two (Problem with Vertices and GetCenter or with overlap)" << endl;
	}
	(*newvertices[0]).edges.push_back(newedgecenter);
	(*newvertices[1]).edges.push_back(newedgecenter);

	//create the four new edges to replace the ones to be removed and add them to the vertices
	c1ptr = (*deletededges[0]).GetOtherCell(parentcell);
	c2ptr = (*deletededges[1]).GetOtherCell(parentcell);
	v1ptr = (*deletededges[0]).vertex1;
	v2ptr = (*deletededges[0]).vertex2;
	v3ptr = (*deletededges[1]).vertex1;
	v4ptr = (*deletededges[1]).vertex2;

	//Making sure to give all new edges the approriate IOVertexID's and that the vertices know their edges
	newedges.push_back((*this).CreateEdge(newedgecenter, 0, 0, v1ptr, newvertices[0], parentcell, c1ptr));
	(*v1ptr).edges.push_back(newedges[0]);
	(*newvertices[0]).edges.push_back(newedges[0]);
	(*c1ptr).edges.push_back(newedges[0]);
	(*parentcell).edges.push_back(newedges[0]);
	(*(newedges[0])).IOVertexID[0] = (*v1ptr).id;
	(*(newedges[0])).IOVertexID[1] = (*newvertices[0]).id;

	newedges.push_back((*this).CreateEdge(newedgecenter,0, 0, v2ptr, newvertices[0], parentcell, c1ptr));
	(*v2ptr).edges.push_back(newedges[1]);
	(*newvertices[0]).edges.push_back(newedges[1]);
	(*c1ptr).edges.push_back(newedges[1]);
	(*parentcell).edges.push_back(newedges[1]);
	(*(newedges[1])).IOVertexID[0] = (*v2ptr).id;
	(*(newedges[1])).IOVertexID[1] = (*newvertices[0]).id;

	newedges.push_back((*this).CreateEdge(newedgecenter,0, 0, v3ptr, newvertices[1], parentcell, c2ptr));
	(*v3ptr).edges.push_back(newedges[2]);
	(*newvertices[1]).edges.push_back(newedges[2]);
	(*c2ptr).edges.push_back(newedges[2]);
	(*parentcell).edges.push_back(newedges[2]);
	(*(newedges[2])).IOVertexID[0] = (*v3ptr).id;
	(*(newedges[2])).IOVertexID[1] = (*newvertices[1]).id;

	newedges.push_back((*this).CreateEdge(newedgecenter,0, 0, v4ptr, newvertices[1], parentcell, c2ptr));
	(*v4ptr).edges.push_back(newedges[3]);
	(*newvertices[1]).edges.push_back(newedges[3]);
	(*c2ptr).edges.push_back(newedges[3]);
	(*parentcell).edges.push_back(newedges[3]);
	(*(newedges[3])).IOVertexID[0] = (*v4ptr).id;
	(*(newedges[3])).IOVertexID[1] = (*newvertices[1]).id;


	//This sequence of if statements checks if the edge to be deleted crosses a boundary and sees (based on if the newedgecenter crosses the boundary)
	//which of the two newly created edges on deleted edges needs a flag. This relies heavily on a testflag I set above to see if HelperVerticesPos() took a vertex out of the domain.
	if (edgeflagstestind.size() == 4){ //This just makes sure that this only runs when the indices of the four vertices of deleted edges are well known
		if ((*newedgecenter).xflag == 1){ //if the new edge crosses the x bound
			if (edgeflagstestx[0] != edgeflagstestx[1]) { //if the deleted edge vertices are on opposite sides of the boundary
				for (i = 0; i < 2; i++) { //for first deleted edge
					if (edgeflagstestx[i] != newflagstestx[0]) { //which one is opposite the new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[0])).xflag = 1;
					}
				}
			}
			if (edgeflagstestx[2] != edgeflagstestx[3]) { //if the deleted edge vertices are on opposite sides of the boundary
				for (i = 2; i < 4; i++) { //for second deleted edge
					if (edgeflagstestx[i] != newflagstestx[1]) { //which one is opposite the new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[1])).xflag = 1;
					}
				}
			}
		}
		if ((*newedgecenter).xflag == 0 && (*parentcell).GetXFlag() == 1) { //if newedgecenter doesn't cross x bound, but cell does
			if (edgeflagstestx[0] != edgeflagstestx[1]) { //if the deleted edge vertices are on opposite sides of the boundary
				for (i = 0; i < 2; i++) { //for first deleted edge
					if (edgeflagstestx[i] != newflagstestx[0]) { //which one is opposite the new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[0])).xflag = 1;
					}
				}
			}
			if (edgeflagstestx[2] != edgeflagstestx[3]) {
				for (i = 2; i < 4; i++) { //for second edge
					if (edgeflagstestx[i] != newflagstestx[1]) {
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[1])).xflag = 1;
					}
				}
			}
		}
		if ((*newedgecenter).yflag == 1){ //if newedgecenter crosses the y boundary
			if (edgeflagstesty[0] != edgeflagstesty[1]) { //if vertices are on opposite side of boundary
				for (i = 0; i < 2; i++) { //for first deleted edge
					if (edgeflagstesty[i] != newflagstesty[0]) { //which one is opposite the new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[0])).yflag = 1;
					}
				}
			}
			if (edgeflagstesty[2] != edgeflagstesty[3]) {
				for (i = 2; i < 4; i++) { //for second deleted edge
					if (edgeflagstesty[i] != newflagstesty[1]) { //which one is opposite the vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[1])).yflag = 1;
					}
				}
			}
		}
		if ((*newedgecenter).yflag == 0 && (*parentcell).GetYFlag() == 1) { //if newedgecenter doesn't cross y bound, but cell does
			if (edgeflagstesty[0] != edgeflagstesty[1]) { //if vertices on opposite side of boundary
				for (i = 0; i < 2; i++) { //for first deleted edge
					if (edgeflagstesty[i] != newflagstesty[0]) { //which one is opposite new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[0])).yflag = 1;
					}
				}
			}
			if (edgeflagstesty[2] != edgeflagstesty[3]) { //if vertices on opposite side of the boundary
				for (i = 2; i < 4; i++) { //for second deleted edge
					if (edgeflagstesty[i] != newflagstesty[1]) { //which one is opposite the new vertex
						(*(*vertsordered[edgeflagstestind[i]]).GetEdgeBetween(newvertices[1])).yflag = 1;
					}
				}
			}
		}
	}
	//end of sequence of if statements. All flags of new edges should be correct.


	//Create the two new daughter cells 
	daughter1ptr = (*this).CreateCell(0, empty);
	daughter2ptr = (*this).CreateCell(0, empty);

	//Delete the two old edges 
	(*parentcell).RemoveEdge(deletededges[0]);
	(*parentcell).RemoveEdge(deletededges[1]);
	(*c1ptr).RemoveEdge(deletededges[0]);
	(*c2ptr).RemoveEdge(deletededges[1]);
	(*v1ptr).RemoveEdge(deletededges[0]);
	(*v2ptr).RemoveEdge(deletededges[0]);
	(*v3ptr).RemoveEdge(deletededges[1]);
	(*v4ptr).RemoveEdge(deletededges[1]);
	(*this).DestroyEdge(deletededges[0]);
	(*this).DestroyEdge(deletededges[1]);

	//fill the new daughter cells edge lists and update the edges to include the new daughter cell
	(*daughter1ptr).edges.push_back(newedgecenter);
	(*daughter2ptr).edges.push_back(newedgecenter);
	(*newedgecenter).UpdateCell(parentcell, daughter1ptr);
	(*newedgecenter).UpdateCell(parentcell, daughter2ptr);

	Edge e, e0, *eptr;
	Vertex v, v0;
	e0 = *newedgecenter;
	v0 = *(newvertices[0]);
	do{							//for the first daughter				
		for (i = 0; i < (*parentcell).edges.size(); i++){																					// look through all of the edges in the cell
			if (((*(*(*parentcell).edges[i]).vertex1) == v0 || (*(*(*parentcell).edges[i]).vertex2) == v0) && (*(*parentcell).edges[i]) != e0){		//find the edge that has v0 as a vertex and is not the edge e0 (the one you just looked at)
				e = (*(*parentcell).edges[i]);
				eptr = (*parentcell).edges[i];
			}
		}
		(*daughter1ptr).edges.push_back(eptr);					//put the edge in the first daughter cell
		(*eptr).UpdateCell(parentcell, daughter1ptr); //and update the edge's cells
		(*daughter1ptr).IOEdgeID.push_back((*eptr).id); //build IOEdgeID for output
		v = *(v0.GetOtherVertex(eptr));							//use that edge to get the next vertex
		e0 = e;													//update e0,e,v0,v
		v0 = v;
	} while (v0 != (*newvertices[1]));
	do{
		for (i = 0; i < (*parentcell).edges.size(); i++){																					// look through all of the edges in the cell
			if (((*(*(*parentcell).edges[i]).vertex1) == v0 || (*(*(*parentcell).edges[i]).vertex2) == v0) && (*(*parentcell).edges[i]) != e0){		//find the edge that has v0 as a vertex and is not the edge e0 (the one you just looked at)
				e = (*(*parentcell).edges[i]);
				eptr = (*parentcell).edges[i];
			}
		}						//for the second daughter
		(*daughter2ptr).edges.push_back(eptr);						//put the edge in the second daughter cell
		(*eptr).UpdateCell(parentcell, daughter2ptr); //and update the edge's cells
		(*daughter2ptr).IOEdgeID.push_back((*eptr).id); //build IOEdgeID for output
		v = *(v0.GetOtherVertex(eptr));								//use that edge to get the next vertex
		e0 = e;														//update e0,e,v0,v
		v0 = v;
	} while (v0 != (*newvertices[0]));

	//Set A0 for cells (otherwise integrator won't know what to do)
	(*daughter1ptr).A0 = (*parentcell).A0/2;
	(*daughter2ptr).A0 = (*parentcell).A0/2;

	//Add newedgecenter to IOEDGEID for both cells
	(*daughter1ptr).IOEdgeID.push_back((*newedgecenter).id);
	(*daughter2ptr).IOEdgeID.push_back((*newedgecenter).id);

	//Define cell types (will become important when discussing compartment boundaries)
	(*daughter1ptr).type = (*parentcell).type+1;
	(*daughter2ptr).type = (*parentcell).type+1;

	//delete the parent cell
	(*this).DestroyCell(parentcell);
}

// Resolves a forfold vertex into 2 threefold vertices if it is energeticaly favorable to do so, returns TRUE if the resolve is successful.
/*!
\par Pseudocode
\code{.cpp}
NOTE: there are two ways to resolve. Way A keeps edge0 and edge1 connected to the same vertex. Way B keeps edge1 and edge2 connected to the same vertex

startfcn:
if the vertex is not fourfold
return an error
endif

call GetOrderedEdges()
(This returns a list of the edges ordered CW or CCW)
make a list Acells such that Acells[1] is between edge0 and edge1 with the cells ordered the same direction as the edges
This requires checking 4 cases:
edge0.cell1=edge1.cell1  	then Acells=[edge0.cell2, edge0.cell1, edge1.cell2]
edge0.cell1=edge1.cell2 	then Acells=[edge0.cell2, edge0.cell1, edge1.cell1]
edge0.cell2=edge1.cell1  	then Acells=[edge0.cell1, edge0.cell2, edge1.cell2]
edge0.cell2=edge1.cell2 	then Acells=[edge0.cell1, edge0.cell2, edge1.cell1]

make a list Bcells such that Bcells[1] is between edge1 and edge2 with the cells ordered the same direction as the edges

ForceA=edge0.GetTension+edge1.GetTension+Acells0.GetPressure+Acells1.GetPressure+Acells2.GetPressure (vecs)
ForceB=edge1.GetTension+edge2.GetTension+Bcells0.GetPressure+Bcells1.GetPressure+Bcells2.GetPressure (vecs)
ForceNewEdge=GetTension

if Magnitude(ForceNewEdge) > Magnitude(ForceA) and Magnitude(ForceB)
return FALSE
else if Magnitude(ForceA) > Magnitude(ForceB)
get the position of the two new vertices by putting them at newedgelength/2 apart along the line of forceA
Vertex1 is along ForceA and vertex2 is along negative ForceA

for each edge, do:
if it bounds Acell[1]
add it to the list of edges for newvertex1
else
add it to the list of edges for newvertex2
endif
endfor

create two new vertices at the two new positions with the appropriate two edges
create the new edge with vertices: newv1 and newv2 and cells Acells[0] and Acells[2]
add the new edge to newv1 and newv2 edge list
add the new edge to Acell[0] and Acell[2] edge lists
add the two new vertices to the edges and remove the old fourfold vertex
remove the fourfold vertex from the tissue
return TRUE

else if Mag(ForceB)>Mag(ForceA)
do the same process as before but with Bcells instead of Acell and Bforce instead of Aforce
endif

endfcn
\endcode
*/
bool Tissue::HelperResolveVertices_ResolveFourfoldVertex(Vertex* v){
	vector<Edge*> edges;
	vector<Cell*> cells;
	double stable12, stable14; //the stability when pairing edges 12 and edges 14 negitive values denote stability against breaking in that direction
	Vect Force12, Force14;	//the force from splitting in each topology used to set the angle on new edge growth

	//first make sure the vertex is fourfold
	if (!((*v).CheckFourfold()))
		cout << "ERROR trying to resolve nonfourfold vertex" << endl;

	//Get the edges and cells in CW order
	edges = (*v).GetCWEdges();
	cells = (*v).GetCWCells();

	//find the stability of each configuration
	stable12 = (*this).HelperResolveVertices_CheckStability(v, edges[0], edges[1], edges[2], edges[3], cells[0], cells[1], cells[2], cells[3], &Force12);
	stable14 = (*this).HelperResolveVertices_CheckStability(v, edges[3], edges[0], edges[1], edges[2], cells[3], cells[0], cells[1], cells[2], &Force14);

	//Three cases based on the stability
	if (stable12 < 0 && stable14 < 0){
		return false;		//the vertex is stable youre done!
	}
	else if (stable12 > stable14){
		(*this).HelperResolveVertices_UpdatePtrs(v, edges[0], edges[1], edges[2], edges[3], cells[0], cells[1], cells[2], cells[3], &Force12);
		return true;
	}
	else{
		(*this).HelperResolveVertices_UpdatePtrs(v, edges[3], edges[0], edges[1], edges[2], cells[3], cells[0], cells[1], cells[2], &Force14);
		return true;
	}
}

//Called by HelperResolveVertices_ResolveFourfoldVertex to check the stability of one configuration. Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors retruns false if the vertex is unstable in that topology
double Tissue::HelperResolveVertices_CheckStability(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force){
	//First make new objects which are coppies of the objects in the linked lists we will minipulate these objecs and they will be destroyed when the function ends
	Cell c1 = (*cell1);
	Cell c2 = (*cell2);
	Cell c3 = (*cell3);
	Cell c4 = (*cell4);
	Edge e1 = (*edge1);
	Edge e2 = (*edge2);
	Edge e3 = (*edge3);
	Edge e4 = (*edge4);
	double stability;
	double a1, a2, a3, a4;

	//Make the new edge and vertices resulting from the split into 2 3fold vertices
	Edge enew;
	Vertex v12; //the vertex on edges 1 and 2
	Vertex v34; //the vertex on edges 3 and 4

	//Now update the info on the coppies of the objects so they are 2 3fold vertices

	//For the new vertices
	v12.r = (*vert).r;							//put the two new verts where the olf 4ourfold was
	v12.edges.push_back(&e1);
	v12.edges.push_back(&e2);
	v12.edges.push_back(&enew);
	v34.r = (*vert).r;							//put the two new verts where the olf 4ourfold was
	v34.edges.push_back(&e3);
	v34.edges.push_back(&e4);
	v34.edges.push_back(&enew);

	//For the new edge
	enew.yflag = 0;		//the new edge has no length and therefore cannot have a flag
	enew.xflag = 0;
	enew.vertex1 = &v12;
	enew.vertex2 = &v34;
	enew.cell1 = &c4;
	enew.cell2 = &c2;

	//For the old edges
	e1.UpdateVertex(vert, &v12);
	e2.UpdateVertex(vert, &v12);
	e3.UpdateVertex(vert, &v34);
	e4.UpdateVertex(vert, &v34);

	//Now we have at the position of the fourfold vertex two threefold vertices in which everyone knows all of their neighbors correclty, we can now calculate the forces based on what the tensiions and pressures are in this configuration

	//pressure differences across cells
	a1 = c1.GetArea();
	a2 = c2.GetArea();
	a3 = c3.GetArea();
	a4 = c4.GetArea();
	double p2 = (c2.GetPressure(a2) - c1.GetPressure(a1)); //pressure difference acorss edge2 between cell 1 and cell 2 taken CCW
	double p3 = (c3.GetPressure(a3) - c2.GetPressure(a2));
	double p4 = (c4.GetPressure(a4) - c3.GetPressure(a3));
	double p1 = (c1.GetPressure(a1) - c4.GetPressure(a4));

	//getting z X edge (both the diredction and magnitude are correctHelperResolveVertices_ResolveFourfoldVertex
	Vect zXe1 = (e1).GetVectorDifference(&v12, v12.GetOtherVertex(&e1)).Rotate(PI / 2.0);
	Vect zXe2 = (e2).GetVectorDifference(&v12, v12.GetOtherVertex(&e2)).Rotate(PI / 2.0);
	Vect zXe3 = (e3).GetVectorDifference(&v34, v34.GetOtherVertex(&e3)).Rotate(PI / 2.0);
	Vect zXe4 = (e4).GetVectorDifference(&v34, v34.GetOtherVertex(&e4)).Rotate(PI / 2.0);

	//Get the forces
	Vect ForceT1 = (e1).GetTension_ForceOnVertex(&v12);
	Vect ForceT2 = (e2).GetTension_ForceOnVertex(&v12);
	Vect ForceT3 = (e3).GetTension_ForceOnVertex(&v34);
	Vect ForceT4 = (e4).GetTension_ForceOnVertex(&v34);
	Vect ForceP1 = zXe1 * (p1 * 0.5);
	Vect ForceP2 = zXe2 * (p2 * 0.5);
	Vect ForceP3 = zXe3 * (p3 * 0.5);
	Vect ForceP4 = zXe4 * (p4 * 0.5);
	*Force = ((ForceT1 + ForceT2 + ForceP1 + ForceP2) - (ForceT3 + ForceT4 + ForceP3 + ForceP4))* 0.5;

	//Calculate the tension on the new edge
	Vect ForceNewEdge = enew.GetTension_ForceOnVertex(&v12);

	//Check the stability
	stability = (*Force).Magnitude() - ForceNewEdge.Magnitude();
 	return stability;
}

//Called by HelperResolveVertices_ResolveFourfoldVertex to update the pointers when the vertex needs to resolve, Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors 
Edge* Tissue::HelperResolveVertices_UpdatePtrs(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force){
	Vect oldvr = (*vert).r;
	Vect newv1r = Vect((*vert).r, (*Force).Angle(), NEWEDGELENGTH / 2.0);		 //find the possition of the new vertices
	Vect newv2r = Vect((*vert).r, (*Force).Angle() + PI, NEWEDGELENGTH / 2.0);

	//get the edge lists
	vector<Edge*> v12edges;
	v12edges.push_back(edge1);
	v12edges.push_back(edge2);
	vector<Edge*> v34edges;
	v34edges.push_back(edge3);
	v34edges.push_back(edge4);

	Vertex* v12ptr = (*this).CreateVertex(oldvr, v12edges);						//create two new vertices at the same position as the old vertex
	Vertex* v34ptr = (*this).CreateVertex(oldvr, v34edges);
	Edge* eptr = (*this).CreateEdge(edge1,0, 0, v12ptr, v34ptr, cell2, cell4);			//create new edge with vertex and cell ptrs (note is has no flags b/c it can't cross a boundary with length 0)

	(*v12ptr).edges.push_back(eptr);								//add the new edge to the new vertices
	(*v34ptr).edges.push_back(eptr);
	(*cell2).edges.push_back(eptr);									//add the new edge to the cells
	(*cell4).edges.push_back(eptr);
	(*edge1).UpdateVertex(vert, v12ptr);								//uptate the vertices on the edges
	(*edge2).UpdateVertex(vert, v12ptr);
	(*edge3).UpdateVertex(vert, v34ptr);
	(*edge4).UpdateVertex(vert, v34ptr);

	(*this).DestroyVertex(vert);	//destory the fourfold vertex

	//now move the vertices apart along the line of force and set the flags
	(*v12ptr).rnew = newv1r;
	(*v34ptr).rnew = newv2r;
	(*this).HelperResolve_UpdatePosition(v12ptr);
	(*this).HelperResolve_UpdatePosition(v34ptr);

	(*cell1).SavedInfoForEvolve_CWVerts = (*cell1).GetClockWiseVertices();
	(*cell2).SavedInfoForEvolve_CWVerts = (*cell2).GetClockWiseVertices(); 
	(*cell3).SavedInfoForEvolve_CWVerts = (*cell3).GetClockWiseVertices();
	(*cell4).SavedInfoForEvolve_CWVerts = (*cell4).GetClockWiseVertices();
	return eptr; 
}


// Moves the vertices to their new positions and updates the approproate flags if they cross a boundary.
bool Tissue::HelperEvolve_UpdatePosition(){
	double newx, newy;
	bool VertexHasCrossed = 0;
	bool flag=0; 

	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		flag = (*(*vit)).UpdatePosition(); 
		VertexHasCrossed = VertexHasCrossed || flag; 
	}
	return VertexHasCrossed;
}

//! Build array of vertex positions (x and y) as well as LX and LY based on current values.
void Tissue::Store_Initial_Condition(vector<Vect> &y){
	//Loop through the current positions and store them in an array.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		y.at(i) = (*(*vit)).rold;
		i++;
	}
	//Make the last elements the box size dimensions.
	//Check that this vect is doing what you want.
	y.at(i) = Vect(LXold, LYold);						//set last to elements of y[] to current box size
}

//! Update rold with specified value.
void Tissue::Update_Old_Mechanical_Variables(vector<Vect> &y){
	//Store the values from y into the old mechanical variables.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).rold = y.at(i);
		i++;
	}
	LXold = y.at(i).x;
	LYold = y.at(i).y;

	//Force the .r and LX,LY to be the y values.
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		(*(*vit)).r = (*(*vit)).rold;
	}
	LX = LXold;
	LY = LYold;
}

//! Store the updated positions (from any of the mechanical position evolvers).
void Tissue::Store_New_Positions(vector<Vect> &y){
	//Loop through the current positions and store them in an array.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		y.at(i) = (*(*vit)).rnew;
		i++;
	}
	//Make the last elements the box size dimensions.
	//Check that this vect is doing what you want.
	y.at(i) = Vect(LXnew, LYnew);						//set last to elements of y[] to current box size
}

//! Step size control mechanical evolution (using double step error estimation)
bool Tissue::Double_Step_Size_Control(const vector<Vect> &y_error, const double error_tolerance_absolute, double* initial_step_size, double consistency_order){
	//dimension of mechanical systems (vertices plus the boxsize)
	int dimension_mechanical_vect = vertices.size() + 1;
	int dimension_mechanical_double = 2 * dimension_mechanical_vect;
	//Initialize the flag that is output to show if satisying error tolerance.
	bool step_error_check = false;
	//Initialize the factor for scaling the step size.
	double scale_factor_double_step = 1.0;
	//Initialize a variable for storing the adjusted step size.
	double adjusted_step_size = *initial_step_size;
	//The order of the numerical solver (consistency_order)
	double safety_factor = .9;
	//perhaps tune this up.
	double upper_fractional_tolerance = 1.1;
	//possibly change this and the parameters
	double lower_fractional_tolerance = 0.5;
	//Check whether error exceeds tolerance.
	vector<double> excess_error;
	excess_error.resize(dimension_mechanical_double);
	//Check whether error is excessively below tolerance.
	vector<double> low_error;
	low_error.resize(dimension_mechanical_double);
	//Ratio of error to tolerance
	vector<double> error_ratio;
	error_ratio.resize(dimension_mechanical_double);
	//Store the max ratio of the error to the tolerance.
	double max_error_ratio = 0.0;
	//Check for excess error.
	double max_error_check = 0.0;

	//For stability, enforce bounds on the scale factor.
	double lower_bound_scaling_factor = 1. / 5.;
	double upper_bound_scaling_factor = 5.;

	//for debugging print the meadian and max error 
	/*double meanerr = 0; 
	double maxerr =0; 
	for (int i = 0; i != dimension_mechanical_vect; i++){
		meanerr += y_error[i].x;
		meanerr += y_error[i].y;
		if (y_error[i].x > maxerr) maxerr = y_error[i].x; 
		if (y_error[i].y > maxerr) maxerr = y_error[i].y;
	}
	double meanmov = 0;
	double maxmov = 0;
	double mov; 
	for (list<Vertex*>::iterator v = vertices.begin(); v != vertices.end(); v++){
		mov= sqrt( ((*(*v)).rold.x - (*(*v)).rnew.x)* ((*(*v)).rold.x - (*(*v)).rnew.x) + ((*(*v)).rold.y- (*(*v)).rnew.y)*((*(*v)).rold.y - (*(*v)).rnew.y));
		meanmov += mov;
		if (mov > maxmov) maxmov = mov; 
	}
	meanerr = meanerr / dimension_mechanical_vect;
	meanmov = meanmov / (*this).vertices.size(); 
	cout << "MEAN error " << meanerr << " MAX error "<< maxerr << " Error tolerence " << error_tolerance_absolute <<endl; 
	cout << "MEAN mov " << meanmov << " MAX mov " << maxmov << endl;*/


	int i = 0;
	//Subtract the tolerated from error from the observed error (for all components)
	for (int index = 0; index != dimension_mechanical_vect; index++){
		excess_error[2 * i] = y_error[index].x - upper_fractional_tolerance * error_tolerance_absolute;
		excess_error.at(2 * i + 1) = y_error.at(index).y - upper_fractional_tolerance * error_tolerance_absolute;
		//Store the ratio of error to tolerance.
		error_ratio.at(2 * i) = y_error.at(index).x / error_tolerance_absolute;
		error_ratio.at(2 * i + 1) = y_error.at(index).y / error_tolerance_absolute;
		i++;
	}

	//Store the maximum error ratio.
	max_error_ratio = *max_element(error_ratio.begin(), error_ratio.end());
	//Store the maximum error check.
	max_error_check = *max_element(excess_error.begin(), excess_error.end());

	//If any of the components violated the error tolerance as enforced, set a factor for scaling the step size.
	if (max_error_check > 0.0){
		scale_factor_double_step = safety_factor * pow(max_error_ratio, -1 / consistency_order);
	}

	i = 0;
	//Check if all of the error is below the low fractional tolerance (scale up step size)
	for (int index = 0; index != dimension_mechanical_vect; index++){
		low_error.at(2 * i) = y_error.at(index).x - lower_fractional_tolerance * error_tolerance_absolute;
		low_error.at(2 * i + 1) = y_error.at(index).y - lower_fractional_tolerance * error_tolerance_absolute;
		i++;
	}

	//If all of the errors are less than half of the tolerance, define a factor for upscaling the step size.
	max_error_check = *max_element(low_error.begin(), low_error.end());
	if (max_error_check < 0.0){
		scale_factor_double_step = safety_factor * pow(max_error_ratio, -1 / (consistency_order + 1));
	}

	//If the up scale factor is greater than the upper bound, snap the scale factor to the upper bound.
	if (scale_factor_double_step > upper_bound_scaling_factor){
		scale_factor_double_step = upper_bound_scaling_factor;
	}
	if (scale_factor_double_step < 1.00){
		step_error_check = true;
	}
	//If the scale factor is lower than the lower bound, snap the scale factor to the lower bound.
	if (scale_factor_double_step < lower_bound_scaling_factor){
		scale_factor_double_step = lower_bound_scaling_factor;
	}

	//Adjust the step size.
	adjusted_step_size = *initial_step_size * scale_factor_double_step;
	*initial_step_size = adjusted_step_size;

	//Return the flag for if the step size was too large.
	return step_error_check;
}

//! Does the same thing as HelperEvolve_UpdatePosition but to a single vertex.
void Tissue::HelperResolve_UpdatePosition(Vertex* vptr){
	double newx = (*vptr).rnew.x;
	double newy = (*vptr).rnew.y;
	//for x
	if (newx > LX){
		(*vptr).r.x = newx - LX;
		(*vptr).ChangeXEdgeFlags();
	}
	else if (newx < 0){
		(*vptr).r.x = LX + newx;
		(*vptr).ChangeXEdgeFlags();
	}
	else{
		(*vptr).r.x = newx;
	}
	//for y
	if (newy > LY){
		(*vptr).r.y = newy - LY;
		(*vptr).ChangeYEdgeFlags();
	}
	else if (newy < 0){
		(*vptr).r.y = LY + newy;
		(*vptr).ChangeYEdgeFlags();
	}
	else{
		(*vptr).r.y = newy;
	}
}

// Writes the current state of the simulation to file. The file is formatted like InputTemplate.txt and thus can be readily read by ReadInputData(). 
// Actual implementation is in HelperWriteOutputData_Write2File().
// \note This function will only write the essential data, enough for ReadInputData() to reconstruct the simulation state. All other information (e.g. cell area, pressure, etc) can be calculated from the data.
// \note For cases where this extra information is needed (e.g. for visualization with MATLAB, debugging, etc) use function WriteOutputData4Matlab(string fileName) instead.
void Tissue::WriteOutputData(string fileName){
	ofstream datafile;
	datafile.open(fileName);
	(*this).HelperWriteOutputData_Write2File(&datafile, false);
	datafile.close();
}

//Assigns the cells in the tissue A0 based on an inverse Gaussian Distrabution with skew lambda
void Tissue::AssignA0_InverseGaussian(double lambda){
	double tissuesize = LX*LY;
	int numbercells = cells.size();
	double A0avg = tissuesize / numbercells;

	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++){
		(*(*cit)).A0 = A0avg * inverseGaussian(1, lambda);
	}
}

double inverseGaussian(double mu, double lambda){
	normal_distribution<> normaldist(0, 1);  //creates a normal distrabution with a mean of 0 and stdv of 1
	uniform_real_distribution<> uniformdist(0, 1); //creates a uniform distrabution on the interval 0 to 1

	double v = normaldist(generator);			//sample from a normal distrabution with mu=0 and stdev=1
	double y = v*v;
	double x = mu + (mu*mu*y) / (2 * lambda) - (mu / (2 * lambda)) * sqrt(4 * mu*lambda*y + mu*mu*y*y);
	double test = uniformdist(generator);  // sample from a uniform distribution between 0 and 1
	if (test <= (mu) / (mu + x)){ return x; }
	else { return (mu*mu) / x; }
}

//Returns the average length of all edges in the tissue
double Tissue::GetAvgEdgeLength(){
	double lsum; 
	double Nedge = (*this).edges.size(); 

	lsum = 0;
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		lsum += (*(*eit)).GetLength(); 
	}

	return lsum / Nedge; 
}

void calculateexternalstress_linear(double maxtime){
	if (CURRENTTIME < 0){
		TISSUE_EXT_STRESS_X = TISSUE_EXT_STRESS_X;
		TISSUE_EXT_STRESS_Y = TISSUE_EXT_STRESS_Y;
	}
	else if (CURRENTTIME > maxtime){
		TISSUE_EXT_STRESS_X = TISSUE_EXT_STRESS_X_MAX; 
		TISSUE_EXT_STRESS_Y = TISSUE_EXT_STRESS_Y_MAX; 
	}
	else if (CURRENTTIME >= 0 &&   CURRENTTIME < maxtime){
		TISSUE_EXT_STRESS_X = (TISSUE_EXT_STRESS_X_MAX - TISSUE_EXT_STRESS) / maxtime*CURRENTTIME + TISSUE_EXT_STRESS;
		TISSUE_EXT_STRESS_Y = (TISSUE_EXT_STRESS_Y_MAX - TISSUE_EXT_STRESS) / maxtime*CURRENTTIME + TISSUE_EXT_STRESS;
	}


}