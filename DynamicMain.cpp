// The main function of the vertex model is found here
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <list>
#include <vector> 
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <time.h>
#include <unordered_map>
#include <chrono>
#include <random>
#include <ctime>
//#include <omp.h>
#include <typeinfo>
#include <cassert> 
#include <stddef.h>

//#include "CellClass.h" // SHOULD NOT NEED THIS -- only need to declare dependence on AGCell!
#include "AGCell.h"
//#include "EdgeClass.h"
#include "DynamicEdge.h"
#include "VertexClass.h"
//#include "TissueClass.h"
//#include "DynamicTissue.h"
//#include "DynamicTissueAG.h"
#include "DynamicTissue.h"
#include "Constants.h"
#include "DynamicConstants.h"
#include "AGConstants.h"

using namespace std;

//Defult settings of the constants if the file does not specify. Constants given a null or zero value must be specified in the file If they are not declared with a default value their will be a unresolved external error
string InputFileName = "NULL";
string INTEGRATORNAME = "RK4_Adaptive";
double LX = 0;
double LY = 0;
double MINAREA = 0;
double A0 = 1;
double MINLENGTH = 1;
double NEWEDGELENGTH = 5;
int SEED = 5;								//Note: setting seed to 0 or 1 gives unwanted behavior
double K = 5e-6;
double GAMMA = 10;

double EpsilonMax = 0.6; 		//!< Max edge tension before edges undergo a changes in rest length, originally value 0.27
double KAPPA = 0.23; 			//!< Remodeling rate constant for changes in edge rest length
double Y = 5.66;  //original number  /!< Spring constant for GAMMA = Y(l - lo) where l is the current length and lo is the rest length of the edge -> Y set for L0 85% of L and GAMMA0=1
// GAMMA0 was 1 for him
const double GAMMA0 = 1;		//!< GAMMA at the very beginning of the simulation (the const GAMMA the tissue was equilibrated with) 
								//!< should probably not be const

double INITIALSTEPSIZE = 0.001;
double TOTALTIME = 10.0;
double CURRENTTIME = 0.0;
double TIMEINTERVAL = 1.0;
double TISSUE_ALPHA = 1.0;
double alphaM = 1;
double TISSUE_EXT_STRESS = 0;
double TISSUE_EXT_STRESS_X = 0;
double TISSUE_EXT_STRESS_Y = 0;
double TISSUE_EXT_STRESS_X_MAX = 0;
double TISSUE_EXT_STRESS_Y_MAX = 0;
double error_tolerance_absolute_mechanical = 0.2;
double LXnew = 0;
double LYnew = 0;
double TAU = 1;
double TAU_STRESS = 5;
double A0SKEW = 1;
double LX0 = 0;
double LY0 = 0;
double T1PERCENT = 0;
int MITOSISANGLE = 0;
int TOTALT1 = 0;
double THETAAN = 0;
double ANSTRENGTH = 0;
double TCJSTRENGTH = 0;
bool MITOSISON = false;	
bool T1ON = true;	
bool APOPTOSISON = false; 
bool WRITEMITOSIS = false; 
bool VERBOSEON = true;
bool EQUILIBRIUMMITOSIS = false;
double NUCLEATIONTHRESHOLD = 1.4;
bool NUCLEATIONBASECRITERIA = 0;
//bool InEquilibrium = false; //This is used to determine if the tissue is equilibrated
double C4 = 0;
//This is the generator for random numbers to use call "distribution(generator)" 
mt19937_64 generator;
uniform_real_distribution<double> distribution(0.0, 1.0);
chrono::time_point<std::chrono::system_clock> starttime, endtime;

//Constants
Tissue_Dynamic tissue;

//*****************************************************************************************************
//********************         MAIN                    ************************************************
//*****************************************************************************************************

int main()
{	
	//Path to the folder that your initial files are in 
	string PathName =  "//Users//rafaelvolkamer-pastor//Library//CloudStorage//GoogleDrive-volkamerr@carleton.edu//My Drive//Summer Experiences//Michigan Biophysics REU//vertexModelSummer2026//vertexModelSummer2026//input_files//";

	cout << "Enter name of imput file: ";
	cin >> InputFileName;
	InputFileName = PathName + InputFileName;
	starttime = std::chrono::system_clock::now();

	//Read the input file
	bool readfile;
	cout << InputFileName << "\n";
	readfile = tissue.ReadInputData(InputFileName);

	if (!readfile){
		cout << "Ending because file was not readable" << endl;
		return 0;

	}
	//Seed the random number generator
	mt19937_64 generator(SEED);
	uniform_real_distribution<double> distribution(0.0, 1.0);


	//Set up the output files 
	string OutputFileName1, OutputFileName2;
	string filename = InputFileName.substr(0, InputFileName.size() - 4);
	OutputFileName1 = filename + "_out.txt";
	string OutputFileName3 = filename + "_error.txt";
	ofstream datafile, infofile, errorfile, digraphfile;
	datafile.open(OutputFileName1);
	errorfile.open(OutputFileName3);
	cout << "I made output files!" << endl;
	datafile << "write testing\n";
	//******************************INITIAlIZE***********************************************************************
	//**********************************************************************************************************
	Vect o(0, 0);
	for (list<Vertex*>::iterator it = tissue.vertices.begin(); it != tissue.vertices.end(); it++){
		Vertex* v = &(*(*it));
		(*v).SavedInfoForEvolve_MechForce = o; //Set the initial forces on vertices to 0
		// REST LENGTHS???
	}
	cout << "At INITIALSTEPSIZE" << endl;
	double stepSize = INITIALSTEPSIZE;
	//***********************************************************************************************
	//***************************EVOLVE**********************************************************
	//*********************************************************************************************
	cout << endl << "*******************************************************************************" << endl;
	cout << "Evolving with step size" << INITIALSTEPSIZE << endl;
	cout << "*******************************************************************************" << endl;

	tissue.WriteOutputDataAppend(&datafile);
	cout << "Tissue Guard" << endl;
	// above time variables allow us to pick how often we write to the appendfile
	while (CURRENTTIME < TOTALTIME) //run until totaltime
	{
		cout << "CURRENTTIME " << CURRENTTIME << endl;
		tissue.Evolve(TIMEINTERVAL, &stepSize, &errorfile);
		cout << "exited evolve" << "\n";
		tissue.WriteOutputDataAppend(&datafile);
		cout << "writing at CURRENTTIME: " << CURRENTTIME << "\n";
		cout << "Number of cells: " << tissue.cells.size() << endl;
		cout << "Equilibrated: " << tissue.IsEquilibrated() << endl;  //I don't know if I actually need this here anymore
	}

	tissue.WriteOutputDataAppend(&datafile);

	///////******************************End***********************************************************************
	///////**********************************************************************************************************
	endtime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = endtime - starttime;
	cout << "ELAPSED TIME: " << elapsed_seconds.count() / 60.0 << "  min\n";
	return 0;
}
