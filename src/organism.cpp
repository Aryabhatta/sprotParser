#include "organism.h"
#include <exception>

organism::organism()
{
	this->expLoc = 0;
	this->nonExpLoc = 0;
	this->noLoc = 0;
}

organism::~organism()
{
}


string organism::toString() {
	string orgEntry ="";
	
	try{
		// structuring for better view
		if(this->OSName.length() < 50) {
			this->OSName.append(50-this->OSName.length(),' ');
		}
		orgEntry.append(this->OSName + "\t");
		orgEntry.append(this->OSId + "\t");
		
		stringstream ss1,ss2,ss3;
		ss1 << this->expLoc;
		ss2 << this->nonExpLoc;
		ss3 << this->noLoc;
		
		orgEntry.append(ss1.str() + "\t");
		orgEntry.append(ss2.str() + "\t");
		orgEntry.append(ss3.str() + "\n");
	} catch(exception & e) {
		cout << e.what() << endl;
		cout << "Exception caught !" << endl;
	}
	
	return orgEntry;
}