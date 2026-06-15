#include <cstdlib>
#include <list>
#include <vector> 
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <ctime>
#include <random>
#include <typeinfo>
#include <cassert>
#include <numeric>
#include <stddef.h>

#include "Constants.h"
#include "DynamicEdge.h"
#include "EdgeClass.h"
#include "CellClass.h"
#include "AGCell.h"
#include "DynamicConstants.h"

using namespace std;

double Edge_Dynamic::GetTension_Magnitude() { //tension eqn from Banerjee 2019 
	double currentLength; //the current length of the edge
	double currentRestLength; //the current rest length of the edge
	double SpringConst = Y; //spring constant for the edge
	double tension;

	currentLength = (*this).GetLength(); //get the current length of the edge
	currentRestLength = (*this).GetRestLength(); //get the current rest length of the edge
	tension = SpringConst * ((currentLength - currentRestLength)/currentRestLength); //edge tension calculation

	return tension;
}


double Edge_Dynamic::GetRestLength() { //return edge's rest length
	return L0;
}


//Returns a vector of the force on the vertex using the tension of the removed edge
Vect Edge_Dynamic::GetTension_ForceOnVertex(Vertex* v, double* ptr_savedTension_T1) {
	double angle = (*this).GetAngle(v);
	Vect origin(0.0, 0.0);						//define the origin
	double mag = *ptr_savedTension_T1;			//get the magnitude of the tension
	Vect ret = Vect(origin, angle, mag);
	return ret;				//same magnitude for all tension
}