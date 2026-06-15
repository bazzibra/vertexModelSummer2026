//! \file AGConstants.h
//! This file contains declaration of global variables/parameters for the vertex model extention, AGVertex.
//! The following variables are global and thus are accessible from any function in the simulation.
//! Note that this file only contains declaration of the global variables. Their values should be set in the Main.cpp file or through file/user input.

// include guard
#ifndef __AGCONSTANTS_H_INCLUDED__
#define __AGCONSTANTS_H_INCLUDED__

#include <string>
#include <random>

using namespace std;

extern double TCJSTRENGTH;
extern double THETAAN;
extern double ANSTRENGTH;
extern bool MITOSISON;
extern bool APOPTOSISON;
extern bool T1ON;
extern bool WRITEMITOSIS;
extern bool VERBOSEON;
extern bool EQUILIBRIUMMITOSIS;
extern double C4;
#endif

