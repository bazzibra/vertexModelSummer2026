//! \file DynamicConstants.h
//! This file contains declaration of global variables/parameters for the vertex model extention, AGVertex.
//! The following variables are global and thus are accessible from any function in the simulation.
//! Note that this file only contains declaration of the global variables. Their values should be set in the Main.cpp file or through file/user input.

// include guard
#ifndef __DYNAMICCONSTANTS_H_INCLUDED__
#define __DYNAMICCONSTANTS_H_INCLUDED__

#include <string>
#include <random>

using namespace std;

extern double Y;					//!< Spring constant for GAMMA = Y(l - lo) where l is the current length and lo is the rest length of the edge.
extern double EpsilonMax;			//!< Max edge tension before edges undergo a changes in rest length.
extern double KAPPA;				 //!< Remodeling rate constant for changes in edge rest length
extern double L0;					//!< Rest length of the edge when it is first created
extern const double GAMMA0;         //!< GAMMA at the very beginning of the simulation (the const GAMMA the tissue was equilibrated with) 
extern double ExternalStressRatio;  //!< Ratio of the external stress to the maximum edge tension before edges undergo a changes in rest length. Used to calculate TISSUE_EXT_STRESS_Y in DynamicMain.cpp
extern int TOTALT1; 
#endif

 

