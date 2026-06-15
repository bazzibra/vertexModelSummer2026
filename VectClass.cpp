/**	\file	VectClass.cpp
*	Implementation of Vect class.
*	Vect is a class for 2D vector. Do not confuse with C++ STL vector (std::vector) container!
*/


#include "VectClass.h"
#include "VertexClass.h"
#include "CellClass.h"
#include "EdgeClass.h"
#include <vector>

// Constructor for angle and length.
// Constructs vector of length 'length' and angle 'angle', shifted by 'origin'. Angle uses standard convention (positive angle is CCW with respect to positive x-axis).
Vect::Vect(Vect origin, double angle, double length){
	double xcomp, ycomp;
	xcomp=length*cos(angle);
	ycomp=length*sin(angle);
	
	x= origin.x + xcomp;
	y= origin.y + ycomp; 
}


// Calculates magnitude/length of the vector.
double Vect::Magnitude(){
	return( sqrt( this->DotProduct(*this )) );	//sqrt of dot product with itself
}


// Calculates angle of the vector in [-pi,+pi] radian.
// Standard convention applies, i.e. positive angle is CCW from positive x-axis.
double Vect::Angle(){
	return( atan2(y, x) );
}


// Return absolute value of elements.
Vect Vect::Abs(){
	return Vect(fabs(x), fabs(y));
}

//! Find perpendicular vector in plane of tissue.
Vect Vect::FindPerpendicularVector(){
	//zhat = (0.0, 0.0, 1.0);
	Vect perp_vector = Vect(y, -x);

	return perp_vector;
}


// Calculates scalar product of two vectors.
double Vect::DotProduct(Vect u){
	return( (x*u.x + y*u.y) );
}

// Calculates cross product of two vectors (in z-direction)
double Vect::CrossProduct(Vect u){
	return((x * u.y) - (y * u.x));
}


// Calculates sign of cross product of two vectors.
// Returns +1 if 'this' X 'u' is in the +z direction, -1 otherwise.
int Vect::SignCrossProduct(Vect u){
	double x1=x;
	double y1=y;
	double x2=u.x;
	double y2=u.y;

	double zcomponent = (*this).CrossProduct(u);

	if( zcomponent >= 0)
		return 1;
	else
		return -1;
}

// Calculates relative distance (magnitude of difference) between two vectors.
double Vect::Distance(Vect u){
	return( (*this-u).Magnitude() );
}

// Rotates the vector by angle 'Phi' radians.
Vect Vect::Rotate(double Phi){
	double xnew = x*cos(Phi) - y*sin(Phi);
	double ynew = x*sin(Phi) + y*cos(Phi);
	return (Vect(xnew, ynew));
}


// other useful functions not related to vect 

double avg(vector<double> v){
	int i;
	double sum=0;
	for(i=0; i<v.size(); i++){
		sum=sum+v[i];
	}
	return sum/(v.size());
}

//! Returns true if the line segments given by v0-v1 and v2-v3 intersect. Also stores the point of interesection in intersect.
//! /note Hand NULL pointer for intersect if you don't need to store the intersection.
bool test_for_Intersect(Vect v0, Vect v1, Vect v2, Vect v3, Vect *intersect){
	//first get the x and y conponents of the points that define the lines
	double p0_x, p0_y, p1_x, p1_y, p2_x, p2_y, p3_x, p3_y;
	Vect testforcross;
	p0_x = v0.x;
	p0_y = v0.y;
	p1_x = v1.x;
	p1_y = v1.y;
	p2_x = v2.x;
	p2_y = v2.y;
	p3_x = v3.x;
	p3_y = v3.y;

	//now do the calculation using a known algorythem 
	double s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	double s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1){
		// Collision detected
		// Take into account that the new vertex position may be out of the domain
		if (intersect != NULL) {
			(*intersect).x = p0_x + (t * s1_x);

		}
		if (intersect != NULL) {
			(*intersect).y = p0_y + (t * s1_y);

		}
		return 1;
	}

	return 0; // No collision
}

//Returns true if the edge e1 and e2 cross including PBC  
bool EdgesIntersect(Edge* e1, Edge* e2){
	// First get the positions of the vertices
	double p0_x = (*(*e1).vertex1).r.x;
	double p0_y = (*(*e1).vertex1).r.y;
	double p1_x = (*(*e1).vertex2).r.x;
	double p1_y = (*(*e1).vertex2).r.y;
	double p2_x = (*(*e2).vertex1).r.x;
	double p2_y = (*(*e2).vertex1).r.y;
	double p3_x = (*(*e2).vertex2).r.x;
	double p3_y = (*(*e2).vertex2).r.y;

	//Next deal with any boundary conditions 
	if ((*e1).xflag){
		if (p0_x < p1_x) p0_x = p0_x + LX; 
		else p1_x = p1_x + LX;
	}
	if ((*e2).xflag){
		if (p2_x < p3_x) p2_x = p2_x + LX;
		else p3_x = p3_x + LX;
	}
	if ((*e1).yflag){
		if (p0_y < p1_y) p0_y = p0_y + LY;
		else p1_y = p1_y + LY;
	}
	if ((*e2).yflag){
		if (p2_y < p3_y) p2_y = p2_y + LY;
		else p3_y = p3_y + LY;
	}

	//now do the calculation using a known algorythem 
	double s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	double s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1){ // Collision detected
		return 1;
	}
	else{ return 0; }
}
