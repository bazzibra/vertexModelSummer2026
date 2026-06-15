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

#include "CellClass.h"
#include "EdgeClass.h"
#include "VertexClass.h"
#include "TissueClass.h"
#include "AGCell.h"
#include "AGTissue.h"
#include "Constants.h"
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
double INITIALSTEPSIZE = 0.001;
double TOTALTIME = 10.0;
double CURRENTTIME = 0.0;
double TIMEINTERVAL = 1.0;
double TISSUE_ALPHA = 500.0;
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
bool VERBOSEON = false;
bool EQUILIBRIUMMITOSIS = false;
double C4 = 0;
//This is the generator for random numbers to use call "distribution(generator)" 
mt19937_64 generator;
uniform_real_distribution<double> distribution(0.0, 1.0);
chrono::time_point<std::chrono::system_clock> starttime, endtime;

//Constants
AGTissue tissue;

//*****************************************************************************************************
//********************         MAIN                    ************************************************
//*****************************************************************************************************

//Uncomment the following lines to run the the code on the comand line as ./vertexmodel nameofimputfile
//int main(int argc, char* argv[]){
//	cout << "You made it into main congradulations!" << endl;
//	const char* temp = argv[1];
//	InputFileName = temp;
//	cout << "Reading argv[1] (name) it is:  " << InputFileName << endl;

int main()
{
	/*
	vector<Edge*> empty;
	AGCell* ag = new AGCell(0, empty, 0, 1, 0.5, PI / 4.0, 1,"tcj");
	cout << "test" << endl;
	//AGCell* ag2 = new AGCell(1, empty, 0, 1, PI / 4.0, 0, "combined");
	//AGCell * ag3 = new AGCell(2, empty, 0, 1, PI / 4.0, 0, "combined");
	for (int index = 0; index < 100000; index++)
	{
	//	cout << (*ag).DivisionAngleDistribution("combined") << "," << (*ag2).DivisionAngleDistribution("combined") << "," << (*ag3).DivisionAngleDistribution("combined") << endl;
		cout << (*ag).DivisionAngleDistribution("tcj") << endl;
	}

	delete ag;
	//delete ag2;
	// delete ag3;
	*/
	
	//Path to the folder that your initial files are in 
	// /Users/hnunley/Desktop/VertexModelCode_Avik/AGVertexModel_DynamicTensions_HSN/AGVertexModel_DynamicTensions
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
	}

	double stepSize = INITIALSTEPSIZE;
	//***********************************************************************************************
	//***************************EVOLVE**********************************************************
	//*********************************************************************************************
	cout << endl << "*******************************************************************************" << endl;
	cout << "Evolving with step size" << INITIALSTEPSIZE << endl;
	cout << "*******************************************************************************" << endl;

	tissue.WriteOutputDataAppend(&datafile);
	// above time variables allow us to pick how often we write to the appendfile
	while (CURRENTTIME < TOTALTIME)
	{
		cout << "CURRENTTIME " << CURRENTTIME << endl;
		tissue.Evolve(TIMEINTERVAL, &stepSize, &errorfile);
		cout << "exited evolve" << "\n";
		tissue.WriteOutputDataAppend(&datafile);
		cout << "writing at CURRENTTIME: " << CURRENTTIME << "\n";
		cout << "Number of cells: " << tissue.cells.size() << endl;
		cout << "Equilibrated: " << tissue.IsEquilibrated() << endl;

	}
	tissue.WriteOutputDataAppend(&datafile);

	///////******************************End***********************************************************************
	///////**********************************************************************************************************
	endtime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = endtime - starttime;
	cout << "ELAPSED TIME: " << elapsed_seconds.count() / 60.0 << "  min\n";
	return 0;
}

