//! \file Constants.h
//! This file contains declaration of global variables/parameters for the simulation.
//! The following variables are global and thus are accessible from any function in the simulation.
//! Note that this file only contains declaration of the global variables. Their values should be set in the Main.cpp file or through file/user input.

// include guard
#ifndef __CONSTANTS_H_INCLUDED__
#define __CONSTANTS_H_INCLUDED__

#include <string>
#include <random>

using namespace std;

const double PI = 3.14159265359;
const double E = 2.71828182845;

extern string InputFileName;
extern string INTEGRATORNAME;						//!< This stores which mechanical integrator is being used.
extern int SEED;									//!< Seed for Mersenne-Twister random number generator. Same inital seed should produce the same sequence of randon number (and hence the same simulation trajectory, everything else being equal).
extern double LX;									//!< Length of the tissue/simulation box in x direction. Can change dynamically based on overall tissue stress.
extern double LY;	
extern double LX0;                                  //!< Length of the tissue/simulation box in y direction. Can change dynamically based on overall tissue stress.
extern double LY0;                                  //!< Initial length of the tissue/simulation box in x direction.

extern double LXnew;								//!< Length of the tissue/simulation box in x direction after mechanical evolution.
extern double LYnew;								//!< Length of the tissue/simulation box in y direction after mechanical evolution.
extern double A0; 
extern double MINAREA;								//!< Minimum area a cell can have before apoptosis.
extern double MINLENGTH;							//!< Minimum length an edge can have before it undergoes T1 collapse into 4-fold vertex.
extern double NEWEDGELENGTH;						//!< Length of newly-resolved edge from 4-fold vertex (should be greater than MINLENGTH!!).
extern double K;									//!< Area elasticity constant. Defined such that energy for area elasticity is given by E = 0.5*K*(A-A0)^2.
extern double GAMMA;								//!< Default tension of an edge. Defined such that energy term for tension is given by E = GAMMA*length.

extern double INITIALSTEPSIZE;						//!< Initial step-size for GSL ODE solver. The actual step-size at any given time can differ since the ODE solver implements adaptive step-size.
extern double TIMEINTERVAL;							//!< Time interval for Tissue.Evolve() to proceed before it exits and checks for T1, apoptosis, etc.
extern double TOTALTIME;							//!< Total time that the whole simulation should run.
extern double CURRENTTIME;							//!< Current simulation time. To be compared to TOTALTIME to decide if the simulation should end.

extern double alphaM;								//!< Control the inverse viscosity of vertex dynamics.
extern double TISSUE_ALPHA;							//!< How fast the box moves relative to the vertices. 1=sametimescale 100=100 times slower.  Inverse viscosity for tissue-size dynamics (i.e. determine the relative evolution speed of box sizes to that of vertex positions)
extern double error_tolerance_absolute_mechanical;	//!< error tolerence for vertex motion 
extern double TISSUE_EXT_STRESS;					//The external stress that matches the internal stress and does not change the box size
extern double TISSUE_EXT_STRESS_X_MAX;					//!< External stress applied to tissue along x direction. E.g.: if external tension is greater than tissue internal (inward) tension, then the tissue will expand along that direction.
extern double TISSUE_EXT_STRESS_Y_MAX;					//!< External stress applied to tissue along y direction. E.g.: if external tension is greater than tissue internal (inward) tension, then the tissue will expand along that direction.
extern double TISSUE_EXT_STRESS_X;
extern double TISSUE_EXT_STRESS_Y; 
extern double PULLINGDIR_X; //The pulling direction defined to investiagte ASF fiber growth
extern double PULLINGDIR_Y;
extern double TAU_STRESS;							//time it takes to go from min stress to max stress
extern mt19937_64 generator;						//!< Mersenne-Twister random number generator engine, for Mitosis.
extern uniform_real_distribution<double> distribution;
extern double PULLINGFORCE; 
extern bool FREEBOUNDARY;
extern double NUCLEATIONTHRESHOLD;
extern bool NUCLEATIONBASECRITERIA;

#endif