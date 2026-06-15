/**	\file	CellClass.cpp
*	Implementation of Cell class.
*	Contains function definitions associated with Cell objects.
*/

#include <iostream>
#include <cassert>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <algorithm>
#include <stddef.h>

#include "TissueClass.h"
#include "VertexClass.h"
#include "CellClass.h"
#include "EdgeClass.h"
#include "VectClass.h"
#include "Constants.h"

using namespace std;

// Tests if two cells are the same.
// Compares ID of the two cells and returns 'true' if they are the same, 'false' otherwise.
bool Cell::operator==(const Cell &right) const{
	//check that the id's are the same
	if (id == right.id)
		return true;
	else
		return false;
}

// Returns pointers to all vertices of the cell.
vector<Vertex*> Cell::GetVertices(){
	int i, j;
	vector<Vertex*> verts;							//vector of pointers used as intermediate storage
	vector<Vertex*> returnverts;					//return a vector of pointers to vertices

	for(i=0; i<edges.size(); i++){					//for each neighboring edge
		verts.push_back((*edges[i]).vertex1);			//add the first vertex to the vector
		verts.push_back((*edges[i]).vertex2);			//add the second vertex to the vector
	}

	//Now remove the duplicates in the vector
	returnverts.push_back(verts[0]);				//start by coppying over the first vertex so the list to return has an element
	for(j=0; j<verts.size(); j++){					//for each cell in cells
		if( find(returnverts.begin(), returnverts.end(), verts[j])==returnverts.end()) {			//if the curent vertex is NOT in the list of returnvertices
			returnverts.push_back(verts[j]);														//add it to the return list
		}
	}
	return returnverts;
}

// Returns the number of vertices of the cell.
int Cell::GetVertexNumber(){
	return edges.size();
}

// Removes edge 'eptr' from the cell.
void Cell::RemoveEdge( Edge* eptr){
	int i;
	for (i = 0; i < edges.size(); i++){
		if( (*edges[i]) == (*eptr) ){
			edges.erase( edges.begin()+i );
		}
	}
}

// Returns TRUE if the cell crosses the X boundary, FALSE otherwise.
bool Cell::GetXFlag(){
	int i;
	int checkflag = 0;
	Edge e;

	for (i = 0; i < edges.size(); i++){			//loop through all of the edges
		e = *edges[i];
		checkflag = checkflag + e.xflag; //keep a running sum of all of the flags
	}

	if (checkflag == 0) { return 0; }
	else if (checkflag>0) { return 1; }
	else{
		cout << "ERROR with GetXFlag" << endl;
		return 0;
	}
}

// Returns TRUE if the cell crosses the Y boundary, FALSE otherwise.
bool Cell::GetYFlag(){
	int i;
	int checkflag = 0;
	Edge e;

	for (i = 0; i < edges.size(); i++){			//loop through all of the edges
		e = *edges[i];
		checkflag = checkflag + e.yflag; //keep a running sum of all of the flags
	}

	if (checkflag == 0) { return 0; }
	else if (checkflag>0) { return 1; }
	else {
		cout << "ERROR with GetYFlag" << endl;
		return 0; 
	}
}

// Returns TRUE if the cell crosses any boundary boundary, FALSE otherwise.
bool Cell::GetFlag(){
	if (!((*this).GetXFlag()) && !((*this).GetYFlag())){
		return 0;
	}
	else return 1;
}

// Returns center of the cell, respecting PBC. For cells that crosses bondaries, the function returns the center position WITHIN the box.
Vect Cell::GetCenter(){
	Vect GeoCent;
	vector<vector<double>> vertposforflag;

	vertposforflag = (*this).HelperOrderVerticesPos(); //allows positions of vertices outside of domain
	GeoCent = GetGeoCenter(vertposforflag[0], vertposforflag[1]);

	if (GeoCent.x > LX){ GeoCent.x = GeoCent.x - LX; }
	else if (GeoCent.x < 0) { GeoCent.x = GeoCent.x + LX; }
	else {GeoCent.x = GeoCent.x;}
	if (GeoCent.y > LY) { GeoCent.y = GeoCent.y - LY; }
	else if (GeoCent.y < 0) { GeoCent.y = GeoCent.y + LY; }
	else {GeoCent.y = GeoCent.y;}

	return GeoCent;
}

// Updates 'rnew' of the vertices to the center of the cell (noting that the center may be outside the box for PBC), to be used for apoptosis. This calls the function GetGeoCenter().
void Cell::MoveVertsToCenter(){
	int i, j;
	bool xshiftpos; //true if the cell has been moved to be greater than LX
	bool yshiftpos;
	vector<Vertex*> verts;
	vector<Vertex*> xshiftverts, yshiftverts, xnormalverts, ynromalverts;  //lists to keep track of which vertices have been shifted
	vector<double> xpos;
	vector<double> ypos;
	Edge* nextedg;
	Vect center;

	verts = (*this).HelperOrderVertices(); //order the vertices in verts
	

	
	//now pick out the x and y positions based on the PBC

	//No flags
	if((*this).GetFlag()==0){
		for(i=0; i<verts.size(); i++){
			xpos.push_back( (*verts[i]).r.x );
			xnormalverts.push_back( verts[i]);
			ypos.push_back( (*verts[i]).r.y );
			ynromalverts.push_back(verts[i]);
		}
	}
	//XFlags (Cell crosses the x boundary only)
	else if ( (*this).GetXFlag()==1 && (*this).GetYFlag()==0){
		i=-1;														//start the index at -1 so the first vertex is the zeroth one
		do{
			i++;													// move to the next vertex
			xpos.push_back( (*verts[i]).r.x );						//put thr ith xpos in the list
			xnormalverts.push_back( verts[i]);
			ypos.push_back( (*verts[i]).r.y );
			ynromalverts.push_back(verts[i]);
			nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
		}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going     
		//Vertices across the boundary need to have LX/LY added or subtracted accordingly
		if( (*verts[i]).r.x > (*verts[i+1]).r.x ){					//if you have crossed the boundary from high to low 
			xshiftpos=true;
			do{
				i++;													// move to the next vertex
				xpos.push_back( ((*verts[i]).r.x +LX) );				//put thr ith xpos in the list AND ADD LX
				xshiftverts.push_back(verts[i]);
				ypos.push_back( (*verts[i]).r.y );
				ynromalverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		else{															//if you have crossed the boundary from low to high
			xshiftpos=false;
			do{
				i++;													// move to the next vertex
				xpos.push_back( ((*verts[i]).r.x -LX) );				//put thr ith xpos in the list AND SUBTRACT LX
				xshiftverts.push_back(verts[i]);
				ypos.push_back( (*verts[i]).r.y );
				ynromalverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		//Now we are back to the origonal side of the boundary
		do{
			i++;													// move to the next vertex
			if(i<verts.size()){
				xpos.push_back( (*verts[i]).r.x );						//put thr ith xpos in the list
				xnormalverts.push_back( verts[i]);
				ypos.push_back( (*verts[i]).r.y );
				ynromalverts.push_back(verts[i]);
			}
		}while( i< verts.size() );								//untill you get to the end of the list of vertices

	}

	//YFlags (Cell crosses the y boundary only)
	else if ((*this).GetYFlag() == 1 && (*this).GetXFlag() == 0){
		i=-1;														//start the index at -1 so the first vertex is the zeroth one
		do{
			i++;													// move to the next vertex
			ypos.push_back( (*verts[i]).r.y );						//put thr ith xpos in the list
			ynromalverts.push_back(verts[i]);
			xpos.push_back( (*verts[i]).r.x );
			xnormalverts.push_back(verts[i]);
			nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
		}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going     
		//Vertices across the boundary need to have LX/LY added or subtracted accordingly
		if( (*verts[i]).r.y > (*verts[i+1]).r.y ){					//if you have crossed the boundary from high to low 
			yshiftpos=true;
			do{
				i++;													// move to the next vertex
				ypos.push_back( ((*verts[i]).r.y + LY) );				//put thr ith xpos in the list
				yshiftverts.push_back(verts[i]);
				xpos.push_back( (*verts[i]).r.x );
				xnormalverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[ (i+1) % verts.size() ]);		//look at the next edge in the list
			}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		else{															//if you have crossed the boundary from low to high
			yshiftpos=false;
			do{
				i++;													// move to the next vertex
				ypos.push_back( ((*verts[i]).r.y-LY) );						//put thr ith xpos in the list
				yshiftverts.push_back(verts[i]);
				xpos.push_back( (*verts[i]).r.x );
				xnormalverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		//Now we are back to the origonal side of the boundary
		do{
			i++;														// move to the next vertex
			if(i<verts.size()){											//make sure you did not already reach the end
				ypos.push_back( (*verts[i]).r.y );						//put thr ith xpos in the list
				ynromalverts.push_back(verts[i]);
				xpos.push_back( (*verts[i]).r.x );
				xnormalverts.push_back(verts[i]);
			}
			}while( i< verts.size() );								//untill you get to the end of the list of vertices
	}

	//X and Y flags
	else {
		//First do the x pos
		i=-1;														//start the index at -1 so the first vertex is the zeroth one
		do{
			i++;													// move to the next vertex
			xpos.push_back( (*verts[i]).r.x );						//put thr ith xpos in the list
			xnormalverts.push_back(verts[i]);
			nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
		}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going     
		//Vertices across the boundary need to have LX/LY added or subtracted accordingly
		if( (*verts[i]).r.x > (*verts[i+1]).r.x ){					//if you have crossed the boundary from high to low 
			xshiftpos=true;
			do{
				i++;													// move to the next vertex
				xpos.push_back( ((*verts[i]).r.x +LX) );				//put thr ith xpos in the list AND ADD LX
				xshiftverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		else{															//if you have crossed the boundary from low to high
			xshiftpos=false;
			do{
				i++;													// move to the next vertex
				xpos.push_back( ((*verts[i]).r.x -LX) );				//put thr ith xpos in the list AND SUBTRACT LX
				xshiftverts.push_back(verts[i]);
				nextedg = (*verts[i]).GetEdgeBetween(verts[(i+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).xflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		//Now we are back to the origonal side of the boundary
		do{
			i++;													// move to the next vertex
			if(i<verts.size()){	
			xpos.push_back( (*verts[i]).r.x );						//put thr ith xpos in the list
			xnormalverts.push_back(verts[i]);
			}
		}while( i< verts.size() );								//untill you get to the end of the list of vertices

		//Then do the ypos
		j=-1;														//start the index at -1 so the first vertex is the zeroth one
		do{
			j++;													// move to the next vertex
			ypos.push_back( (*verts[j]).r.y );						//put thr ith xpos in the list
			ynromalverts.push_back(verts[j]);
			nextedg = (*verts[j]).GetEdgeBetween(verts[(j+1) % verts.size()]);		//look at the next edge in the list
		}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going     
		//Vertices across the boundary need to have LX/LY added or subtracted accordingly
		if( (*verts[j]).r.y > (*verts[(j+1) % verts.size()]).r.y ){					//if you have crossed the boundary from high to low 
			yshiftpos=true;
			do{
				j++;													// move to the next vertex
				ypos.push_back( ((*verts[j]).r.y + LY) );				//put thr ith xpos in the list
				yshiftverts.push_back(verts[j]);
				nextedg = (*verts[j]).GetEdgeBetween(verts[(j+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		else{															//if you have crossed the boundary from low to high
			yshiftpos=false;
			do{
				j++;													// move to the next vertex
				ypos.push_back( ((*verts[j]).r.y-LY) );						//put thr ith xpos in the list
				yshiftverts.push_back(verts[j]);
				nextedg = (*verts[j]).GetEdgeBetween(verts[(j+1) % verts.size()]);		//look at the next edge in the list
			}while( (*nextedg).yflag==0 );								//while the next edge doesn't have a flag keep going    
		}
		//Now we are back to the origonal side of the boundary
		do{
			j++;													// move to the next vertex
			if(j<verts.size()){
				ypos.push_back( (*verts[j]).r.y );						//put thr ith xpos in the list
				ynromalverts.push_back(verts[j]);
			}
		}while( j< (verts.size()) );								//untill you get to the end of the list of vertices
	}

	//Now find the center and put all of the verts there
	center = GetGeoCenter(xpos, ypos);



	
	
	//for the x direction
	if(GetXFlag()==0){																//for the x direction
		for(i=0; i<xnormalverts.size(); i++){ (*xnormalverts[i]).rnew.x=center.x;}
	}
	else{
		if(xshiftpos){						//if the cell is shifted positive past LX
			for(i=0; i<xnormalverts.size(); i++){ (*xnormalverts[i]).rnew.x=center.x; }
			for(i=0; i<xshiftverts.size(); i++){ (*xshiftverts[i]).rnew.x=center.x-LX; }
		}
		else{
			for(i=0; i<xnormalverts.size(); i++){ (*xnormalverts[i]).rnew.x=center.x; }
			for(i=0; i<xshiftverts.size(); i++){ (*xshiftverts[i]).rnew.x=center.x+LX; }
		}
	}
	//for the y direction
	if(GetYFlag()==0){
		for(i=0; i<ynromalverts.size(); i++){ (*ynromalverts[i]).rnew.y=center.y;}
	}
	else{
		if(yshiftpos){
			for(i=0; i<ynromalverts.size(); i++){ (*ynromalverts[i]).rnew.y=center.y;}
			for(i=0; i<yshiftverts.size(); i++){ (*yshiftverts[i]).rnew.y=center.y-LY;}
		}
		else{
			for(i=0; i<ynromalverts.size(); i++){ (*ynromalverts[i]).rnew.y=center.y;}
			for(i=0; i<yshiftverts.size(); i++){ (*yshiftverts[i]).rnew.y=center.y+LY;}
		}
	}
}

// Calculates area of a polygon given position of vertices in CW order.
// Area returned may be positive or negative. Negative area means that the vertices were ordered CCW.
double Cell::HelperArea(vector<double> &xpos, vector<double> &ypos){
	int i,j;
	double area = 0;
	j = xpos.size() - 1;	//The last vertex is the 'previous' one to the first
	
	for (i = 0; i < xpos.size(); i++){
		area = area + ((xpos[j] + xpos[i]) * (ypos[j] - ypos[i]));
		j = i;				//j is previous vertex to i
    }

	return (area / 2.0);
}

// Returns a list of pointers to the vertices in either CW or CCW order.
vector<Vertex*> Cell::HelperOrderVertices(){
	int i;
	int N = edges.size();
	vector<Vertex*> orderedverts;			//place to store the output data
	orderedverts.reserve(N);

	Vertex v0;								//v0 is a vertex
	Vertex v;
	Edge e0;								//e is an edge
	Edge e;
	Edge* eptr=0;							//eptr is a pointer to an edge

	e0=*(edges[0]);							//get the first edge in the cell
	orderedverts.push_back( e0.vertex1 );	//and add its vertices it to the ordered list
	orderedverts.push_back( e0.vertex2 );	
	v0=*(e0.vertex2);

	//now keep getting the next edge and vertex in the cell untill you have the correct number of vertices
	do{
		//get the next edge
		for(i=0; i<edges.size(); i++){																									// look through all of the edges in the cell
			if( ( ((*(*edges[i]).vertex1).id == v0.id) || ((*(*edges[i]).vertex2).id == v0.id)  ) && ((*edges[i]).id != e0.id) ){		//find the edge that has v0 as a vertex and is not the edge e0 (the one you just looked at)
				e = *(edges[i]);
				eptr = edges[i];
				break; 
			}
		}
		//use that edge to get the next vertex
		orderedverts.push_back(v0.GetOtherVertex(eptr));
		v = *(v0.GetOtherVertex(eptr));
		
		//update e0,e,v0,v
		e0=e;
		v0=v;

	} while (orderedverts.size() < N);

	return orderedverts;

}

// GetArea returns the area of the cell. Actual implementation of algorithm that calculates the area is in function HelperArea(vector<double> &xpos, vector<double> &ypos).
double Cell::GetArea(){
	double area;
	vector<vector<double>> xypos;
	xypos = (*this).HelperOrderVerticesPos();
	area = (*this).HelperArea(xypos[0], xypos[1]);
	return std::fabs(area);
}

// GetArea returns the area of the cell given a precalculated list of the clockwise ordering of the vertices in the cell
double Cell::GetArea(vector<Vertex*> cwverts){
	double area;
	vector<vector<double>> xypos;

	xypos = (*this).HelperOrderVerticesPos(cwverts);
	area = (*this).HelperArea(xypos[0], xypos[1]);

	return fabs(area);
}

//Finds the perimeter of the cell
double Cell::GetPerimeter(){
	int i = 0; 
	double P; //the perimeter

	P = 0; 
	for (i = 0; i < (*this).edges.size(); i++){
		P += (*edges[i]).GetLength(); 
	}

	return P; 
}

// Returns a 2D vector of x and y position of vertices, ordered either CW or CCW. Respects PBC.
vector<vector<double>> Cell::HelperOrderVerticesPos(){
	int i, j;
	int N = edges.size();
	vector<vector<double>> returnpos;
	vector<double> xpos;
	vector<double> ypos;

	returnpos.reserve(2);
	xpos.reserve(N);
	ypos.reserve(N);

	vector<Vertex*> orderedVerts = HelperOrderVertices();						//order the vertices in verts
	vector<vector<int>> orderedFlags = HelperOrderFlags(orderedVerts);			//order vertices flags in the same way (look at HelperOrderFlags() for more details on the flags)

	vector<int> orderedFlagsX = orderedFlags[0];
	vector<int> orderedFlagsY = orderedFlags[1];

	for (int i = 0; i < N; i++){
		Vertex* v = orderedVerts[i];
		double xtemp = (*v).r.x + LX * orderedFlagsX[i];			//'undoing' the PBC. Value of flags are 0, +1, or -1 (look at HelperOrderFlags() for more details).
		double ytemp = (*v).r.y + LY * orderedFlagsY[i];
		xpos.push_back(xtemp);
		ypos.push_back(ytemp);
	}

	returnpos.push_back(xpos);
	returnpos.push_back(ypos);

	return returnpos;
}

// Faster calculation of HelperOrderVerticesPos() due to precalculated list of cwvertices 
vector<vector<double>> Cell::HelperOrderVerticesPos(vector<Vertex*> cwverts){
	int i, j;
	int N = edges.size();
	vector<vector<double>> returnpos;
	vector<double> xpos;
	vector<double> ypos;

	returnpos.reserve(2);
	xpos.reserve(N);
	ypos.reserve(N);

	vector<vector<int>> orderedFlags = HelperOrderFlags(cwverts);			//order vertices flags in the same way (look at HelperOrderFlags() for more details on the flags)
	vector<int> orderedFlagsX = orderedFlags[0];
	vector<int> orderedFlagsY = orderedFlags[1];

	for (int i = 0; i < N; i++){
		double xtemp = (*cwverts[i]).r.x + LX * orderedFlagsX[i];			//'undoing' the PBC. Value of flags are 0, +1, or -1 (look at HelperOrderFlags() for more details).
		double ytemp = (*cwverts[i]).r.y + LY * orderedFlagsY[i];
		xpos.push_back(xtemp);
		ypos.push_back(ytemp);
	}

	returnpos.push_back(xpos);
	returnpos.push_back(ypos);

	return returnpos;
}

// Faster calculation of HelperOrderVerticesPos() due to precalculated list of cwvertices: starts building vertex position list with the position of *first_vertex.
// \note This is important because with PBC, it is often useful to build a list of vertex positions that respects PBC and is on one side of the boundary (the side specified by *first_vertex).
vector<vector<double>> Cell::HelperOrderVerticesPos(vector<Vertex*> cwverts, Vertex* first_vertex){
	vector<vector<double>> returnpos;
	vector<double> xpos;
	vector<double> ypos;
	Vertex* next_vertex;
	int N = edges.size();

	returnpos.reserve(2);
	xpos.reserve(N);
	ypos.reserve(N);

	vector<vector<int>> orderedFlags = (*this).HelperOrderFlags(cwverts, first_vertex);			//order vertices flags in the same way (look at HelperOrderFlags() for more details on the flags)
	vector<int> orderedFlagsX = orderedFlags[0];
	vector<int> orderedFlagsY = orderedFlags[1];

	//Start the list with first_vertex.
	next_vertex = first_vertex;
	//Initialize the temporary position index to 0 (used to find corresponding orderedflags).
	int i = 0;

	//Loop through the ordered list of vertices (until next_vertex is first_vertex again).
	//Store vertex positions as you go (respecting PBC).
	do{
		double xtemp = (*next_vertex).r.x + LX * orderedFlagsX[i];			//'undoing' the PBC. Value of flags are 0, +1, or -1 (look at HelperOrderFlags() for more details).
		double ytemp = (*next_vertex).r.y + LY * orderedFlagsY[i];
		xpos.push_back(xtemp);
		ypos.push_back(ytemp);
		//Increment the index for the vertex positions.
		i++;
		//Store the next_vertex.
		next_vertex = (*this).GetNextCWVert(next_vertex, cwverts);
	} while (*next_vertex != *first_vertex);		//End the do ... while loop when next_vertex == first_vertex.

	returnpos.push_back(xpos);
	returnpos.push_back(ypos);

	return returnpos;
}

// Returns a vector of pointers to cells in contact with the given cell.
vector<Cell*> Cell::GetNeighboringCells(){
	int i;
	Edge e;
	Cell* other_cell_ptr;
	vector<Cell*> neighboring_cell_ptrs;

	for (i = 0; i < edges.size(); i++){					//loop through all of the edges
		e = *edges[i];
		other_cell_ptr = (e).GetOtherCell(this);		//find pointer to cell on other side of this edge
		neighboring_cell_ptrs.push_back(other_cell_ptr);//push back neighoring cell pointer
	}

	return neighboring_cell_ptrs;
}

//! Returns a vector of contact length between neighboring cells (same convention as GetNeighboringCells)
vector<double> Cell::GetNeighboringCells_ContactLength(){
	int i;
	Edge e;
	double this_edge_length;
	vector<double> edge_lengths;

	for (i = 0; i < edges.size(); i++){					//loop through all of the edges
		e = *edges[i];
		this_edge_length = e.GetLength();				//store the length of this edge (which is the interface between the two cells)
		edge_lengths.push_back(this_edge_length);		//push back length
	}

	return edge_lengths;
}

// Returns a vector of pointers to all vertices in the cell in clockwise (CW) order.
vector<Vertex*> Cell::GetClockWiseVertices(){
	double area;
	vector<Vertex*> verts;
	vector<vector<double>> xypos;

	verts = (*this).HelperOrderVertices();
	xypos = (*this).HelperOrderVerticesPos();
	area = (*this).HelperArea(xypos[0], xypos[1]);

	if (area > 0){
		return verts;
	}
	else{
		reverse(verts.begin(), verts.end());;
		return verts;
	}
}

// Returns flags of the edges between vertices given in OrderedVerts (i.e. xFlag[i] is the flag of edge between vertices OrderedVerts[i] and OrderedVerts[i+1]).
// Use HelperOrderVertices() or GetClockWiseVertices() to get OrderedVerts.
// xFlag[i] = 0 if on the same side of OrderedVerts[0], -1 if crossing the boundary from low to high, and +1 if crossing boundary from high to low.
// Hence, the position of vertices without PBC is given by x1[i] = x[i] + xFlag[i] * LX. This is useful in GetArea() calculation.
vector<vector<int>> Cell::HelperOrderFlags(vector<Vertex*> OrderedVerts){
	int N = OrderedVerts.size();
	vector<vector<int>> OrderedFlags;		//for return value

	vector<int> xFlag(N + 1, 0);
	vector<int> yFlag(N + 1, 0);

	xFlag[0] = 0;						//flag for 1st vertex is always 0 (i.e. it is on the same side as itself)
	yFlag[0] = 0;
	int noOfCrossingX = 0;
	int noOfCrossingY = 0;

	for (int i = 1; i < N + 1; i++){			//+1 so it goes one full circle back to where it started
		Vertex* v1 = OrderedVerts[i - 1];
		Vertex* v2 = OrderedVerts[i % N];
		Edge* e = (*v1).GetEdgeBetween(v2);				//NOTE: the vertices in the input vector OrderedVerts are assumed to be ordered/sequential!
		bool xflag = (*e).xflag;
		bool yflag = (*e).yflag;
		

		//walk through ordered edges and set xFlag
		if (!xflag){									//if no flag, then xFlag[i] is the same as the previous one
			xFlag[i] = xFlag[i - 1];
		}
		else if ((*v2).r.x < (*v1).r.x){				//if there is flag and x2 < x1 (i.e. crossing boundary from high to low), increment xFlag by 1
			xFlag[i] = xFlag[i - 1] + 1;
			noOfCrossingX += +1;
		}
		else{											//if there is flag and x2 > x1 (i.e. crossing boundary from low to high), decrement xFlag by 1
			xFlag[i] = xFlag[i - 1] - 1;
			noOfCrossingX += -1;
		}


		//do the same for yFlag
		if (!yflag){									//if no flag, then yFlag[i] is the same as the previous one
			yFlag[i] = yFlag[i - 1];
		}
		else if ((*v2).r.y < (*v1).r.y){				//if there is flag and y2 < y1 (i.e. crossing boundary from high to low), increment yFlag by 1
			yFlag[i] = yFlag[i - 1] + 1;
			noOfCrossingY += +1;
		}
		else{											//if there is flag and y2 > y1 (i.e. crossing boundary from low to high), decrement yFlag by 1
			yFlag[i] = yFlag[i - 1] - 1;
			noOfCrossingY += -1;
		}
	}

	xFlag.pop_back();
	yFlag.pop_back();					//remove last element, since it is equal to first element

	assert((noOfCrossingX == 0) && "ERROR Cell::HelperOrderFlags - Unexpected number of x crossings. Tell JH.");
	assert((noOfCrossingY == 0) && "ERROR Cell::HelperOrderFlags - Unexpected number of y crossings. Tell JH.");

	OrderedFlags.push_back(xFlag);
	OrderedFlags.push_back(yFlag);		//prepare return value

	return OrderedFlags;
}

// This returns order vertex flags (as HelperOrderFlags(vector<Vertex*> OrderedVerts) does), but starts the list with *first_vertex.
// \note This is important because with PBC, it is often useful to build a list of vertex positions that respects PBC and is on one side of the boundary (the side specified by *first_vertex).
vector<vector<int>> Cell::HelperOrderFlags(vector<Vertex*> OrderedVerts, Vertex* first_vertex){
	int N = OrderedVerts.size();
	vector<vector<int>> OrderedFlags;		//for return value

	vector<int> xFlag(N + 1, 0);
	vector<int> yFlag(N + 1, 0);

	xFlag[0] = 0;						//flag for 1st vertex is always 0 (i.e. it is on the same side as itself)
	yFlag[0] = 0;
	int noOfCrossingX = 0;
	int noOfCrossingY = 0;

	//Specify the two vertices that specify the first edge.
	Vertex* v1 = first_vertex;
	Vertex* v2 = (*this).GetNextCWVert(first_vertex, OrderedVerts);
	int i = 1;

	//Loop through the ordered list of vertices (until v1 again reaches first_vertex).
	//Store ordered flags as you loop through the vertices in this order.
	do{
		Edge* e = (*v1).GetEdgeBetween(v2);				//NOTE: the vertices in the input vector OrderedVerts are assumed to be ordered/sequential!
		bool xflag = (*e).xflag;
		bool yflag = (*e).yflag;


		//walk through ordered edges and set xFlag
		if (!xflag){									//if no flag, then xFlag[i] is the same as the previous one
			xFlag[i] = xFlag[i - 1];
		}
		else if ((*v2).r.x < (*v1).r.x){				//if there is flag and x2 < x1 (i.e. crossing boundary from high to low), increment xFlag by 1
			xFlag[i] = xFlag[i - 1] + 1;
			noOfCrossingX += +1;
		}
		else{											//if there is flag and x2 > x1 (i.e. crossing boundary from low to high), decrement xFlag by 1
			xFlag[i] = xFlag[i - 1] - 1;
			noOfCrossingX += -1;
		}


		//do the same for yFlag
		if (!yflag){									//if no flag, then yFlag[i] is the same as the previous one
			yFlag[i] = yFlag[i - 1];
		}
		else if ((*v2).r.y < (*v1).r.y){				//if there is flag and y2 < y1 (i.e. crossing boundary from high to low), increment yFlag by 1
			yFlag[i] = yFlag[i - 1] + 1;
			noOfCrossingY += +1;
		}
		else{											//if there is flag and y2 > y1 (i.e. crossing boundary from low to high), decrement yFlag by 1
			yFlag[i] = yFlag[i - 1] - 1;
			noOfCrossingY += -1;
		}
		//Increment i to indicate that you have stored another ordered flag.
		i++;
		//Update v1 and v2, move one step further clockwise.
		v1 = v2;
		v2 = (*this).GetNextCWVert(v2, OrderedVerts);
	} while (*v1 != *first_vertex);		//Stop the do ... while loop when v1 loops back to first_vertex.

	xFlag.pop_back();
	yFlag.pop_back();					//remove last element, since it is equal to first element

	assert((noOfCrossingX == 0) && "ERROR Cell::HelperOrderFlags - Unexpected number of x crossings. Tell JH.");
	assert((noOfCrossingY == 0) && "ERROR Cell::HelperOrderFlags - Unexpected number of y crossings. Tell JH.");

	OrderedFlags.push_back(xFlag);
	OrderedFlags.push_back(yFlag);		//prepare return value

	return OrderedFlags;
}

//! Removes an edge from the list of edges.
void Cell::HelperRemoveEdge(Edge* eptr){
	int i;
	for(i=0; i<edges.size(); i++){
		if( (*edges[i])==(*eptr) ){
			edges.erase( edges.begin()+i );
		}
	}
}

//Given a list of CW vertices in the cell it returns the next vertex after v0
Vertex* Cell::GetNextCWVert(Vertex* v0, vector<Vertex*> cwvert){
	int i;
	long int v0id = (*v0).id;
	int N = cwvert.size();
	Vertex* vptr = 0;
	for (i = 0; i < N; i++){
		if (v0id == (*cwvert[i]).id){
			vptr = &(*cwvert[(i + 1) % N]);
			return vptr;
		}
	}
	assert((vptr == 0) && "ERROR Cell::GetNextCWVert");
	return vptr;
}

//Returns 0 if given edge is not part of this cell and 1 if it is (used for error checking)
bool Cell::IsEdgeInCell(Edge* e){
	bool flag = 0; 
	int i = 0; 
	long int edgeid = (*e).id; 
	for (i = 0; i < edges.size(); i++){
		if (edgeid == (*edges[i]).id){
			flag = 1; 
		}
	}
	return flag; 
}

//Returns true if poit is in the cell
bool Cell::IsPointInCell(Vect point){
	vector<vector<double>> vertpos; 
	bool result; 

	vertpos = HelperOrderVerticesPos(); //gets the xy cordinates of the verticies in CW or CCW order
	result = is_Point_in_Polygon(point, vertpos); 
	return result; 

}

//returns the moment of inertia tensor Ixx, Ixy, Iyx, Iyy as calculated from https://en.wikipedia.org/wiki/Second_moment_of_area
vector<double> Cell::GetMOItensor(vector<vector<double>> orderedvertpos){
	vector<double> x;  //xpositions of the verticies
	vector<double> y;  //ypositions of the verticies
	int N; //number of verts
	double Ixx, Ixy, Iyy; 
	double termxx, termyy, termxy, da; 
	int i; 
	Vect cent; 
	//first get the x and y pos
	x = orderedvertpos[0]; //note that the PBC are already taken care of by geting the ordered vertex positions
	y = orderedvertpos[1]; 
	cent = GetGeoCenter(orderedvertpos[0], orderedvertpos[1]);
	N = x.size(); 

	//Shift over by center
	for (i = 0; i < N; i++){
		x[i] = x[i] - cent.x; 
		y[i] = y[i] - cent.y; 
	}

	//get Ixx
	Ixx = 0; 
	Ixy = 0;
	Iyy = 0; 
	for (i = 0; i < N; i++){
		termxx = (y[i] * y[i] + y[i] * y[(i + 1) % N] + y[(i + 1) % N] * y[(i + 1) % N]);      //(yi^2+yiyi+1+yi+1^2)*(xiyi+1-xi+1yi)
		termyy = (x[i] * x[i] + x[i] * x[(i + 1) % N] + x[(i + 1) % N] * x[(i + 1) % N]);      //(xi^2+xixi+1+xi+1^2)*(xiyi+1-xi+1yi)
		termxy = (x[i] * y[(i + 1) % N] + 2 * x[i] * y[i] + 2 * x[(i + 1) % N] * y[(i + 1) % N] + x[(i + 1) % N] * y[i]);      //(xi*yi+1 +2xiyi+2xi+1yi+1 +xi+1yi)*(xiyi+1-xi+1yi)
		da = (x[i] * y[(i + 1) % N] - x[(i + 1) % N] * y[i]);
		Ixx = Ixx + termxx * da;
		Iyy = Iyy + termyy * da;
		Ixy = Ixy + termxy * da;
	}
	Ixx = abs(Ixx / 12); 
	Iyy = abs(Iyy / 12);
	Ixy = abs(Ixy / 24);

	//parallel axis theorem 


	//return values
	vector<double> result; 
	result.push_back(Ixx); 
	result.push_back(Ixy);
	result.push_back(Ixy);
	result.push_back(Iyy);

	return result; 
}

//Returns the length of the line at pos x (or y) that goes through the cell
double Cell::GetLengthHorzCut(double ycut){
	double result; 
	vector<vector<double>> orderedvertpos;

	//First get the positions of the vertices
	orderedvertpos = HelperOrderVerticesPos();

	//then call the fast version of the function 
	result = GetLengthHorzCut(ycut, orderedvertpos); 
	return result; 
}
double Cell::GetLengthVertCut(double ycut){
	double result;
	vector<vector<double>> orderedvertpos;

	//First get the positions of the vertices
	orderedvertpos = HelperOrderVerticesPos();

	//then call the fast version of the function 
	result = GetLengthVertCut(ycut, orderedvertpos);
	return result;
}

//returns the aspect ratio from the eigenvalues of the moment of inertia tensor AR=sqrt(I1/I2)
double Cell::GetAspectRatio(){
	//first get the ordered verts
	vector<Vertex*> Orderedverts; 
	Orderedverts = (*this).GetClockWiseVertices(); 

	//now call fast version
	double result = (*this).GetAspectRatio(Orderedverts); 

	return result; 
}
double Cell::GetAspectRatio(vector<Vertex*> OrderedVerts){
	double Ixx, Ixy, Iyx, Iyy; 
	vector<double> MOItensor; 
	vector<vector<double>> orderedvertpos; 
	double e1, e2, area; 

	//Get the moment of inertia tensor 
	orderedvertpos = (*this).HelperOrderVerticesPos(OrderedVerts); 
	MOItensor = (*this).GetMOItensor(orderedvertpos); 
	area = (*this).GetArea(OrderedVerts); 

	Ixx = MOItensor[0]; 
	Ixy = MOItensor[1];
	Iyx = MOItensor[2];
	Iyy = MOItensor[3];

	double trace = Ixx + Iyy;
	double det = Ixx*Iyy - Ixy*Iyx; 
	
	e1 = trace / 2 + sqrt(trace*trace / 4 - det);
	e2 = trace / 2 - sqrt(trace*trace / 4 - det);

	return sqrt(e1 / e2); 
}

// This is a general static function that calculates the geometrical center of a polygon, given some x positions and y positions of vertices.
// Please note that that geometrical center may be outside of the boundary if some of these specified input position values are outside of the boundary. GetCenter() will perform additional calculations to ensure the center is within the box.
// \note Vertex positions must be listed in order (either CW or CCW).
Vect GetGeoCenter(vector<double> xpos, vector<double> ypos){
	int numbervert;
	double signedarea;
	Vect geocenterpos;
	int i, j, k;

	numbervert = xpos.size(); //stores number of vertices in cell

	//All of these loop make sure that each possible pair of neighboring indices (going along the edges in order) is added
	//This uses a general formula for the geometrical center of a polygon.
	signedarea = 0;
	for (i = 0; i<numbervert - 1; i++){
		signedarea = signedarea + ((xpos[i])*(ypos[i + 1]) - (xpos[i + 1])*(ypos[i])) / 2;
	}
	signedarea = signedarea + ((xpos[numbervert - 1])*(ypos[0]) - (xpos[0])*(ypos[numbervert - 1])) / 2;
	// uses cross product formula for area of polygon, weights using area.
	geocenterpos.x = 0;
	for (j = 0; j<numbervert - 1; j++){
		geocenterpos.x = geocenterpos.x + (xpos[j] + xpos[j + 1])*((xpos[j])*(ypos[j + 1]) - (xpos[j + 1])*(ypos[j])) / (6 * signedarea);
	}
	geocenterpos.x = geocenterpos.x + (xpos[numbervert - 1] + xpos[0])*((xpos[numbervert - 1])*(ypos[0]) - (xpos[0])*(ypos[numbervert - 1])) / (6 * signedarea);

	geocenterpos.y = 0;
	for (k = 0; k<numbervert - 1; k++){
		geocenterpos.y = geocenterpos.y + (ypos[k] + ypos[k + 1])*((xpos[k])*(ypos[k + 1]) - (xpos[k + 1])*(ypos[k])) / (6 * signedarea);
	}
	geocenterpos.y = geocenterpos.y + (ypos[numbervert - 1] + ypos[0])*((xpos[numbervert - 1])*(ypos[0]) - (xpos[0])*(ypos[numbervert - 1])) / (6 * signedarea);
	return geocenterpos;
}

// Tests if the cell is convex.
// Returns TRUE if the cell is convex, FALSE otherwise. Takes as input list of ordered vertex positions, and uses cross product along the polygon to determine convexity.
// \note If you already have orderedvertices stored, use HelperOrderVerticesPos(const double y[], unordered_map<long, int> verts_id2index, vector<Vertex*> orderedVerts); otherwise use HelperVerticesPos().
bool CheckConvex(vector<vector<double>> xypos) {
	int vertnumb;
	int i, j;
	double dx1 = 0;
	double dx2 = 0;
	double dy1 = 0;
	double dy2 = 0;
	Vect dr1, dr2;
	vector<double> crossprodstore;
	bool flag = true;

	vertnumb = (xypos[0]).size();

	//goes around the cell storing the cross product of each pair of adjacent edges
	for (i = 0; i != vertnumb - 2; i++) {
		dx1 = xypos[0][i + 1] - xypos[0][i];
		dy1 = xypos[1][i + 1] - xypos[1][i];
		dr1 = Vect(dx1, dy1);
		dx2 = xypos[0][i + 2] - xypos[0][i + 1];
		dy2 = xypos[1][i + 2] - xypos[1][i + 1];
		dr2 = Vect(dx2, dy2);
		crossprodstore.push_back(dr1.CrossProduct(dr2));
	}
	//Do the two other pairings that are necessary to go around the entire shape
	dx1 = xypos[0][vertnumb - 1] - xypos[0][vertnumb - 2];
	dy1 = xypos[1][vertnumb - 1] - xypos[1][vertnumb - 2];
	dr1 = Vect(dx1, dy1);
	dx2 = xypos[0][0] - xypos[0][vertnumb - 1];
	dy2 = xypos[1][0] - xypos[1][vertnumb - 1];
	dr2 = Vect(dx2, dy2);
	crossprodstore.push_back(dr1.CrossProduct(dr2));

	dx1 = xypos[0][0] - xypos[0][vertnumb - 1];
	dy1 = xypos[1][0] - xypos[1][vertnumb - 1];
	dr1 = Vect(dx1, dy1);
	dx2 = xypos[0][1] - xypos[0][0];
	dy2 = xypos[1][1] - xypos[1][0];
	dr2 = Vect(dx2, dy2);
	crossprodstore.push_back(dr1.CrossProduct(dr2));

	//Cross products must be all positive or all negative for the cell to be convex
	for (j = 0; j != crossprodstore.size() - 1; j++) {
		if (crossprodstore[j] * crossprodstore[j + 1] < 0.0){
			flag = false;
		}
	}
	if (crossprodstore[crossprodstore.size() - 1] * crossprodstore[0] < 0.0){
		flag = false;
	}
	return flag;
}

//! Using a list of vertex positions (vertposlist), define a ray from the geocenter to the right. 
//! Return the indices of the edge which the ray intersects. If the edge is between (vertex i, vertex i+1) in vertposlist, return i. Include all intersections.
// \note This is used for defining front propogation for PCP as well as for point in polygon.
vector<int> which_Edge_Intersection(Vect position, vector<vector<double>> xypos, double angle){
	//Define a vector for storing indices for edges that the ray intersects.
	vector<int> list_of_intersected_edges;
	//Define the length of the ray such that it will most certainly be larger than the cell.
	double length_ray = LX + LY;
	//Define the ray from position at an angle of 0.0 with length LX + LY.
	Vect ray = Vect(position, angle, length_ray);
	
	//Store the number of vertices
	int vertnumb = (xypos[0]).size();												//number of vertices
	//Store positions for two connected vertices.
	Vect r1, r2;

	//Index for intersection.
	int intersection_index;
	//Check if the ray intersect the given edge.
	bool does_intersect = false;

	//goes around the cell, checking if the ray from position intersects any edge.
	for (int i = 0; i != vertnumb - 1; i++) {
		r1 = Vect(xypos[0][i + 1], xypos[1][i + 1]);								//Store vector for first vertex for an edge.
		r2 = Vect(xypos[0][i], xypos[1][i]);										//Store vector for second vertex for an edge.
		does_intersect = test_for_Intersect(r1, r2, position, ray, NULL);			//Check if the ray intersects the edge (defined by r1, r2).
		if (does_intersect){
			intersection_index = i;
			list_of_intersected_edges.push_back(intersection_index);
		}
	}
	r1 = Vect(xypos[0][0], xypos[1][0]);											//Store vector for first vertex of last edge.
	r2 = Vect(xypos[0][vertnumb - 1], xypos[1][vertnumb - 1]);						//Store vector for second vertex of last edge.
	does_intersect = test_for_Intersect(r1, r2, position, ray, NULL);				//Check if the ray intersects the edge (defined by r1, r2).
	if (does_intersect){
		intersection_index = vertnumb - 1;
		list_of_intersected_edges.push_back(intersection_index);
	}

	return list_of_intersected_edges;
}

//! Return a vector of point where the ray defined by (cell_cut_vector_1 , cell_cut_vector_2) intersects the polygon defined by the vertex positions xypos.
// \note This is used within CalculateTissueStressX and CalculateTissueStressY to define the intersection between cells and the cutline.
vector<Vect> store_Intersections(Vect cell_cut_vector_1, Vect cell_cut_vector_2, vector<vector<double>> xypos){
	//Define a vector for storing indices for edges that the ray intersects.
	vector<int> list_of_intersected_edges;
	vector<Vect> intersection_points;

	//Store the number of vertices
	int vertnumb = (xypos[0]).size();												//number of vertices
	//Store positions for two connected vertices.
	Vect r1, r2;

	//Index for intersection.
	int intersection_index;
	//Check if the ray intersect the given edge.
	bool does_intersect = false;

	//goes around the cell, checking if the ray from position intersects any edge.
	for (int i = 0; i != vertnumb - 1; i++) {
		r1 = Vect(xypos[0][i + 1], xypos[1][i + 1]);								//Store vector for first vertex for an edge.
		r2 = Vect(xypos[0][i], xypos[1][i]);										//Store vector for second vertex for an edge.
		Vect intersection_point;
		does_intersect = test_for_Intersect(r1, r2, cell_cut_vector_1, cell_cut_vector_2, &intersection_point);			//Check if the ray intersects the edge (defined by r1, r2).
		if (does_intersect){
			intersection_index = i;
			list_of_intersected_edges.push_back(intersection_index);
			intersection_points.push_back(intersection_point);
		}
	}
	r1 = Vect(xypos[0][0], xypos[1][0]);											//Store vector for first vertex of last edge.
	r2 = Vect(xypos[0][vertnumb - 1], xypos[1][vertnumb - 1]);						//Store vector for second vertex of last edge.
	Vect intersection_point;
	does_intersect = test_for_Intersect(r1, r2, cell_cut_vector_1, cell_cut_vector_2, &intersection_point);				//Check if the ray intersects the edge (defined by r1, r2).
	if (does_intersect){
		intersection_index = vertnumb - 1;
		list_of_intersected_edges.push_back(intersection_index);
		intersection_points.push_back(intersection_point);
	}

	return intersection_points;
}

// Check if the point position is in the cell specified by vertposlist. (vertposlist list is given for speeding up calculation)
// \note This uses the ray-casting algorithm: If a ray going outward from the point in an arbitrary direction intersects the polygon an even number of times, the point is outside the polygon. 
// \note If a ray going outward from the point in an arbitrary direction intersects the polygon an odd number of times, the point is inside the polygon. 
bool is_Point_in_Polygon(Vect position, vector<vector<double>> xypos){
	double angle = 0.276;
	vector<int> list_of_intersected_edges = which_Edge_Intersection(position, xypos, angle);
	int intersect_count = list_of_intersected_edges.size();
	bool is_point_in_polygon;

	if (intersect_count % 2 == 0)													//If the number of intersections is even, the point is outside of the polygon.
		is_point_in_polygon = false;
	else
		is_point_in_polygon = true;

	return is_point_in_polygon;
}

//Returns the length of the line at pos x (or y) that goes through the cell
//Faster version that starts with the positions of the verticies (where the positions have been adjusted so that the cell may lie outside the PBC box) 
double Cell::GetLengthHorzCut(double ycut, vector<vector<double>> vertposlist){
	//the positions of the verticies have already been moved so that the cell is one continuous polygon that is not split in half by the PBC (however the positions of verticies may be outside the box)
	double miny, minx, maxy, maxx; 
	double v1x, v1y, v2x, v2y; 
	double intersecty, h; 
	vector<double> xpos = vertposlist[0]; 
	vector<double> ypos = vertposlist[1]; 
	vector<double> intersect; 
	double m, b; 
	int i; 
	int Nverts = GetVertexNumber(); 
	bool intersectcell; 

	//Get the min and max x and y values of the cell
	miny = *min_element(begin(ypos), end(ypos));
	maxy = *max_element(begin(ypos), end(ypos));

	//Move the cut (we may have moved the part of the cell that intersects the cut when we undid the PBC we now need to move the cut as well)
	if (ycut < miny) ycut = ycut + LY; 
	if (ycut > maxy) ycut = ycut - LY; 

	//For every edge get the point at which the edge intersects the ycut
	intersectcell = false; 
	for (i = 0; i < Nverts; i++){
		//Get the endpts of the edge
		v1x = xpos[i]; 
		v1y = ypos[i]; 
		v2x = xpos[(i + 1) % Nverts]; 
		v2y = ypos[(i + 1) % Nverts];

		//if the edge intersects
		if (ycut > min(v1y, v2y) && ycut < max(v1y, v2y)){
			if (v1x != v2x){
				m = ((v1y - v2y) / (v1x - v2x));
				b = v1y - m *v1x;
				intersecty = (ycut - b) / m;
			}
			else{
				intersecty = ycut;
			}
			intersect.push_back(intersecty); //add the intersect point to the list
			intersectcell = true; 
		}
	}

	//if no edge were intersected return 0
	if (!intersectcell) { return 0; }
	//otherwise get the length of the cut through the cell
	else{
		//Get the intersection points ordered from greatest to least
		sort(intersect.begin(), intersect.end()); //sorts the vector min to max
		reverse(intersect.begin(), intersect.end()); 
		h = 0; //the length of the cut through the cell
		//you must intersect the cell an even number of times (unless you managed to pass exactly through a vertex) 
		if (intersect.size() % 2 != 0){ 
			cout << "ERROR IN GetLengthHorzCut: Cut passed through vertex!" << endl; 
			return 0; 
		}
		//walking across the cut you enter and exit the cell every other intersect you incounter h=(int[0]-int[1])+(int[2]-int[3])+...
		for (i = 0; i < intersect.size() / 2; i++){ 
			h = h + (intersect[2 * i] - intersect[2 * i + 1]); 
		}
		return h; 
	}
}

double Cell::GetLengthVertCut(double xcut, vector<vector<double>> vertposlist){
	//the positions of the verticies have already been moved so that the cell is one continuous polygon that is not split in half by the PBC (however the positions of verticies may be outside the box)
	double miny, minx, maxy, maxx;
	double v1x, v1y, v2x, v2y;
	double intersectx, h;
	vector<double> xpos = vertposlist[0];
	vector<double> ypos = vertposlist[1];
	vector<double> intersect;
	double m, b; 
	int i;
	int Nverts = GetVertexNumber();
	bool intersectcell;

	//Get the min and max x and y values of the cell
	minx = *min_element(begin(xpos), end(xpos));
	maxx = *max_element(begin(xpos), end(xpos));

	//Move the cut (we may have moved the part of the cell that intersects the cut when we undid the PBC we now need to move the cut as well)
	if (xcut < minx) xcut = xcut + LX;
	if (xcut > maxx) xcut = xcut - LX;

	//For every edge get the point at which the edge intersects the ycut
	intersectcell = false;
	for (i = 0; i < Nverts; i++){
		//Get the endpts of the edge
		v1x = xpos[i];
		v1y = ypos[i];
		v2x = xpos[(i + 1) % Nverts];
		v2y = ypos[(i + 1) % Nverts];

		//if the edge intersects
		if (xcut > min(v1x, v2x) && xcut < max(v1x, v2x)){
			m = ((v1y - v2y) / (v1x - v2x)); 
			b = v1y - m *v1x; 
			intersectx = m*xcut + b;
			intersect.push_back(intersectx); //add the intersect point to the list
			intersectcell = true;
		}
	}

	//if no edge were intersected return 0
	if (!intersectcell) { return 0; }
	//otherwise get the length of the cut through the cell
	else{
		//Get the intersection points ordered from greatest to least
		sort(intersect.begin(), intersect.end()); //sorts the vector min to max
		reverse(intersect.begin(), intersect.end());
		h = 0; //the length of the cut through the cell
		//you must intersect the cell an even number of times (unless you managed to pass exactly through a vertex) 
		if (intersect.size() % 2 != 0){
			cout << "ERROR IN GetLengthHorzCut: Cut passed through vertex!" << endl;
			return 0;
		}
		//walking across the cut you enter and exit the cell every other intersect you incounter h=(int[0]-int[1])+(int[2]-int[3])+...
		for (i = 0; i < intersect.size() / 2; i++){
			h = h + (intersect[2 * i] - intersect[2 * i + 1]);
		}
		return h;
	}
}


