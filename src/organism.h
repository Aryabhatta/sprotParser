#ifndef organism_H_
#define organism_H_

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class organism
{
	string OSName;
	string OSId;
	int expLoc;
	int nonExpLoc;
	int noLoc;
	
public:
	organism();
	virtual ~organism();
	
	void setOSName(string OrgName) {this->OSName = OrgName; }
	void setOSId(string OrgId) {this->OSId = OrgId; }
	
	void incExpLoc() { this->expLoc++; }
	void incNonExpLoc() {this->nonExpLoc++; }
	void incNoLoc() {this->noLoc++; }
	
	string toString();
};

#endif /*organism_H_*/
