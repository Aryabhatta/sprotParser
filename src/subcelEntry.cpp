#include "subcelEntry.h"

subcelEntry::subcelEntry()
{
	expLoc = 0; 
	nonexpLoc=0;
}

subcelEntry::~subcelEntry()
{
}

void subcelEntry::addLocation(string loc) {
	location.push_back(loc);
}

void subcelEntry::clear() {
	this->AC.clear();
	this->ID.clear();
	this->OS.clear();
	this->OS_ID.clear();
	this->location.clear();
	this->pubmedIDs.clear();
	
	this->expLoc = 0;
	this->nonexpLoc =0;
}

string subcelEntry::toString() {
	string entryString = "";
	entryString.append(this->AC + "\t");
	
	if(this->ID.length() < 11 ) {
			this->ID.append(11-this->ID.length(),' ');
	}
	entryString.append(this->ID + "\t");
	
	if(this->OS.length() < 40 ) {
		this->OS.append(40-this->OS.length(),' ');
	}
	entryString.append(this->OS + "\t");
	entryString.append(this->OS_ID + "\t");
	
	stringstream ssNonExp, ssExp;
	ssNonExp << this->nonexpLoc;
	ssExp << this->expLoc;
	
	entryString.append(ssExp.str() + "\t");
	entryString.append(ssNonExp.str() + "\t");	
	
	vector<string>::iterator iter;
	for(iter=location.begin();iter != location.end(); ++iter) {
		entryString.append(*iter + ".");
	}
	
	if(entryString.find_last_of(".",0) != string::npos) {
		entryString.replace(entryString.find_last_of('.'),1,"");	
	}
	
	return entryString;
}

string subcelEntry::getPubMedEntries() {
	string pubEntry = "";
	
	vector<string>::iterator iter;
	for(iter=pubmedIDs.begin(); iter!=pubmedIDs.end(); ++iter ) {
		pubEntry.append(this->AC + "\t");
		
		string token = *iter;
		
		if(token.find(",",0) != string::npos) {
			std::vector<string> strs;
			boost::split(strs, token, boost::is_any_of(","));
		
			if(strs[0].length() < 10) {
				strs[0].append(10-strs[0].length(),' ');
			}
			pubEntry.append(strs[0] + "\t");
			pubEntry.append(strs[1] + "\n");
		} else {
			pubEntry.append(token + "\n");
		}		
	}
	
	return pubEntry;
}