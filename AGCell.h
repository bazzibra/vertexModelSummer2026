// define guard
#ifndef __AGCELL_H_INCLUDED__
#define __AGCELL_H_INCLUDED__

// forward class dependencies
class Cell; 
class Vertex;
class Vect;
class Edge;

//includes 
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <random>

#include "CellClass.h"

using namespace std;

// AGCell = developed for anisotropically growing models.
class AGCell : public Cell{
public:

//***************************************************************************************************************
//********** Additional Data ************************************************************************************
//***************************************************************************************************************
	//double ThetaTCJ; 
	//double TCJAnisotropy;
	double TCJSTRENGTH;
	double THETAAN;
	double ANSTRENGTH;
	string DIVISIONPOTENTIAL;
	bool xflag;
	bool yflag;
	double currentEnergy; // most recent energy measure
	double pastEnergy; // energy of cell at previous measurement
	string pressureType; // labels which pressure function to use
//***************************************************************************************************************
//*********** CONSTRUCTORS AND DESTRUCTORS **********************************************************************
//***************************************************************************************************************
	AGCell(){};
	virtual ~AGCell(){};
	//destructor: does not need to be called by you unless you created the object using the word "new" otherwise the compiler will take care of this for you
	AGCell(int idnumb, vector<Edge*> edglist, int typeinput, double A0input){
		id = idnumb;
		edges = edglist;
		type = typeinput;
		A0 = A0input;
	}
	
	AGCell(int idnumb, vector<Edge*> edglist, int typeinput, double A0input, double tcjstrength, double thetaan, double anstrength, string divp,string cellpotentialtype){
		id = idnumb;
		edges = edglist;
		type = typeinput;
		A0 = A0input;
		TCJSTRENGTH = tcjstrength;
		THETAAN = thetaan;
		ANSTRENGTH = anstrength;
		DIVISIONPOTENTIAL = divp;
		pressureType = cellpotentialtype;
	}
	

//***************************************************************************************************************
//*********** FUNCTIONS FOR CALCULATING CELL PROPERTIES **********************************************************************
//***************************************************************************************************************
	//! Same as GetPressure() but with cell area as input. Attention This function contains the actual implementation of GetPressure. All other GetPressure functions should call this function to prevent multiple implementations. Furthermore any modification/extension to the algorithm should only be made here.
	double GetPressure(double Area) override;

	virtual double GetNegativeDPressureDArea(double Area); // Useful for shear modulus calculation

	double GetThetaTCJ(); // helps determine orientation of cell division using TCJ distribution

	double GetTCJAnisotropy(); // measure of anisotropy in cells 

	list<Vect> GetBarycentricUnitVectors(); //calculates unit vectors between TCJ's and cell center
	// this is a helper for getThetaTC() and GetBiopolarityTensor()

	vector<vector<double>> GetBipolarityTensor(); // uses barycentric unit vectors to calculate bipolarity tensor
	// this is a helper for GetThetaTCJ()
	// Is this different from Meryl's moment of inertia tensor? might be worth checking between cellclass and agcell (10/11/2023)

	bool IsCrossingX(); // checks whether a cell is crossing the X-boundary

	bool IsCrossingY(); // checks whether a cell is crossing the Y-boundary

	virtual Vect GetdAdv(Vertex* v); // gives the vector corresponding to the derivative of a cell area with respect to a vertex 

	virtual Vect GetdPdv(Vertex* v); // gives the vector corresponding to the derivative of a cell perimeter with respect to a vertex 

	// is used
	virtual double GetdAdLx(); // gets the derivative of the cell's area w.r.t LX

	// is used
	virtual double GetdAdLy(); // gets the derivative of the cell's area w.r.t. LY

	virtual double Getd2AdLxdy(Vertex* v); 
	// gets the derivative of cell area with respect to gamma and a vertex y-coordinate (the derivative with respect to gamma and a vertex x-coordinate would be 0)

	virtual double GetdAdgammaForSimpleShearAlongXAxis(); 
	// gets the derivative of the cells area w.r.t. gamma if gamma is the shear strength for a simple shear in the x-direction

	virtual double GetdAdgammaForSimpleShearAlongXAxisALTERNATE();
	// this is the older version of dAdgamma - and it should be less correct by my vertex model calculation is getting worse...

	virtual double Getd2AdgammadyForSimpleShearAlongXAxis(Vertex* v);
	// gets the derivative of the cells area w.r.t. gamma and a vertex y coordinate if gamma is the shear strength for a simple shear in the x-direction

	virtual double GetCellStressXX(); // gets the xx component of the cell's average cauchy stress tensor

	virtual double GetCellStressYY(); // gets the yy component of the cell's average cauchy stress tensor

	virtual double GetCellStressInterpolateXX(); // uses x forces interpolated across edges to calculate Cauchy
	// stress tensor. Not clear if this is accurate, this is a function to test that (see Lee and Morishita 2017, stress tensor A)

	virtual double GetCellStressInterpolateYY(); // uses y forces interpolated across edges to calculate Cauchy
	// stress tensor. Not clear if this is accurate, this is a function to test that

	virtual double GetEnergy(); // calculates energy attributed to cell (takes half of the tension forces along each edge)

//************************************************************************************************************************
//********************** Division Angle for Mitosis Functions ***********************************************************
//***********************************************************************************************************************
	double TCJDivProbability(double phi);
	// outputs the probability of phi being selected as a cleavage angle, using a Boltzmann distribution with
	// the MudTCJ potential (see Bosveld et al, 2016)

	double AnisotropicDivProbability(double phi);
	// outputs the probability of a certain angle phi being selected as cleavage angle, taking "strength" as a 
	// parameter for a potential inspired by that of magnetic materials (  U = STRENGTH * cos^2(phi)  )

	double CombTCJANProbability(double phi);
	// outputs the probability of a certain angle phi being selected as cleavage angle, taking a strength
	// parameter to indicate strength of anisotropy. The potential for this is the combination of the 
	// MudTCJ potential and the anisotropic material potential used in AnisotropicDivProbability

	double CombTCJANProbability(double phi, double thetatcj);
	// outputs the probability of a certain angle phi being selected as cleavage angle, taking a strength
	// parameter to indicate strength of anisotropy. The potential for this is the combination of the 
	// MudTCJ potential and the anisotropic material potential used in AnisotropicDivProbability
	// adding thetatcj as an extra parameter for convenience

	double CombTCJANProbMaximum();
	// outputs the maximum of the probability distribution of the combined TCJ/anisotropic division potentials
	// this is necessary for the rejection sampling algorithm (in DivisionAngleDistribution) so that we can
	// properly scale proposal random variable 

	double CombTCJANProbMaximum(double thetatcj);
	// outputs the maximum of the probability distribution of the combined TCJ/anisotropic division potentials
	// this is necessary for the rejection sampling algorithm (in DivisionAngleDistribution) so that we can
	// properly scale proposal random variable 
	// we add thetatcj as a parameter for making DivisionAngleDistribution()

	virtual double DivisionAngleDistribution(string potential);
	// uses rejection sampling to produce a random number distributed according to a Boltzmann distribution,
	// using a specific potential as an energy. 

	double DivisionAngleDistribution();
	// uses rejection sampling to produce a random number distributed according to a Boltzmann distribution,
	// using a specific potential as an energy. Same as DivisionAngleDistribution(string potential), just uses
	// AGCell's field "potential"


};
#endif
