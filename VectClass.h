/**	\file	VectClass.h
*	Header of Vect class.
*	Vect is a class for 2D vector. Do not confuse with C++ STL vector (std::vector) container!
*/



// include guard
#ifndef __VECTCLASS_H_INCLUDED__
#define __VECTCLASS_H_INCLUDED__

#define _USE_MATH_DEFINES		//needed to use mathematical constants like PI, etc.
#include <math.h>
#include <vector>

#include "EdgeClass.h"

using namespace std;



/** Class for 2D physical vector.
*	Do not confuse with C++ STL vector (std::vector) container!
*/
class Vect{
public:

	/********/
	/* DATA */
	/********/

	double x;		//!< x-component of the vector.
	double y;		//!< y-component of the vector.


	/****************/
	/* CONSTRUCTORS */
	/****************/

	//! Default constructor.
	//! Constructs the vector without any assignment.
	Vect(){};

	//! Constructor for double type.
	//! Constructs vector and sets the x- and y-components to 'xcomp' and 'ycomp', respectively.
	Vect(double xcomp, double ycomp){
		this->x = xcomp; this->y = ycomp;
	}

	//! Constructor for int type.
	//! Constructs vector and sets the x- and y-components to 'xcomp' and 'ycomp', respectively.
	Vect(int xcomp, int ycomp){
		this->x = xcomp; this->y = ycomp;
	}

	//!	Constructor for angle and length.
	//! Constructs vector of length 'length' and angle 'angle', shifted by 'origin'. Angle uses standard convention (positive angle is CCW with respect to positive x-axis).
	Vect(Vect origin, double angle, double length);

	/*******************************/
	/* STANDARD OPERATOR OVERLOADS */
	/*******************************/

	//! Overloads the + operator to do vector addition.
	Vect operator+(Vect u){
		return( Vect(x + u.x, y + u.y) );
	}
	//! Overloads the - operator to do vector subtraction (i.e. 'this' vector minus 'u' vector).
	Vect operator-(Vect u){
		return( Vect(x - u.x, y - u.y) );
	}
	//! Overloads the * operator to do element-wise scalar multiplication.
	Vect operator*(double C){
		return( Vect((this->x)*C, (this->y)*C) );
	}
	//! Overloads the / operator to do element-wise scalar division.
	Vect operator/(double C){
		return( Vect((this->x)/C, (this->y)/C) );
	}


	/*************/
	/* FUNCTIONS */
	/*************/

	//! Calculates magnitude/length of the vector.
	double Magnitude();

	//! Calculates angle of the vector in [-pi,+pi] radian.
	//! Standard convention applies, i.e. positive angle is CCW from positive x-axis.
	double Angle();

	//! Return absolute value of elements.
	Vect Abs();

	//! Find perpendicular vector in plane of tissue.
	Vect FindPerpendicularVector();

	//! Calculates scalar product of two vectors.
	double DotProduct(Vect u);
	
	//! Calculates cross product of two vectors (in z-direction)
	double CrossProduct(Vect u);

	//! Calculates sign of cross product of two vectors.
	//! Returns +1 if 'this' X 'u' is in the +z direction, -1 otherwise.
	int SignCrossProduct(Vect u);

	//! Calculates relative distance (magnitude of difference) between two vectors.
	double Distance(Vect u);

	//! Rotates the vector by angle 'Phi' radians.
	Vect Rotate(double Phi);
};

//other useful functions

double avg(vector<double> v);

//! Returns true if the line segments given by v0-v1 and v2-v3 intersect. Also stores the point of interesection in intersect.
//! /note Hand NULL pointer for intersect if you don't need to store the intersection.
bool test_for_Intersect(Vect v0, Vect v1, Vect v2, Vect v3, Vect *intersect);

//Returns true if the edge e1 and e2 cross respecting PBC  
bool EdgesIntersect(Edge* e1, Edge* e2); 

#endif