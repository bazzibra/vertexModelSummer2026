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
#include <array>
#include <typeinfo>
#include <numeric>
#include <stddef.h>


#include "Constants.h"
#include "AGConstants.h"
//#include "TissueClass.h"
#include "AGCell.h"
#include "VertexClass.h"
#include "DynamicEdge.h"
#include "DynamicTissue.h"
#include "DynamicConstants.h"

/*
* void HelperReadInputData_SetGlobalValues(ifstream* datafile)
*
* Reads the data and sets all of the global variables.
* Starts at current position of *datafile and look for keyword 'Global', reads until keyword 'endGlobal'.
*/
void Tissue_Dynamic::HelperReadInputData_SetGlobalValues(ifstream* datafile) {

	string line, word, key, value;		//temporary variables to store lines and words 
	string type;						//what type of file is it (in case latter versions need to do slightly different things based on type)
	stringstream ssline;				//ssline is a string stream so we can comvert strings to stringstreams
	map <string, string> globals;



	//******ADD MORE GLOBAL VARIABLES HERE ALSO ADD THEM TO THE CONSTANTS.H FILE AND GIVE THEM A DEFAULT VALUE IN MAIN*********************//

	//Read until you get to the word Global signifying the begining of the global variables
	while (line.compare("Global:") != 0) {
		getline(*datafile, line);
		cout << line << endl;
	}

	//Now read the global variables and make a map with the keywords
	while (line.compare("endGlobal") != 0) {
		getline(*datafile, line);							//get the next line
		ssline.clear();
		ssline << line;										//put it into a stream
		ssline >> key;										//the first word is the key
		ssline >> value;									//the second is the value of the key
		if (value != "") {									//if the item actually has a value
			globals[key] = value;								//stick the item and it's value in the globals map
		}
	}

	//Now assign the constants their values
	//makes sure the key is actually part of the map before giving the constant a value if the key is not defined it will have the value given at the begining of main
	// the atof is there to convert the string to a double
	if (globals.count("LX") == 1) { LX = atof(globals["LX"].c_str()); }
	else if (globals.count("LX") == 0) { cout << "WARNING: LX is not given in input file, used default value " << LX << endl; }
	else { cout << "ERROR: Global variable LX has multiple definition in input file, used default value LX = " << LX << endl; }

	if (globals.count("LY") == 1) { LY = atof(globals["LY"].c_str()); }
	else if (globals.count("LY") == 0) { cout << "WARNING: LY is not given in input file, used default value " << LY << endl; }
	else { cout << "ERROR: Global variable LX has multiple definition in input file, used default value LY = " << LY << endl; }

	if (globals.count("MINAREA") == 1) { MINAREA = atof(globals["MINAREA"].c_str()); }
	else if (globals.count("MINAREA") == 0) { cout << "WARNING: MINAREA is not given in input file, used default value " << MINAREA << endl; }
	else { cout << "ERROR: Global variable MINAREA has multiple definition in input file, used default value MINAREA = " << MINAREA << endl; }

	if (globals.count("MINLENGTH") == 1) { MINLENGTH = atof(globals["MINLENGTH"].c_str()); }
	else if (globals.count("MINLENGTH") == 0) { cout << "WARNING: LX is not given in input file, used default value " << MINLENGTH << endl; }
	else { cout << "ERROR: Global variable MINLENGTH has multiple definition in input file, used default value MINLENGTH = " << MINLENGTH << endl; }

	if (globals.count("NEWEDGELENGTH") == 1) { NEWEDGELENGTH = atof(globals["NEWEDGELENGTH"].c_str()); }
	else if (globals.count("NEWEDGELENGTH") == 0) { cout << "WARNING: NEWEDGELENGTH is not given in input file, used default value " << NEWEDGELENGTH << endl; }
	else { cout << "ERROR: Global variable NEWEDGELENGTH has multiple definition in input file, used default value NEWEDGELENGTH = " << NEWEDGELENGTH << endl; }

	if (globals.count("GAMMA") == 1) { GAMMA = atof(globals["GAMMA"].c_str()); }
	else if (globals.count("GAMMA") == 0) { cout << "WARNING: GAMMA is not given in input file, used default value " << GAMMA << endl; }
	else { cout << "ERROR: Global variable GAMMA has multiple definition in input file, used default value GAMMA = " << GAMMA << endl; }

	if (globals.count("K") == 1) { K = atof(globals["K"].c_str()); }
	else if (globals.count("K") == 0) { cout << "WARNING: K is not given in input file, used default value " << K << endl; }
	else { cout << "ERROR: Global variable K has multiple definition in input file, used default value K = " << K << endl; }

	if (globals.count("SEED") == 1) { SEED = atof(globals["SEED"].c_str()); }
	else if (globals.count("SEED") == 0) { cout << "WARNING: SEED is not given in input file, used default value " << SEED << endl; }
	else { cout << "ERROR: Global variable SEED has multiple definition in input file, used default value SEED = " << SEED << endl; }

	if (globals.count("INITIALSTEPSIZE") == 1) { INITIALSTEPSIZE = atof(globals["INITIALSTEPSIZE"].c_str()); }
	else if (globals.count("INITIALSTEPSIZE") == 0) { cout << "WARNING: INITIALSTEPSIZE is not given in input file, used default value " << INITIALSTEPSIZE << endl; }
	else { cout << "ERROR: Global variable INITIALSTEPSIZE has multiple definition in input file, used default value INITIALSTEPSIZE = " << INITIALSTEPSIZE << endl; }

	if (globals.count("TIMEINTERVAL") == 1) { TIMEINTERVAL = atof(globals["TIMEINTERVAL"].c_str()); }
	else if (globals.count("TIMEINTERVAL") == 0) { cout << "WARNING: TIMEINTERVAL is not given in input file, used default value " << TIMEINTERVAL << endl; }
	else { cout << "ERROR: Global variable TIMEINTERVAL has multiple definition in input file, used default value TIMEINTERVAL = " << TIMEINTERVAL << endl; }

	if (globals.count("TOTALTIME") == 1) { TOTALTIME = atof(globals["TOTALTIME"].c_str()); }
	else if (globals.count("TOTALTIME") == 0) { cout << "WARNING: TOTALTIME is not given in input file, used default value " << TOTALTIME << endl; }
	else { cout << "ERROR: Global variable TOTALTIME has multiple definition in input file, used default value TOTALTIME = " << TOTALTIME << endl; }

	if (globals.count("CURRENTTIME") == 1) { CURRENTTIME = atof(globals["CURRENTTIME"].c_str()); }
	else if (globals.count("CURRENTTIME") == 0) { cout << "WARNING: CURRENTTIME is not given in input file, used default value " << CURRENTTIME << endl; }
	else { cout << "ERROR: Global variable CURRENTTIME has multiple definition in input file, used default value CURRENTTIME = " << CURRENTTIME << endl; }

	if (globals.count("error_tolerance_absolute_mechanical") == 1) { error_tolerance_absolute_mechanical = atof(globals["error_tolerance_absolute_mechanical"].c_str()); }
	else if (globals.count("error_tolerance_absolute_mechanical") == 0) { cout << "WARNING: error_tolerance_absolute_mechanical is not given in input file, used default value " << error_tolerance_absolute_mechanical << endl; }
	else { cout << "ERROR: Global variable error_tolerance_absolute_mechanical has multiple definition in input file, used default value error_tolerance_absolute_mechanical = " << error_tolerance_absolute_mechanical << endl; }

	if (globals.count("ALPHAM") == 1) { alphaM = atof(globals["ALPHAM"].c_str()); }
	else if (globals.count("ALPHAM") == 0) { cout << "WARNING: ALPHAM is not given in input file, used default value " << alphaM << endl; }
	else { cout << "ERROR: Global variable ALPHAM has multiple definition in input file, used default value ALPHAM = " << alphaM << endl; }

	if (globals.count("TISSUE_ALPHA") == 1) { TISSUE_ALPHA = atof(globals["TISSUE_ALPHA"].c_str()); }
	else if (globals.count("TISSUE_ALPHA") == 0) { cout << "WARNING: TISSUE_ALPHA is not given in input file, used default value " << TISSUE_ALPHA << endl; }
	else { cout << "ERROR: Global variable TISSUE_ALPHA has multiple definition in input file, used default value TISSUE_ALPHA = " << TISSUE_ALPHA << endl; }

	if (globals.count("TISSUE_EXT_STRESS") == 1) { TISSUE_EXT_STRESS = atof(globals["TISSUE_EXT_STRESS"].c_str()); }
	else if (globals.count("TISSUE_EXT_STRESS") == 0) { cout << "WARNING: TISSUE_EXT_STRESS is not given in input file, used default value " << TISSUE_EXT_STRESS << endl; }
	else { cout << "ERROR: Global variable TISSUE_EXT_STRESS has multiple definition in input file, used default value TISSUE_EXT_STRESS = " << TISSUE_EXT_STRESS << endl; }

	//if (globals.count("TISSUE_EXT_STRESS_X") == 1) { TISSUE_EXT_STRESS_X = atof(globals["TISSUE_EXT_STRESS_X"].c_str()); }
	//else if (globals.count("TISSUE_EXT_STRESS_X") == 0) { cout << "WARNING: TISSUE_EXT_STRESS_X is not given in input file, used default value " << TISSUE_EXT_STRESS_X << endl; }
	//else { cout << "ERROR: Global variable TISSUE_EXT_STRESS_X has multiple definition in input file, used default value TISSUE_EXT_STRESS_X = " << TISSUE_EXT_STRESS_X << endl; }

	//if (globals.count("TISSUE_EXT_STRESS_Y") == 1) { TISSUE_EXT_STRESS_Y = atof(globals["TISSUE_EXT_STRESS_Y"].c_str()); }
	//else if (globals.count("TISSUE_EXT_STRESS_Y") == 0) { cout << "WARNING: TISSUE_EXT_STRESS_Y is not given in input file, used default value " << TISSUE_EXT_STRESS_Y << endl; }
	//else { cout << "ERROR: Global variable TISSUE_EXT_STRESS_Y has multiple definition in input file, used default value TISSUE_EXT_STRESS_Y = " << TISSUE_EXT_STRESS_Y << endl; }

	cout << "WARNING: TISSUE_EXT_STRESS_X and TISSUE_EXT_STRESS_Y are overwritten, used default value TISSUE_EXT_STRESS_X = 0 and TISSUE_EXT_STRESS_Y = -ExternalStressRatio*EpsilonMax*Y*sqrt(Ncells)/LX" << endl;

	if (globals.count("MITOSISON") == 1) { MITOSISON = (atof(globals["MITOSISON"].c_str()) > 0); }
	else if (globals.count("MITOSISON") == 0) { cout << "WARNING: MITOSISON is not given in input file, used default value " << MITOSISON << endl; }
	else { cout << "ERROR: Global variable MITOSISON has multiple definition in input file, used default value MITOSISON = " << MITOSISON << endl; }

	if (globals.count("T1ON") == 1) { T1ON = (atof(globals["T1ON"].c_str()) > 0); }
	else if (globals.count("T1ON") == 0) { cout << "WARNING: T1ON is not given in input file, used default value " << T1ON << endl; }
	else { cout << "ERROR: Global variable T1ON has multiple definition in input file, used default value T1ON = " << T1ON << endl; }

	if (globals.count("APOPTOSISON") == 1) { APOPTOSISON = (atof(globals["APOPTOSISON"].c_str()) > 0); }
	else if (globals.count("APOPTOSISON") == 0) { cout << "WARNING: APOPTOSISON is not given in input file, used default value " << APOPTOSISON << endl; }
	else { cout << "ERROR: Global variable APOPTOSISON has multiple definition in input file, used default value APOPTOSISON = " << APOPTOSISON << endl; }

	if (globals.count("WRITEMITOSIS") == 1) { WRITEMITOSIS = atof(globals["WRITEMITOSIS"].c_str()); }
	else if (globals.count("WRITEMITOSIS") == 0) { cout << "WARNING: WRITEMITOSIS is not given in input file, used default value " << WRITEMITOSIS << endl; }
	else { cout << "ERROR: Global variable WRITEMITOSIS has multiple definition in input file, used default value WRITEMITSOSIS = " << WRITEMITOSIS << endl; }

	if (globals.count("VERBOSEON") == 1) { VERBOSEON = (atof(globals["VERBOSEON"].c_str()) > 0); }
	else if (globals.count("VERBOSEON") == 0) { cout << "WARNING: VERBOSEON is not given in input file, used default value " << VERBOSEON << endl; }
	else { cout << "ERROR: Global variable VERBOSEON has multiple definition in input file, used default value VERBOSEON = " << VERBOSEON << endl; }

	if (globals.count("EQUILIBRIUMMITOSIS") == 1) { EQUILIBRIUMMITOSIS = (atof(globals["EQUILIBRIUMMITOSIS"].c_str()) > 0); }
	else if (globals.count("EQUILIBRIUMMITOSIS") == 0) { cout << "WARNING: EQUILIBRIUMMITOSIS is not given in input file, used default value " << EQUILIBRIUMMITOSIS << endl; }
	else { cout << "ERROR: Global variable EQUILIBRIUMMITOSIS has multiple definition in input file, used default value EQUILIBRIUMMITOSIS = " << EQUILIBRIUMMITOSIS << endl; }

	if (globals.count("C4") == 1) { C4 = atof(globals["C4"].c_str()); }
	else if (globals.count("C4") == 0) { cout << "WARNING: C4 is not given in input file, used default value " << C4 << endl; }
	else { cout << "ERROR: Global variable C4 has multiple definition in input file, used default value C4 = " << C4 << endl; }

	if (globals.count("EpsilonMax") == 1) { EpsilonMax = atof(globals["EpsilonMax"].c_str()); }
	else if (globals.count("EpsilonMax") == 0) { cout << "WARNING: EpsilonMax is not given in input file, used default value " << EpsilonMax << endl; }
	else { cout << "ERROR: Global variable EpsilonMax has multiple definition in input file, used default value EpsilonMax = " << EpsilonMax << endl; }

	if (globals.count("KAPPA") == 1) { KAPPA = atof(globals["KAPPA"].c_str()); }
	else if (globals.count("KAPPA") == 0) { cout << "WARNING: KAPPA is not given in input file, used default value " << KAPPA << endl; }
	else { cout << "ERROR: Global variable KAPPA has multiple definition in input file, used default value KAPPA = " << KAPPA << endl; }

	if (globals.count("Y") == 1) { Y = atof(globals["Y"].c_str()); }
	else if (globals.count("Y") == 0) { cout << "WARNING: Y is not given in input file, used default value " << Y << endl; }
	else { cout << "ERROR: Global variable Y has multiple definition in input file, used default value Y = " << Y << endl; }

	if (globals.count("NUCLEATIONTHRESHOLD") == 1) { NUCLEATIONTHRESHOLD = atof(globals["NUCLEATIONTHRESHOLD"].c_str()); }
	else if (globals.count("NUCLEATIONTHRESHOLD") == 0) { cout << "WARNING: NUCLEATIONTHRESHOLD is not given in input file, used default value " << NUCLEATIONTHRESHOLD << endl; }
	else { cout << "ERROR: Global variable NUCLEATIONTHRESHOLD has multiple definition in input file, used default value NUCLEATIONTHRESHOLD = " << NUCLEATIONTHRESHOLD << endl; }

	if (globals.count("NUCLEATIONBASECRITERIA") == 1) { NUCLEATIONBASECRITERIA = atof(globals["NUCLEATIONBASECRITERIA"].c_str()); }
	else if (globals.count("NUCLEATIONBASECRITERIA") == 0) { cout << "WARNING: NUCLEATIONBASECRITERIA is not given in input file, used default value " << NUCLEATIONBASECRITERIA << endl; }
	else { cout << "ERROR: Global variable NUCLEATIONBASECRITERIA has multiple definition in input file, used default value NUCLEATIONBASECRITERIA = " << NUCLEATIONBASECRITERIA << endl; }

	if (globals.count("A0") == 1) { A0 = atof(globals["A0"].c_str()); }
	else if (globals.count("A0") == 0) { cout << "WARNING: A0 is not given in input file, used default value " << A0 << endl; }
	else { cout << "ERROR: Global variable A0 has multiple definition in input file, used default value A0 = " << A0 << endl; }

	if (globals.count("equilibriumtolerance") == 1) { equilibriumtolerance = atof(globals["equilibriumtolerance"].c_str()); }
	else if (globals.count("equilibriumtolerance") == 0) { cout << "WARNING: equilibriumtolerance is not given in input file, used default value " << equilibriumtolerance << endl; }
	else { cout << "ERROR: Global variable equilibriumtolerance has multiple definition in input file, used default value equilibriumtolerance = " << equilibriumtolerance << endl; }

	if (globals.count("ExternalStressRatio") == 1) { ExternalStressRatio = atof(globals["ExternalStressRatio"].c_str()); }
	else if (globals.count("ExternalStressRatio") == 0) { cout << "WARNING: ExternalStressRatio is not given in input file, used default value " << ExternalStressRatio << endl; }
	else { cout << "ERROR: Global variable ExternalStressRatio has multiple definition in input file, used default value ExternalStressRatio = " << ExternalStressRatio << endl; }

	//******ADD MORE GLOBAL VARIABLES HERE ALSO ADD THEM TO THE CONSTANTS.H FILE AND GIVE THEM A DEFAULT VALUE IN MAIN*********************//


	// setting set the initial box dimensions here
	LX0 = LX;
	LY0 = LY;

}


/*
* void HelperReadInputData_CreateObjects
*
* Make all of the objects with the correct starting positions and flags and recordds their ids in the tissue's
* list of ids.
* Starts at current position of *datafile and reads until keyword 'End'.
*/
void Tissue_Dynamic::HelperReadInputData_CreateObjects(ifstream* datafile) {

	string line, key, value, edg;			//temporary variables to store lines and words 
	string type;							//what type of file is it (in case latter versions need to do slightly different things based on type)
	stringstream ssline, ssedges;			//ssline is a string stream so we can comvert strings to stringstreams
	vector<string> keywords;				//vector of strings to hold the keywords
	vector<int> listedges;
	int i, j;
	map <string, string> objectInfo;		//map to hold the keywords and their data
	int newid, v1, v2, enumb, typenumb;		//temp varables to store the object info
	double x, y;
	bool xf, yf;
	double A0, L0;
	string potential, cellpressuretype;
	double tcjstrength = 0;
	double thetaan = 0;
	double anstrength = 0;


	cout << "Making a base class tissue" << endl;

	//VERTICES
	//read the file untill you get to the word vertices
	while (line.compare("Vertices") != 0) {
		getline(*datafile, line);
	}
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')) {
		keywords.push_back(key);
	}
	//now recorde the data and make the vertex object untill you get to the word edges signifying the end of the data
	while (true) {
		getline(*datafile, line);							//get the next line
		if ((line.compare("Edges") == 0)) { break; }
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')) {				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//make the object and as it to the linked list of vertices also record it's id in the master id list
		newid = atoi(objectInfo["id"].c_str());						//extract the data from the map
		x = atof(objectInfo["x"].c_str());
		y = atof(objectInfo["y"].c_str());
		Vertex* vptr = new Vertex;									// Create a vertes with an id and x,y pos and put it in the master list of all veritces
		(*vptr).id = newid;
		(*vptr).r.x = x;
		(*vptr).r.y = y;
		(vertices).push_back(vptr);
		vertexIDs.push_back(newid);								//and add its id to the list of ids
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*vertices.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}

	//EDGES
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')) {
		keywords.push_back(key);
	}
	//now recorde the data and make the edge object untill you get to the word cells signifying the end of the data
	while (true) {
		getline(*datafile, line);							//get the next line
		if (line.compare("Cells") == 0) { break; }
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')) {				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//make the object and as it to the linked list of vertices also record it's id in the master id list
		newid = atoi(objectInfo["id"].c_str());						//extract the data from the map
		v1 = atoi(objectInfo["vertex1"].c_str());
		v2 = atoi(objectInfo["vertex2"].c_str());
		xf = atoi(objectInfo["xflag"].c_str());
		yf = atoi(objectInfo["yflag"].c_str());
		L0 = atof(objectInfo["length"].c_str());
		Edge_Dynamic* eptr = new Edge_Dynamic;
		(*eptr).id = newid;
		(*eptr).IOVertexID[0] = v1;
		(*eptr).IOVertexID[1] = v2;
		(*eptr).xflag = xf;
		(*eptr).yflag = yf;
		(*eptr).cell1 = NULL;
		(*eptr).cell2 = NULL;
		(*eptr).L0 = L0 / ((GAMMA0/Y)+1);
		(*eptr).currentRestLength = (*eptr).L0;
		(*eptr).restLengthOld = (*eptr).L0;
		(*eptr).restLengthNew = (*eptr).L0;
		//cout << "L0 VAL " << (*eptr).L0 << endl;
		//(*eptr).L0 = L0; //option if you're feeding in rest lengths instead of current edge lengths
		(edges).push_back(eptr);		// Create a vertes with an id and x,y pos and put it in the master list of all veritces
		edgeIDs.push_back(newid);								//and add its id to the list of ids
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*edges.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}

	//AGCELLS
	//then get the keywords on the next line
	keywords.clear();
	getline(*datafile, line);
	ssline.clear();
	ssline << line;
	while (getline(ssline, key, '\t')) {
		keywords.push_back(key);
	}
	//now recorde the data and make the cell object untill you get to the word end signifying the end of the data
	while (true) {
		getline(*datafile, line);							//get the next line
		if (line.compare("End") == 0 || line.compare("Fibers") == 0) { break; }				//break if keyworkd 'End' is found
		ssline.clear();
		ssline << line;
		objectInfo.clear();
		i = 0;
		while (getline(ssline, value, '\t')) {				//parse it and put the data into the objectInfo map	
			objectInfo[keywords[i]] = value;
			i++;
		}
		//extract the data from the map
		newid = atoi(objectInfo["id"].c_str());
		enumb = atoi(objectInfo["EdgeNumb"].c_str());
		if (objectInfo.count("Type")) {
			typenumb = atoi(objectInfo["Type"].c_str());
		}
		else { typenumb = 0; }
		if (objectInfo.count("A0")) {
			A0 = atof(objectInfo["A0"].c_str());
		}
		// you only want atoi if you have a number you want to store as a string. but we should probably have a string


		ssedges.clear();
		ssedges << objectInfo["EdgeList"];
		//now worry about the edge list
		listedges.clear();
		while (getline(ssedges, edg, ',')) {
			listedges.push_back(atoi(edg.c_str()));
		}

		if (objectInfo.count("Potential"))
		{
			potential = objectInfo["Potential"];
		}
		else
		{
			potential = "uniform";
		}

		if (objectInfo.count("TCJStrength"))
		{
			tcjstrength = atof(objectInfo["TCJStrength"].c_str());
		}

		if (objectInfo.count("ThetaAn"))
		{
			thetaan = atof(objectInfo["ThetaAn"].c_str());
		}

		if (objectInfo.count("AnStrength"))
		{
			anstrength = atof(objectInfo["AnStrength"].c_str());
		}
		if (objectInfo.count("CellPressureType"))
		{
			cellpressuretype = objectInfo["CellPressureType"];
		}
		else
		{
			cellpressuretype = "QUADRATIC";
		}

		//make the cell object
		AGCell* cptr = new AGCell;
		(*cptr).id = newid;
		(*cptr).type = typenumb;
		(*cptr).A0 = A0;
		(*cptr).IOEdgeID = listedges;
		(*cptr).DIVISIONPOTENTIAL = potential;
		(*cptr).pressureType = cellpressuretype;

		if (tcjstrength != 0)
		{
			(*cptr).TCJSTRENGTH = tcjstrength;
			// if this has been included in the input file, assign the input file values
		}
		else
		{
			(*cptr).TCJSTRENGTH = TCJSTRENGTH;
		}

		if (anstrength != 0 && thetaan != 0)
		{
			(*cptr).THETAAN = thetaan;
			(*cptr).ANSTRENGTH = anstrength;
			// if these have been included in the input file, then assign input file values 
		}
		else
		{
			(*cptr).THETAAN = THETAAN;
			(*cptr).ANSTRENGTH = ANSTRENGTH;
			// otherwise, use the default values assigned in the Main function (which have been included as failsafe)
		}


		cells.push_back(cptr);
		cellIDs.push_back(newid);
		//******************HOW TO ADD A NEW DATA MEMBER ***********************//
		//NEWDATA = atof( objectInfo["NEWDATA"].c_str() );
		//(*cells.back()).NEWDATANAME = NEWDATA
		//*************************************************************************//
	}
}


/*
* void HelperWriteOutputData_Write2File
*
* Writes data into file specified by 'datafile'. If flag 'includeExtraInfo' is true, then additional information (such as tension, cell area, etc) is included.
*The extra information is non-essential in reconstructing the simulation state, and is just provided for  convenience (e.g. for display in MATLAB).
* (should be helpful for incremental coding erro debug sessions)
*
*/

void Tissue_Dynamic::HelperWriteOutputData_Write2File(ofstream* datafile, bool includeExtraInfo) {
	time_t rawtime;					//to deal with writing the date out
	struct tm* timeinfo;
	*datafile << "INTEGRATORNAME\t" << INTEGRATORNAME << endl;
	//Now write the global variables you used
	*datafile << "CURRENTTIME\t" << CURRENTTIME << endl;
	*datafile << "Global:" << endl;
	*datafile << "LX\t" << LX << endl;
	*datafile << "LY\t" << LY << endl;
	*datafile << "A0\t" << A0 << endl;
	*datafile << "SEED\t" << SEED << endl;
	*datafile << "TIMEINTERVAL\t" << TIMEINTERVAL << endl;
	*datafile << "MINAREA\t" << MINAREA << endl;
	*datafile << "MINLENGTH\t" << MINLENGTH << endl;
	*datafile << "NEWEDGELENGTH\t" << NEWEDGELENGTH << endl;
	*datafile << "K\t" << K << endl;
	*datafile << "GAMMA\t" << GAMMA << endl;
	*datafile << "TOTALTIME\t" << TOTALTIME << endl;
	*datafile << "VERBOSEON\t" << VERBOSEON << endl;
	*datafile << "INITIALSTEPSIZE\t" << INITIALSTEPSIZE << endl;
	*datafile << "error_tolerance_absolute_mechanical\t" << error_tolerance_absolute_mechanical << endl;
	*datafile << "TISSUE_ALPHA\t" << TISSUE_ALPHA << endl;
	*datafile << "ALPHAM\t" << alphaM << endl;
	*datafile << "TISSUE_EXT_STRESS_X\t" << TISSUE_EXT_STRESS_X << endl;
	*datafile << "TISSUE_EXT_STRESS_Y\t" << TISSUE_EXT_STRESS_Y << endl;
	*datafile << "equilibriumtolerance\t" << equilibriumtolerance << endl;
	*datafile << "NUCLEATIONTHRESHOLD\t" << NUCLEATIONTHRESHOLD << endl;
	*datafile << "NUCLEATIONBASECRITERIA\t" << NUCLEATIONBASECRITERIA << endl;
	*datafile << "PuLLINGDIR_X\t" << PULLINGDIR_X << endl;
	*datafile << "PuLLINGDIR_Y\t" << PULLINGDIR_Y << endl;
	*datafile << "C4\t" << C4 << endl;
	*datafile << "NUMBER_OF_CELLS\t" << cells.size() << endl;
	*datafile << "EpsilonMax\t" << EpsilonMax << endl;
	*datafile << "KAPPA\t" << KAPPA << endl;
	*datafile << "GAMMA0\t" << GAMMA0 << endl;
	*datafile << "Y\t" << Y << endl;
	*datafile << "ExternalStressRatio\t" << ExternalStressRatio << endl;

	//******ADD NEW OUTPUT HERE******//
	*datafile << "endGlobal" << endl;
	if (!includeExtraInfo)											// if flag is false, include only essential information to capture the state of the simulation
	{
		//print the vertex info
		/*
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty" << endl;
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {	//loop through all of the vertices in the tissue
			*datafile << (*(*vit)).id << "\t" << (*(*vit)).r.x << "\t" << (*(*vit)).r.y << endl;						//write it's info to the file
		}
		*/

		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty\tAnglestheta1\tAnglestheta2\tAnglestheta3\tPsiAngle1\tPsiAngle2\tPsiAngle3\tNucleation1\tNucleation2\tNucleation3" << endl;

		for (Vertex* v : vertices) {
			v->ComputeVertexAngles();

			*datafile << v->id << "\t"
				<< v->r.x << "\t"
				<< v->r.y << "\t";
				//<< v->EdgesNumber << "\t";

			// Always print exactly 3 thetas
			for (int i = 0; i < 3; i++) {
				if (i < v->theta.size())
					*datafile << v->theta[i];
				else
					*datafile << 0;   // or NaN
				*datafile << "\t";
			}

			// Always print exactly 3 psis
			for (int i = 0; i < 3; i++) {
				if (i < v->psi.size())
					*datafile << v->psi[i];
				else
					*datafile << 0;   // or NaN
				if (i < 2) *datafile << "\t";
			}

			*datafile << "\t";

			// Always print exactly 3 nucleation
			for (int i = 0; i < 3; i++) {
				if (i < v->nucleation.size())
					*datafile << v->nucleation[i];
				else
					*datafile << 0;   // or NaN
				if (i < 2) *datafile << "\t";
			}

			*datafile << endl;
		}


		//print the edge info
		*datafile << "Edges" << endl;
		*datafile << "id\tvertex1\tvertex2\txflag\tyflag" << endl;
		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++) {			//loop through all of the edges in the tissue
			Vertex* v1 = (*(*eit)).vertex1;
			Vertex* v2 = (*(*eit)).vertex2;
			*datafile << (*(*eit)).id << "\t" << (*v1).id << "\t" << (*v2).id << "\t" << (*(*eit)).xflag << "\t" << (*(*eit)).yflag << endl;			//write its info to the file (normal version)
		}
		//print the cell info
		int i;
		*datafile << "Cells" << endl;
		*datafile << "id\tA0\tEdgeNumb\tEdgeList\tType" << endl;
		for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++) {			//loop through all of the cells in the tissue
			stringstream ssEdgeList;
			ssEdgeList.clear();
			for (i = 0; i < (*(*cit)).edges.size(); i++) {				//make comma-separated list of edges
				ssEdgeList << (*(*(*cit)).edges[i]).id << ",";
			}

			*datafile << (*(*cit)).id << "\t" << (*(*cit)).A0 << "\t" << (*(*cit)).edges.size() << "\t" << ssEdgeList.str() << "\t" << (*(*cit)).type << endl;
		}
	}

	else															// if flag is true, include only essential information to capture the state of the simulation
	{
		//print the vertex info with additional information
		/*
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty\t_EdgesNumber" << endl;
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {			//loop through all of the vertices in the tissue
			*datafile << (*(*vit)).id << "\t" << (*(*vit)).r.x << "\t" << (*(*vit)).r.y << "\t" << (*(*vit)).edges.size() << endl;						//write it's info to the file
		}
		*/

		/*
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty\tAnglestheta1\tAnglestheta2\tAnglestheta3\tPsiAngle1\tPsiAngle2\tPsiAngle3" << endl;

		for (Vertex* v : vertices) {
			v->ComputeVertexAngles();

			*datafile << v->id << "\t"
				<< v->r.x << "\t"
				<< v->r.y << "\t";
#//<< v->EdgesNumber << "\t";

			// Always print exactly 3 thetas
			for (int i = 0; i < 3; i++) {
				if (i < v->theta.size())
					*datafile << v->theta[i];
				else
					*datafile << 0;   // or NaN
				*datafile << "\t";
			}

			// Always print exactly 3 psis
			for (int i = 0; i < 3; i++) {
				if (i < v->psi.size())
					*datafile << v->psi[i];
				else
					*datafile << 0;   // or NaN
				if (i < 2) *datafile << "\t";
			}

			*datafile << endl;
		}
		*/
		//Updates with the smaller tension of the best pair of edges for 3- way vertex 
		*datafile << "Vertices" << endl;
		*datafile << "id\tx\ty\tAnglestheta1\tAnglestheta2\tAnglestheta3\t"
			<< "PsiAngle1\tPsiAngle2\tPsiAngle3\t"
			<< "Nucleation1\tNucleation2\tNucleation3"
			// << "\tPairMin12\tPairMin13\tPairMin23\tWeakestPairMin"
			<< endl;

		for (Vertex* v : vertices) {

			v->ComputeVertexAngles();

			// --- keep variables if you may reuse later ---
			/*
			double Tmin12 = 0.0;
			double Tmin13 = 0.0;
			double Tmin23 = 0.0;
			double weakestPairMin = 0.0;

			if (v->edges.size() == 3) {

				Edge* e1 = v->edges[0];
				Edge* e2 = v->edges[1];
				Edge* e3 = v->edges[2];

				double T1 = e1->GetTension_Magnitude();
				double T2 = e2->GetTension_Magnitude();
				double T3 = e3->GetTension_Magnitude();

				Tmin12 = std::min(T1, T2);
				Tmin13 = std::min(T1, T3);
				Tmin23 = std::min(T2, T3);

				weakestPairMin = std::min(Tmin12, std::min(Tmin13, Tmin23));
			}
			*/

			*datafile << v->id << "\t"
				<< v->r.x << "\t"
				<< v->r.y << "\t";

			// --- theta values ---
			for (int i = 0; i < 3; i++) {
				if (i < v->theta.size())
					*datafile << v->theta[i];
				else
					*datafile << 0;
				*datafile << "\t";
			}

			// --- psi values ---
			for (int i = 0; i < 3; i++) {
				if (i < v->psi.size())
					*datafile << v->psi[i];
				else
					*datafile << 0;

				if (i < 2) *datafile << "\t";   // avoid trailing tab
			}
			
			*datafile << "\t";
			
			// Always print exactly 3 nucleation
			for (int i = 0; i < 3; i++) {
				if (i < v->nucleation.size())
					*datafile << v->nucleation[i];
				else
					*datafile << -1;

				if (i < 2) *datafile << "\t";   // avoid trailing tab
			}

			// --- tension outputs removed ---
			/*
			*datafile << "\t"
					  << Tmin12 << "\t"
					  << Tmin13 << "\t"
					  << Tmin23 << "\t"
					  << weakestPairMin;
			*/

			*datafile << endl;
		}







		//print the edge info
		*datafile << "Edges" << endl;
		//*datafile << "id\tvertex1\tvertex2\txflag\tyflag\ttension\tlength\t_cell1\t_cell2" << endl; //version w/ edge lengths (MATLAB CreateMovie can't run this version)
		*datafile << "id\tvertex1\tvertex2\txflag\tyflag\ttension\tlength\trestlength\t_cell1\t_cell2" << endl;  //version w/o edge lengths (MATLAb CreateMovie can run this version)
		for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++) {			//loop through all of the edges in the tissue
			if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*eit)) {
				Vertex* v1 = (*(*eit)).vertex1;
				Vertex* v2 = (*(*eit)).vertex2;
				//*datafile << (*(*eit)).id << "\t" << (*v1).id << "\t" << (*v2).id << "\t" << (*(*eit)).xflag << "\t" << (*(*eit)).yflag << "\t" << (*(*eit)).GetTension_Magnitude() << "\t" << (*ed).GetRestLength() << "\t" << (*(*(*eit)).cell1).id << "\t" << (*(*(*eit)).cell2).id << endl; // version w/ edge length
				*datafile << (*(*eit)).id << "\t" << (*v1).id << "\t" << (*v2).id << "\t" << (*(*eit)).xflag << "\t" << (*(*eit)).yflag << "\t" << (*(ed)).GetTension_Magnitude() << "\t" << (*(ed)).GetLength() << "\t" << (*(ed)).GetRestLength() << "\t" << (*(*(*eit)).cell1).id << "\t" << (*(*(*eit)).cell2).id << endl; //version w/o edge lengths
			}
		}


		//print the cell info
		cout << "Print cell info" << endl;
		int i;
		*datafile << "Cells" << endl;

		*datafile << "id\tA0\tEdgeNumb\tType\txflag\tyflag\tArea\tPressure\tPerimeter\tCenterx\tCentery\tEdgeList\tCWVertices\tdEnergy" << endl;
		for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++)
		{			//loop through all of the cells in the tissue
			stringstream ssEdgeList;
			ssEdgeList.clear();
			for (i = 0; i < (*(*cit)).edges.size(); i++)
			{				//make comma-separated list of edges
				ssEdgeList << (*(*(*cit)).edges[i]).id << ",";
			}

			vector<Vertex*> CWVertices = (*(*cit)).GetClockWiseVertices();
			stringstream ssCWVertices;
			ssCWVertices.clear();
			for (i = 0; i < CWVertices.size(); i++)
			{			//make comma-separated list of vertices ordered clockwise
				ssCWVertices << (*CWVertices[i]).id << ",";
			}
			Vect centroid = (*(*cit)).GetCenter();
			AGCell* agit = dynamic_cast<AGCell*>(*cit);
			*datafile << (*(*cit)).id << "\t" << (*agit).A0 << "\t" << (*(*cit)).edges.size() << "\t" << (*(*cit)).type << "\t" << (*(*cit)).GetXFlag() << "\t" << (*(*cit)).GetYFlag() << "\t"
				<< (*agit).GetArea() << "\t" << (*agit).GetPerimeter() << "\t" << (*agit).GetPressure((*(*cit)).GetArea()) << "\t" << centroid.x << "\t" << centroid.y << "\t" << ssEdgeList.str() << "\t" << ssCWVertices.str() << "\t" << (*agit).pressureType << endl;
		}
	}

	*datafile << "End" << endl << endl;
}

//////////////////////////////////////////////////////////////////////////
// Evolves the tissue one time interval and updates the edge lengths and vertex positions
void Tissue_Dynamic::Evolve(double interval, double* stepsize, ofstream* errorfile) {
	//find the end point of this evolve
	double endtime = CURRENTTIME + interval;
	bool reinitialize = true;
	bool flag0, flag1, flag2, flag3, flag4;
	int count = 0;

	while (CURRENTTIME < endtime) {
		count++;
		//STEP 1: If the topology has changed get a list of the clockwise vertices in each cell
		if (reinitialize) {																	//If their has been any topological change OR vertices have crossed a boundary 
			for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++) {		//Loop through the cells 
				(*(*cit)).SavedInfoForEvolve_CWVerts = (*(*cit)).GetClockWiseVertices();	//and get the vertices in the correct clockwise order 
				reinitialize = false;
			}
		}

		//STEP 2: Integrate 
		//Loop through the vertices and record the current position in rold 
		for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) { (*(*vit)).rold = (*(*vit)).r; }

		//Loop through the edges and record the current edge length in lengthOld 
		for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
			if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
				(*ed).restLengthOld = (*ed).currentRestLength;
			}
		}
		LXold = LX;				//Record the old box size
		LYold = LY;

		//******CHOOSE YOUR INTEGRATOR******
		if (INTEGRATORNAME == "RK4_Adaptive") {
			//4th order Runge-Kutta with adaptive stepsize
			(*this).Integrate_RK4_Adaptive(stepsize, error_tolerance_absolute_mechanical, &CURRENTTIME, endtime, errorfile); //Evolve the vertices and box size dimensions with the given error tolerance (adaptive step size). time is incremented within this function.
		}
		else if (INTEGRATORNAME == "RK4") {
			//4th order Runge-Kutta with fized stepsize
			(*this).Integrate_RK4(*stepsize, errorfile);
			CURRENTTIME += *stepsize;	//Update the current time
		}
		else if (INTEGRATORNAME == "Euiler") {
			//Simple Euiler with Fixed stepsize
			(*this).Integrate_Euiler(*stepsize, errorfile);
			CURRENTTIME += *stepsize;	//Update the current time
		}
		else { cout << "ERROR invalid integrator name!"; }

		//Step 3: Update
		LX = LXnew;				//Update the current box size 
		LY = LYnew;
		//HelperEvolve has the edge update part commented out right now
		(*this).HelperEvolve_UpdatePosition();  //Update the vertex positions (this automaticaly updates edge flags if a vertex has crossed the boundary)

		//STEP 4: look for topological changes and set flags if any have occured 
		flag1 = (*this).Apoptosis(errorfile);
		if (flag1){
			cout << "APOPTOSIS!";
			(*this).WriteOutputDataAppend(errorfile);
		}
		flag2 = (*this).T1(errorfile);
		//if (flag2){
		//	(*this).WriteOutputDataAppend(errorfile);
		//}
		flag3 = (*this).ResolveVertices(errorfile);
		if (flag3) {
        	throw std::runtime_error("Error: calling base class version of ResolveVertices.");
			(*this).WriteOutputDataAppend(errorfile);
    	}


		// STEP 5: Update Free Energy (after topological changes)
		pastFreeEnergy = currentFreeEnergy;
		currentFreeEnergy = (*this).CalculateFreeEnergy();

		if (flag1 || flag2 || flag3 /*|| flag4*/) reinitialize = true;  //Determine if we need to reinitialize
	}

	cout << "TISSUE EVOLVED TO TIME " << CURRENTTIME << " in " << count << " steps" << endl;
	cout << "LX = " << LX << "  LY = " << LY << endl;
}

//Integrates the forcefunction by one euiler step 
// removed the dt? SavedInfoForEvolve_TotalForce instead of MechForce
void Tissue_Dynamic::Integrate_Euiler(double dt, ofstream* errorfile) {

	//Calculate the force on every vertex 
	(*this).ForceFunction(errorfile);

	//loop through all the vertices and set rnew
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_TotalForce * (1 / alphaM) + (*(*vit)).rold;
	}
	// CHECK!!

	//Calculate any changes in rest length for every edge
	(*this).LengthFunction(errorfile);

	//loop through all the edges and set lengthNew
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			//save the old rest length, save the rate, and incorporate the old rest length with the rate of change * dt 
			(*ed).restLengthNew = (*ed).GetRestLength() + (*ed).SavedInfoForEvolve_RestLength * dt; // there is a dt here
		}
	}
	//Set LX and LY new
	LXnew = (1 / TISSUE_ALPHA) * ForceLX + LXold;
	LYnew = (1 / TISSUE_ALPHA) * ForceLY + LYold;
}

///Decides whether or not an edge rest length should be altered  
void Tissue_Dynamic::LengthFunction(ofstream* errorfile) {
	double epsilon;	//Variable for the strain on an edge

	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		double deltaRestLength = 0;
		double currentLength = (*(*dit)).GetLength();
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			double currentRestLength = (*ed).GetRestLength();

			epsilon = (currentLength - currentRestLength) / currentRestLength; //calculate the strain on an edge based on current length and current rest length

			if (epsilon > EpsilonMax || epsilon < -EpsilonMax) { //Nov 5, 2019, Banerjee eqn
				deltaRestLength = (currentRestLength * KAPPA * epsilon);
			}

			//if (epsilon > EpsilonMax) {
			//	deltaRestLength = (currentRestLength * KAPPA * epsilon);
			//}
			//else if (epsilon <= 0.5*EpsilonMax) {
			//	deltaRestLength = (currentRestLength * -35 * epsilon);
			//}

			(*ed).SavedInfoForEvolve_RestLength = deltaRestLength; // no dt

		}
	}

}


//! Adaptive evolver for mechanical variables (enforcing given error tolerances). Only evolve to time t1.
int Tissue_Dynamic::Integrate_RK4_Adaptive(double* step_size, const double error_abs_tolerance, double* t, double t1, ofstream* errorfile) {
	//dimension of mechanical systems
	int dimension_mechanical_v = vertices.size() + 1;
	int dimension_mechanical_e = edges.size();
	//Store the current step size (make sure the total scale factor is not out of range).
	double current_step_size = *step_size;
	//Define the size of a half step.
	double half_step = current_step_size / 2.;
	//Initialize the error check for entering while loop (step_error_check -> step needs to be scaled down).
	bool step_error_check = true;
	//These vectors are used for both half and full steps below as well as error tolerance and estimated error.

	vector<Vect> initial_condition_v;
	vector<double> initial_condition_e;
	vector<Vect> y_error_tolerance;
	vector<Vect> error_vector_v;
	vector<double> error_vector_e;
	vector<Vect> full_step_solution_v;
	vector<double> full_step_solution_e;
	vector<Vect> second_half_step_solution_v;
	vector<double> second_half_step_solution_e;
	vector<Vect> first_half_step_solution_v;
	vector<double> first_half_step_solution_e;

	initial_condition_v.resize(dimension_mechanical_v);
	initial_condition_e.resize(dimension_mechanical_e);

	full_step_solution_v.resize(dimension_mechanical_v);
	full_step_solution_e.resize(dimension_mechanical_e);

	second_half_step_solution_v.resize(dimension_mechanical_v);
	second_half_step_solution_e.resize(dimension_mechanical_e);

	first_half_step_solution_v.resize(dimension_mechanical_v);
	first_half_step_solution_e.resize(dimension_mechanical_e);

	// IS THIS RIGHT?
	y_error_tolerance.resize(dimension_mechanical_v + dimension_mechanical_e); //combined to account for all vertices and edges 
	// IS THIS RIGHT?

	error_vector_v.resize(dimension_mechanical_v);



	//Store initial condition.
	(*this).Store_Initial_Condition(initial_condition_v, initial_condition_e);

	//The consistency order affects how the step size is scaled with observed error.
	double consistency_order = 4.0;

	while (step_error_check == true) {
		//if the step will take you outside of the specified time, scale the stepsize down.
		if (*t + current_step_size > t1) {
			current_step_size = t1 - *t;
		}

		//Reinitialize half_step_size (upon entering loop)
		half_step = current_step_size / 2.0;

		//Take the first half step.
		(*this).Integrate_RK4(half_step, errorfile);
		//Store the solution from the first half step.
		(*this).Store_New_Positions(first_half_step_solution_v, first_half_step_solution_e);

		//Make the first half step solution the initial condition for the second half step.
		(*this).Update_Old_Mechanical_Variables(first_half_step_solution_v, first_half_step_solution_e);

		//Take the second half step.
		(*this).Integrate_RK4(half_step, errorfile);
		//Store the solution from the second half step.
		(*this).Store_New_Positions(second_half_step_solution_v, second_half_step_solution_e);

		//restore initial conditions:
		(*this).Update_Old_Mechanical_Variables(initial_condition_v, initial_condition_e);

		//take a full step
		(*this).Integrate_RK4(current_step_size, errorfile);
		//store the full step solution.
		(*this).Store_New_Positions(full_step_solution_v, full_step_solution_e);

		//restore initial conditions:
		(*this).Update_Old_Mechanical_Variables(initial_condition_v, initial_condition_e);

		//Estimate errors by subtracting two half steps from full step.
		error_vector_v.resize(dimension_mechanical_v);
		for (int index = 0; index != dimension_mechanical_v; index++) {
			error_vector_v.at(index) = full_step_solution_v.at(index) - second_half_step_solution_v.at(index);
			error_vector_v.at(index) = error_vector_v.at(index).Abs();
		}
		//Do the same for edges.
		error_vector_e.resize(dimension_mechanical_e);
		for (int count = 0; count != dimension_mechanical_e; count++) {
			error_vector_e.at(count) = std::abs(full_step_solution_e.at(count) - second_half_step_solution_e.at(count));
		}

		//Check whether step size is too large. (if it is too large, this function will return step_error_check != 0)
		step_error_check = Double_Step_Size_Control(error_vector_v, error_vector_e, error_abs_tolerance, &current_step_size, consistency_order);

		//if the step size was scaled down, reinitialize initial_step_size for reentering while loop.
		if (step_error_check == true) {
			*step_size = current_step_size;
		}
	}

	//Current step size may have been scaled up by the adaptive step function; actual step used is 2 * half_step.
	double used_step_size = 2 * half_step;

	//Increment the time by the utilized step size (once the step is successful).
	if (step_error_check == 0) {
		//It is important to increment the time by two half steps (because the variable current_step_size may have been scaled up).
		*t += used_step_size;
	}

	//Store the current_step_size in step size if it exceeds the step size.
	if (current_step_size > *step_size) *step_size = current_step_size;

	//Return success code.
	return step_error_check;
}

//! Build arrays of edge length and vertex position values 
void Tissue_Dynamic::Store_Initial_Condition(vector<Vect>& y, vector<double>& q) { //y for verts and q for edges
	//Loop through the current lengths and store them in an array.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		y.at(i) = (*(*vit)).rold;
		i++;
	}
	//Make the last elements the box size dimensions.
	//Check that this vect is doing what you want.
	y.at(i) = Vect(LXold, LYold);		//set last to elements of y[] to current box size

	int j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			q.at(j) = (*ed).restLengthOld;
			j++;
		}
	} 
}

//! Store the current vertex positions and current edge lengths 
void Tissue_Dynamic::Store_New_Positions(vector<Vect>& y, vector<double>& q) {
	//Loop through the current positions and store them in an array.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		y.at(i) = (*(*vit)).rnew;
		i++;
	}
	//Make the last elements the box size dimensions.
	//Check that this vect is doing what you want.
	y.at(i) = Vect(LXnew, LYnew);						//set last to elements of y[] to current box size

	//Loop through the current lengths and store them in an array.
	int j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			q.at(j) = (*ed).restLengthNew;
			j++;
		}
	}
}


//! RK4 adaptive integrator over both edge lengths and vertex positions
void Tissue_Dynamic::Integrate_RK4(double dt, ofstream* errorfile) {
	//We will take advantage of the fact that the edge list does not change (in size or order) during integration to store a list of the intermediate lengths of the edges 
	vector<Vect> k1, k2, k3, k4; //for forces on vertices
	vector<double> c1, c2, c3, c4; //for edge rest lengths
	double k1LX, k1LY, k2LX, k2LY, k3LX, k3LY, k4LX, k4LY;
	int i; //for indexing vertices
	int j; //for indexing edges

	//Make sure that you start with .r == .rold
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold;
		i++;
	}
	LX = LXold;
	LY = LYold;
	//Make sure that you start with .currentRestLength == .restLengthOld
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).restLengthOld = (*ed).L0;
			(*ed).currentRestLength = (*ed).restLengthOld;
			j++;
		}
	}

	///////////////////
	//k1=hf(y) and c1=hf(y)
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		k1.push_back((*(*vit)).rnew);
	}
	k1LX = LXnew;
	k1LY = LYnew;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			c1.push_back((*ed).restLengthNew);
		}
	}


	////////////////
	//k2=hf(y+k1/2) and c2=hf(y+c1/2)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold + k1[i] / 2.0;
		i++;
	}
	LX = LXold + k1LX / 2.0;
	LY = LYold + k1LY / 2.0;
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).currentRestLength = (*ed).restLengthOld + c1[j] / 2.0;
			j++;
		}
	}
	(*this).RK4Helper_EuilerStep(dt, errorfile); //crashes third time through 
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		k2.push_back((*(*vit)).rnew);
	}
	k2LX = LXnew;
	k2LY = LYnew;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			c2.push_back((*ed).restLengthNew);
		}

	}

	///////////////////////////
	//k3=hf(y+k2/2) and c3=hf(y+c2/2)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold + k2[i] / 2.0;
		i++;
	}
	LX = LXold + k2LX / 2.0;
	LY = LYold + k2LY / 2.0;
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).currentRestLength =(*ed).restLengthOld + c2[j] / 2.0;
			j++;
		}
	}
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		k3.push_back((*(*vit)).rnew);
	}
	k3LX = LXnew;
	k3LY = LYnew;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			c3.push_back((*ed).restLengthNew);
		}
	}


	/////////////////////////////////////////////
	//k4=hf(y+k3) and c4=hf(y+c3)
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold + k3[i];
		i++;
	}
	LX = LXold + k3LX;
	LY = LYold + k3LY;
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).currentRestLength = (*ed).restLengthOld + c3[j];
			j++;
		}
	}
	(*this).RK4Helper_EuilerStep(dt, errorfile);
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		k4.push_back((*(*vit)).rnew);
	}
	k4LX = LXnew;
	k4LY = LYnew;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			c4.push_back((*ed).restLengthNew);
		}
	}

	///////////////
	//Get rnew
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).rnew = (*(*vit)).rold + k1[i] / 6. + k2[i] / 3. + k3[i] / 3. + k4[i] / 6.;
		i++;
	}
	LXnew = LXold + (k1LX / 6. + k2LX / 3. + k3LX / 3. + k4LX / 6.);
	LYnew = LYold + (k1LY / 6. + k2LY / 3. + k3LY / 3. + k4LY / 6.);
	
	//Get lengthNew
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).restLengthNew = (*ed).restLengthOld + c1[j] / 6. + c2[j] / 3. + c3[j] / 3. + c4[j] / 6.;
			j++;
		}
	}
	//////////////////////////////////////
	//save the old values again: in case step fails.
	i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold;
		i++;
	}
	LX = LXold;
	LY = LYold;
	j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).currentRestLength =(*ed).restLengthOld;
			j++;
		}
	}
}


//HELPER CHANGE.
//Integrates the forcefunction and lengthFunction by one euler step 
//void Tissue_Dynamic::RK4Helper_EuilerStep(double dt, ofstream* errorfile) {
//	//Calculate the force on every vertex 
//	(*this).ForceFunction(errorfile);
//	//loop through all the vertices and set rnew
//	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
//		(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_MechForce * dt;
//	}
//	//Calculate any changes in rest length for every edge
//	(*this).LengthFunction(errorfile);
//	//loop through all the edges and set lengthNew
//	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
//		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
//			//save the old rest length, save the rate, and incorporate the old rest length with the rate of change * dt 
//			(*ed).restLengthNew = (*ed).SavedInfoForEvolve_RestLength * dt;
//		}
//	}
//	//Set LX and LY new
//	LXnew = dt * ForceLX;
//	LYnew = dt * ForceLY;
//}

void Tissue_Dynamic::RK4Helper_EuilerStep(double dt, ofstream* errorfile) {
	//Calculate the force on every vertex 
	(*this).ForceFunction(errorfile);

	//loop through all the vertices and set rnew
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		//(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_MechForce * (dt / alphaM);
		(*(*vit)).rnew = (*(*vit)).SavedInfoForEvolve_TotalForce * (dt / alphaM);
	}

	//Calculate any changes in rest length for every edge
	(*this).LengthFunction(errorfile);

	//loop through all the edges and set lengthNew
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			//save the old rest length, save the rate, and incorporate the old rest length with the rate of change * dt 
			(*ed).restLengthNew = (*ed).SavedInfoForEvolve_RestLength * dt;
		}
	}

	// CHECK THESE FACTORS
	//Set LX and LY new
	LXnew = (dt / TISSUE_ALPHA) * ForceLX;
	LYnew = (dt / TISSUE_ALPHA) * ForceLY;


}

//! Update restLengthOld and rold with specified value.
void Tissue_Dynamic::Update_Old_Mechanical_Variables(vector<Vect>& y, vector<double>& q) {
	//Store the values from y into the old mechanical variables.
	int i = 0;
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).rold = y.at(i);
		i++;
	}
	LXold = y.at(i).x;
	LYold = y.at(i).y;

	//Force the .r and LX,LY to be the y values.
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		(*(*vit)).r = (*(*vit)).rold;
	}
	LX = LXold;
	LY = LYold;

	int j = 0;
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).restLengthOld = q.at(j);
			j++;
		}
	}
	//Force the currentRestLength to be the q values.
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).currentRestLength = (*ed).restLengthOld;
		}
	}
}

// Moves the vertices to their new positions and updates the approproate flags if they cross a boundary & updates edge rest lengths
bool Tissue_Dynamic::HelperEvolve_UpdatePosition() {
	double newx, newy;
	bool VertexHasCrossed = 0;
	bool flag = 0;

	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++) {
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			(*ed).L0 = (*ed).restLengthNew;
			(*ed).currentRestLength = (*ed).restLengthNew;
			(*ed).restLengthOld = (*ed).restLengthNew;
		}
	}
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++) {
		flag = (*(*vit)).UpdatePosition();
		VertexHasCrossed = VertexHasCrossed || flag;
	}
	return VertexHasCrossed;
}

//! Calcualtes the total free energy within the dynamic tissue
double Tissue_Dynamic::CalculateFreeEnergy() {
	double freeenergy = 0;
	double length_difference_sqr = 0;
	double area_difference_sqr = 0;

	// (A_c - A_0)^2
	for (list<Cell*>::iterator cit = cells.begin(); cit != cells.end(); cit++)
	{
		area_difference_sqr += pow(((*(*cit)).GetArea() - (*(*cit)).A0), 2) / (*(*cit)).A0;
	}

	// (Lij - Lo)^2
	for (list<Edge*>::iterator dit = edges.begin(); dit != edges.end(); dit++){
		if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*dit)) {
			{
				length_difference_sqr += pow(((*ed).GetLength() - (*ed).L0), 2) / pow((*ed).L0,2);
			}
		}
	}
	freeenergy = (0.5 * Y * length_difference_sqr) + (0.5 * K * area_difference_sqr) - (*this).CalculateBoxEnergy(); //From Meryl's Thesis
	return freeenergy;
}

bool Tissue_Dynamic::T1(ofstream* errorfile) {
	int flag = 0;
	int i;

	vector<Edge*> t1edges;								//list to store the edges to t1 in (you must put them in a seprate list and then t1 them or the number of edges will change and the iterator will not function properly
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++) {
		if ((*(*eit)).GetLength() < MINLENGTH) {
			t1edges.push_back(&(*(*eit)));
			flag++;
		}
	}
	if (flag == 0) {
		return false;
	}
	else {
		//Output state of system before and after T1.
		//(*this).WriteOutputDataAppend(errorfile);
		for (i = 0; i < t1edges.size(); i++) {
			cout << "T1 of Edge " << (*t1edges[i]).id << "   at location   (" << (*t1edges[i]).GetCenter().x << " , " << (*t1edges[i]).GetCenter().y << ")" << endl;
			int nT1;
			double savedTension_T1;
			double savedRestLength_T1;
			double* ptr_savedTension_T1 = &savedTension_T1; //pointer for storing removed edge's tension
			double* ptr_savedRestLength_T1 = &savedRestLength_T1; //pointer for storing removed edge's rest length

			Vertex* v = (*this).HelperT1_RemoveEdge(t1edges[i], errorfile, &nT1, ptr_savedTension_T1, ptr_savedRestLength_T1);
			HelperResolveVertices_ResolveFourfoldVertex(v, ptr_savedTension_T1, ptr_savedRestLength_T1);
		}
		return true;
	}
}

// RemoveEdge deletes an edge and merges the two vertices to create a fourfold vertex
Vertex* Tissue_Dynamic::HelperT1_RemoveEdge(Edge* deledge, ofstream* errorfile, int* nT1, double* ptr_savedTension_T1, double* ptr_savedRestLength_T1) {
	int newfold;					//is the new vertex 4 or 5 fold
	int i, j, k, l, m;
	Vertex* v1ptr, * v2ptr, * vnewptr;			//pointers to the two merging vertices
	Cell* c1ptr, * c2ptr;			//pointers to the two cells that will loose an edge (the other cells remain uneffected b/c their edges do not change and they do not know their vertices)
	vector<Edge*> newedgelist;

	v1ptr = (*deledge).vertex1;
	v2ptr = (*deledge).vertex2;
	c1ptr = (*deledge).cell1;
	c2ptr = (*deledge).cell2;

	//If this will create a 5 fold vertex give an error
	newfold = (*v1ptr).GetEdgeNumber() + (*v2ptr).GetEdgeNumber() - 2;
	if (newfold > 4) {
		cout << "ERROR: DID NOT COLLAPSE AN EDGE TO GET A 5FOLD VERTEX " << endl;
		return NULL;
	}

	//If this will create a cell with only two edges give an error (ie if the cell on either side of the deleted edge is a triangle)
	if ((*c1ptr).GetVertexNumber() < 4 || (*c2ptr).GetVertexNumber() < 4) {
		cout << "ERROR: DID NOT COLLAPSE AN EDGE THAT IS PART OF A TRIANGLE" << endl;
		return NULL;
	}

	//If there are no errors continue

	//Record the number of t1s
	*nT1 = (*deledge).numbT1;

	//fill the new edge list
	for (i = 0; i < (*v1ptr).edges.size(); i++) {					//get the edges from vertex1
		if ((*deledge) != (*(*v1ptr).edges[i])) {
			newedgelist.push_back((*v1ptr).edges[i]);
		}
	}
	for (j = 0; j < (*v2ptr).edges.size(); j++) {					//get the edges from vertex 2
		if ((*deledge) != (*(*v2ptr).edges[j])) {
			newedgelist.push_back((*v2ptr).edges[j]);
		}
	}

	//FLAGS SET**********************************************************
	//find the new position and move the vertices there
	(*deledge).MoveVertsToCenter();
	(*this).HelperResolve_UpdatePosition(v1ptr);
	(*this).HelperResolve_UpdatePosition(v2ptr);

	//make the new vertex
	vnewptr = (*this).CreateVertex((*deledge).GetCenter(), newedgelist);

	//go to the four edges and change their ptrs from the old vertices to the new four fold vertex
	for (k = 0; k < newedgelist.size(); k++) {
		if ((*(*newedgelist[k]).vertex1).id == (*v1ptr).id) {
			(*newedgelist[k]).vertex1 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex1).id == (*v2ptr).id) {
			(*newedgelist[k]).vertex1 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex2).id == (*v1ptr).id) {
			(*newedgelist[k]).vertex2 = vnewptr;
		}
		else if ((*(*newedgelist[k]).vertex2).id == (*v2ptr).id) {
			(*newedgelist[k]).vertex2 = vnewptr;
		}
		else { cout << "ERROR IN DELETE EDGE" << endl; }
	}

	//Remove the old vertices from the master list
	(*this).DestroyVertex(v1ptr);
	(*this).DestroyVertex(v2ptr);

	//Remove the edge from the 2 cell lists of edges
	for (l = 0; l < (*c1ptr).edges.size(); l++) {
		if ((*(*c1ptr).edges[l]).id == (*deledge).id) {
			(*c1ptr).edges.erase((*c1ptr).edges.begin() + l);
			break;
		}
	}
	for (m = 0; m < (*c2ptr).edges.size(); m++) {
		if ((*(*c2ptr).edges[m]).id == (*deledge).id) {
			(*c2ptr).edges.erase((*c2ptr).edges.begin() + m);
			break;
		}
	}


	//Remove the edge from the master list
	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++) {//loop through all of the edges in the tissue
		if ((*(*eit)).id == (*deledge).id) {//if the curent edge is to be removed
			if (Edge_Dynamic* ed = dynamic_cast<Edge_Dynamic*>(*eit)) {
				*ptr_savedTension_T1 =  (*ed).GetTension_Magnitude(); //assign value to tension pointer
				cout << "Tension of saved T1 edge: " << *ptr_savedTension_T1 << endl;
				*ptr_savedRestLength_T1 = (*ed).GetRestLength(); //assign value to rest length pointer
				cout << "Rest length of saved T1 edge: " << *ptr_savedRestLength_T1 << endl;
			}
			DestroyEdge(*eit);
			break;
		}
	}

	//return a pointer to the vertex
	return vnewptr;
}

bool Tissue_Dynamic::HelperResolveVertices_ResolveFourfoldVertex(Vertex* v, double* ptr_savedTension_T1, double* ptr_savedRestLength_T1) {
	vector<Edge*> edges;
	vector<Cell*> cells;
	double stable12, stable14; //the stability when pairing edges 12 and edges 14 negitive values denote stability against breaking in that direction
	Vect Force12, Force14;	//the force from splitting in each topology used to set the angle on new edge growth

	//first make sure the vertex is fourfold
	if (!((*v).CheckFourfold()))
		cout << "ERROR trying to resolve nonfourfold vertex" << endl;

	//Get the edges and cells in CW order
	edges = (*v).GetCWEdges();
	cells = (*v).GetCWCells();

	//find the stability of each configuration
	stable12 = (*this).HelperResolveVertices_CheckStability(v, edges[0], edges[1], edges[2], edges[3], cells[0], cells[1], cells[2], cells[3], &Force12, ptr_savedTension_T1);
	stable14 = (*this).HelperResolveVertices_CheckStability(v, edges[3], edges[0], edges[1], edges[2], cells[3], cells[0], cells[1], cells[2], &Force14, ptr_savedTension_T1);

	//Three cases based on the stability
	if (stable12 < 0 && stable14 < 0) {
		return false;		//the vertex is stable youre done!
	}
	else if (stable12 > stable14) {
		(*this).HelperResolveVertices_UpdatePtrs(v, edges[0], edges[1], edges[2], edges[3], cells[0], cells[1], cells[2], cells[3], &Force12, ptr_savedRestLength_T1);
		return true;
	}
	else {
		(*this).HelperResolveVertices_UpdatePtrs(v, edges[3], edges[0], edges[1], edges[2], cells[3], cells[0], cells[1], cells[2], &Force14, ptr_savedRestLength_T1);
		return true;
	}
}

//Called by HelperResolveVertices_ResolveFourfoldVertex to check the stability of one configuration. Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors retruns false if the vertex is unstable in that topology
double Tissue_Dynamic::HelperResolveVertices_CheckStability(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force, double* ptr_savedTension_T1) {
	//First make new objects which are coppies of the objects in the linked lists we will minipulate these objecs and they will be destroyed when the function ends
	Cell c1 = (*cell1);
	Cell c2 = (*cell2);
	Cell c3 = (*cell3);
	Cell c4 = (*cell4);
	Edge e1 = (*edge1);
	Edge e2 = (*edge2);
	Edge e3 = (*edge3);
	Edge e4 = (*edge4);
	double stability;
	double a1, a2, a3, a4;

	//Make the new edge and vertices resulting from the split into 2 3fold vertices
	Edge_Dynamic enew;
	Vertex v12; //the vertex on edges 1 and 2
	Vertex v34; //the vertex on edges 3 and 4

	//Now update the info on the coppies of the objects so they are 2 3fold vertices

	//For the new vertices
	v12.r = (*vert).r;							//put the two new verts where the olf 4ourfold was
	v12.edges.push_back(&e1);
	v12.edges.push_back(&e2);
	v12.edges.push_back(&enew);
	v34.r = (*vert).r;							//put the two new verts where the olf 4ourfold was
	v34.edges.push_back(&e3);
	v34.edges.push_back(&e4);
	v34.edges.push_back(&enew);

	//For the new edge
	enew.yflag = 0;		//the new edge has no length and therefore cannot have a flag
	enew.xflag = 0;
	enew.vertex1 = &v12;
	enew.vertex2 = &v34;
	enew.cell1 = &c4;
	enew.cell2 = &c2;

	//For the old edges
	e1.UpdateVertex(vert, &v12);
	e2.UpdateVertex(vert, &v12);
	e3.UpdateVertex(vert, &v34);
	e4.UpdateVertex(vert, &v34);

	//Now we have at the position of the fourfold vertex two threefold vertices in which everyone knows all of their neighbors correclty, we can now calculate the forces based on what the tensiions and pressures are in this configuration

	//pressure differences across cells
	a1 = c1.GetArea();
	a2 = c2.GetArea();
	a3 = c3.GetArea();
	a4 = c4.GetArea();
	double p2 = (c2.GetPressure(a2) - c1.GetPressure(a1)); //pressure difference acorss edge2 between cell 1 and cell 2 taken CCW
	double p3 = (c3.GetPressure(a3) - c2.GetPressure(a2));
	double p4 = (c4.GetPressure(a4) - c3.GetPressure(a3));
	double p1 = (c1.GetPressure(a1) - c4.GetPressure(a4));

	//getting z X edge (both the diredction and magnitude are correctHelperResolveVertices_ResolveFourfoldVertex
	Vect zXe1 = (e1).GetVectorDifference(&v12, v12.GetOtherVertex(&e1)).Rotate(PI / 2.0);
	Vect zXe2 = (e2).GetVectorDifference(&v12, v12.GetOtherVertex(&e2)).Rotate(PI / 2.0);
	Vect zXe3 = (e3).GetVectorDifference(&v34, v34.GetOtherVertex(&e3)).Rotate(PI / 2.0);
	Vect zXe4 = (e4).GetVectorDifference(&v34, v34.GetOtherVertex(&e4)).Rotate(PI / 2.0);

	//Get the forces
	Vect ForceT1 = (e1).GetTension_ForceOnVertex(&v12);
	Vect ForceT2 = (e2).GetTension_ForceOnVertex(&v12);
	Vect ForceT3 = (e3).GetTension_ForceOnVertex(&v34);
	Vect ForceT4 = (e4).GetTension_ForceOnVertex(&v34);
	Vect ForceP1 = zXe1 * (p1 * 0.5);
	Vect ForceP2 = zXe2 * (p2 * 0.5);
	Vect ForceP3 = zXe3 * (p3 * 0.5);
	Vect ForceP4 = zXe4 * (p4 * 0.5);
	*Force = ((ForceT1 + ForceT2 + ForceP1 + ForceP2) - (ForceT3 + ForceT4 + ForceP3 + ForceP4)) * 0.5;

	//Calculate the tension on the new edge used stored tension value
	Vect ForceNewEdge = enew.GetTension_ForceOnVertex(&v12, ptr_savedTension_T1);

	//Check the stability
	stability = (*Force).Magnitude() - ForceNewEdge.Magnitude();
	return stability;
}

//Called by HelperResolveVertices_ResolveFourfoldVertex to update the pointers when the vertex needs to resolve, Splits vertices so edges (e1,e2) and (e3,e4) are paired and cells c2 and c4 are neighbors 
Edge* Tissue_Dynamic::HelperResolveVertices_UpdatePtrs(Vertex* vert, Edge* edge1, Edge* edge2, Edge* edge3, Edge* edge4, Cell* cell1, Cell* cell2, Cell* cell3, Cell* cell4, Vect* Force, double* ptr_savedRestLength_T1) {
	Vect oldvr = (*vert).r;
	Vect newv1r = Vect((*vert).r, (*Force).Angle(), NEWEDGELENGTH / 2.0);		 //find the possition of the new vertices
	Vect newv2r = Vect((*vert).r, (*Force).Angle() + PI, NEWEDGELENGTH / 2.0);

	//get the edge lists
	vector<Edge*> v12edges;
	v12edges.push_back(edge1);
	v12edges.push_back(edge2);
	vector<Edge*> v34edges;
	v34edges.push_back(edge3);
	v34edges.push_back(edge4);

	Vertex* v12ptr = (*this).CreateVertex(oldvr, v12edges);						//create two new vertices at the same position as the old vertex
	Vertex* v34ptr = (*this).CreateVertex(oldvr, v34edges);
	Edge* eptr = (*this).CreateEdge(edge1, 0, 0, v12ptr, v34ptr, cell2, cell4, ptr_savedRestLength_T1);			//create new edge with vertex and cell ptrs (note is has no flags b/c it can't cross a boundary with length 0) //used saved L0 for new edge's L0

	(*v12ptr).edges.push_back(eptr);								//add the new edge to the new vertices
	(*v34ptr).edges.push_back(eptr);
	(*cell2).edges.push_back(eptr);									//add the new edge to the cells
	(*cell4).edges.push_back(eptr);
	(*edge1).UpdateVertex(vert, v12ptr);								//uptate the vertices on the edges
	(*edge2).UpdateVertex(vert, v12ptr);
	(*edge3).UpdateVertex(vert, v34ptr);
	(*edge4).UpdateVertex(vert, v34ptr);

	(*this).DestroyVertex(vert);	//destory the fourfold vertex

	//now move the vertices apart along the line of force and set the flags
	(*v12ptr).rnew = newv1r;
	(*v34ptr).rnew = newv2r;
	(*this).HelperResolve_UpdatePosition(v12ptr);
	(*this).HelperResolve_UpdatePosition(v34ptr);

	(*cell1).SavedInfoForEvolve_CWVerts = (*cell1).GetClockWiseVertices();
	(*cell2).SavedInfoForEvolve_CWVerts = (*cell2).GetClockWiseVertices();
	(*cell3).SavedInfoForEvolve_CWVerts = (*cell3).GetClockWiseVertices();
	(*cell4).SavedInfoForEvolve_CWVerts = (*cell4).GetClockWiseVertices();
	return eptr;
}

// Creates a new edge and addes it to the list of edges, returns a pointer to that edge.
//Uses the saved edge rest length to set the new edge's rest length
Edge* Tissue_Dynamic::CreateEdge(Edge* subclass, bool xflg, bool yflg, Vertex* v1, Vertex* v2, Cell* c1, Cell* c2, double* ptr_savedRestLength_T1) {
	int largestID, newid;
	Edge* newedge{};
	//Get the id
	largestID = (*max_element(edgeIDs.begin(), edgeIDs.end()));			//first look through all of the current vertex ids and get the highest
	newid = largestID + 1;								//Now make a new ID that is unique

	if (newid > 1000000000) {
		cout << "ERROR to many edges" << endl;
		return NULL;
	}
	//create the dynamic edge and add it to the list of edges
	Edge_Dynamic* eptr = new Edge_Dynamic;
	(*eptr).id = newid;
	(*eptr).xflag = xflg;
	(*eptr).yflag = yflg;
	(*eptr).vertex1 = v1;
	(*eptr).vertex2 = v2;
	(*eptr).cell1 = c1;
	(*eptr).cell2 = c2;
	(*eptr).L0 = *ptr_savedRestLength_T1;
	(*eptr).currentRestLength = (*eptr).L0;
	(*eptr).restLengthOld = (*eptr).L0;
	(*eptr).restLengthNew = (*eptr).L0;
	(edges).push_back(eptr);
	(edges).push_back(eptr);
	edgeIDs.push_back(newid);

	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++) {
		if ((*(*eit)).id == newid) {
			//return &(*(*eit));
			newedge = &(*(*eit));
			break;
		}
	}
	return newedge;
}

//! Step size control mechanical evolution (using double step error estimation across both vertices and edges)
bool Tissue_Dynamic::Double_Step_Size_Control(const vector<Vect>& y_error, const vector<double>& rest_error, const double error_tolerance_absolute, double* initial_step_size, double consistency_order) {
	//dimension of mechanical systems (vertices plus the boxsize)
	int dimension_mechanical_vect_v = vertices.size() + 1; //all vertices plus the box size (LX, LY) => verts+1
	int dimension_mechanical_vect_e = edges.size(); //all edges
	int dimension_mechanical_combo_vects = (dimension_mechanical_vect_v + dimension_mechanical_vect_e); //size equal to (vertex ordered pairs + edges)
	int dimension_mechanical_double = (2 * dimension_mechanical_vect_v) + dimension_mechanical_vect_e; //size equal to (vertex x + vertex y + edges)

	//Initialize the flag that is output to show if satisying error tolerance.
	bool step_error_check = false;
	//Initialize the factor for scaling the step size.
	double scale_factor_double_step = 1.0;
	//Initialize a variable for storing the adjusted step size.
	double adjusted_step_size = *initial_step_size;
	//The order of the numerical solver (consistency_order)
	double safety_factor = .9;
	//perhaps tune this up.
	double upper_fractional_tolerance = 1.1;
	//possibly change this and the parameters
	double lower_fractional_tolerance = 0.5;
	//Check whether error exceeds tolerance.
	vector<double> excess_error;
	excess_error.resize(dimension_mechanical_double);
	//Check whether error is excessively below tolerance.
	vector<double> low_error;
	low_error.resize(dimension_mechanical_double);
	//Ratio of error to tolerance
	vector<double> error_ratio;
	error_ratio.resize(dimension_mechanical_double);
	//Store the max ratio of the error to the tolerance.
	double max_error_ratio = 0.0;
	//Check for excess error.
	double max_error_check = 0.0;

	//For stability, enforce bounds on the scale factor.
	double lower_bound_scaling_factor = 1. / 5.;
	double upper_bound_scaling_factor = 5.;

	int i = 0;
	// Is this correct?? probably not...
	//Subtract the tolerated from error from the observed error (for all components)
	for (int index = 0; index != dimension_mechanical_combo_vects; index++) {
		if (index < dimension_mechanical_vect_v) { //handles all vertices first
			//CHANGE BY HSN
			//Store the ratio of error to tolerance for the (x,y) vertex coordinates.
			excess_error[i] = y_error[index].x - upper_fractional_tolerance * error_tolerance_absolute;
			error_ratio.at(i) = y_error.at(index).x / error_tolerance_absolute;
			i++;
			excess_error.at(i) = y_error.at(index).y - upper_fractional_tolerance * error_tolerance_absolute;
			error_ratio.at(i) = y_error.at(index).y / error_tolerance_absolute;
			i++;
			//CHANGE BY HSN
		} 
		else if (index >= dimension_mechanical_vect_v) { //when index exceeds verts+1, move on to the rest lengths
			excess_error[i] = rest_error[index - dimension_mechanical_vect_v] - upper_fractional_tolerance * error_tolerance_absolute;
			//Store the ratio of error to tolerance for the rest lengths.
			error_ratio[i] = rest_error[index - dimension_mechanical_vect_v] / error_tolerance_absolute;
			i++;
		}
	}

	//Store the maximum error ratio.
	max_error_ratio = *max_element(error_ratio.begin(), error_ratio.end());
	//Store the maximum error check.
	max_error_check = *max_element(excess_error.begin(), excess_error.end());

	//If any of the components violated the error tolerance as enforced, set a factor for scaling the step size.
	if (max_error_check > 0.0) {
		scale_factor_double_step = safety_factor * pow(max_error_ratio, -1 / consistency_order);
	}

	i = 0;
	//Check if all of the error is below the low fractional tolerance (scale up step size)
	//This one for loop handles both the vertex errors and the edge.  
	for (int index = 0; index != dimension_mechanical_combo_vects; index++) {
		if (index < dimension_mechanical_vect_v) { //for handling the (x,y) vertex coordinates
			low_error.at(i) = y_error.at(index).x - lower_fractional_tolerance * error_tolerance_absolute; //gets the x coordiante error
			i++;
			low_error.at(i) = y_error.at(index).y - lower_fractional_tolerance * error_tolerance_absolute; //gets the y coordinate error
			i++;
		} 
		else if (index >= dimension_mechanical_vect_v) { //when index exceeds verts+1, move onto handling the rest lengths
			low_error[i] = rest_error[index - dimension_mechanical_vect_v] - lower_fractional_tolerance * error_tolerance_absolute;
			i++;
		}
	}

	//If all of the errors are less than half of the tolerance, define a factor for upscaling the step size.
	max_error_check = *max_element(low_error.begin(), low_error.end());
	if (max_error_check < 0.0) {
		scale_factor_double_step = safety_factor * pow(max_error_ratio, -1 / (consistency_order + 1));
	}

	//If the up scale factor is greater than the upper bound, snap the scale factor to the upper bound.
	if (scale_factor_double_step > upper_bound_scaling_factor) {
		scale_factor_double_step = upper_bound_scaling_factor;
	}
	if (scale_factor_double_step < 1.00) {
		step_error_check = true;
	}
	//If the scale factor is lower than the lower bound, snap the scale factor to the lower bound.
	if (scale_factor_double_step < lower_bound_scaling_factor) {
		scale_factor_double_step = lower_bound_scaling_factor;
	}

	//Adjust the step size.
	//cout << "Scale factor for system: " << scale_factor_double_step << "\n";
	adjusted_step_size = *initial_step_size * scale_factor_double_step;
	*initial_step_size = adjusted_step_size;

	//Return the flag for if the step size was too large.
	return step_error_check;
}

bool Tissue_Dynamic::StrictIsEquilibrated(){
	double maxBoxDeformation = 0.00001;
	double boxXDeformation = (LXnew - LXold) / LXold;
	double boxYDeformation = (LYnew - LYold) / LYold;
	double maxVertexDisplacment = LX/sqrt((*this).cells.size())*0.001;
	double maxEdgeTension = Y*EpsilonMax;

	//cout << "Xnew: " << LXnew << " Xold: " << LXold << endl;
	//cout << "Box Deformation: " << boxXDeformation << " , " << boxYDeformation << endl;

	if (boxXDeformation > maxBoxDeformation || boxYDeformation > maxBoxDeformation) {
		cout << "Box deformation exceeded tolerance." << endl;
		return false;
	}

	for (list<Edge*>::iterator eit = edges.begin(); eit != edges.end(); eit++){
		if ((*eit)->GetTension_Magnitude() > maxEdgeTension) {
			cout << "Edge tension exceeded tolerance." << endl;
			return false;
		}
	}
	for (list<Vertex*>::iterator vit = vertices.begin(); vit != vertices.end(); vit++){
		Vect oldr = (*vit)->rold;
		Vect newr = (*vit)->rnew;
		double displacment = (newr - oldr).Magnitude();
		if (displacment > maxVertexDisplacment) {
			cout << "Vertex displacement exceeded tolerance." << endl;
			return false;
		}
	}

	return true;
}