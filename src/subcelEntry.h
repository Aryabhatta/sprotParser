#ifndef SUBCELENTRY_H_
#define SUBCELENTRY_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
using namespace std;

class subcelEntry
{
	string AC;	
	string ID;
	string OS;
	string OS_ID;
	vector<string> location;
	
	int expLoc;
	int nonexpLoc;
	vector<string> pubmedIDs;
	
public:
	subcelEntry();
	virtual ~subcelEntry();
	
	void setID(string ID) { this->ID = ID; }
	void setAC(string AC) { this->AC = AC; }
	void setOS(string OS) { this->OS = OS; }
	void setOSID(string OS_ID) { this->OS_ID = OS_ID; }
	void addLocation(string loc);
	
	void incrementExpLoc() { expLoc++; }
	void incrementNonExpLoc() { nonexpLoc++; }
	void addPubMedId(string pubmedID) { pubmedIDs.push_back(pubmedID); }
		
	int getExpLoc() { return this->expLoc; }
	int getNonExpLoc() { return this->nonexpLoc; }
	
	string getOrgName() { return this->OS; }
	string getOrgId() { return this->OS_ID; }
		
	void clear();
	string toString();
	string getPubMedEntries();
};

#endif /*SUBCELENTRY_H_*/
