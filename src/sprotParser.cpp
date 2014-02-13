//============================================================================
// Name        : sprotParser.cpp
// Author      : ShrikantV
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <map>
#include <boost/algorithm/string.hpp>
#include "subcelEntry.h"
#include "organism.h"
using namespace std;
using namespace boost;

int main(int argc, char ** argv) {
	
	if(argc < 2) {
		cout << "Usage:" << endl << endl;
		cout << "./sprotParser [filename1] [filename2] [filename3] ..." << endl << endl;
		cout << "[filename] : uniprot_sprot_*****.dat" << endl << endl;
		return 0;
	}

	string outFileNamet1 = "Eukaryote_Table1.dat";
	string outFileNamet2 = "Eukaryote_Table2.dat";
	ofstream outputFilet1(outFileNamet1.data());
	ofstream outputFile(outFileNamet2.data());
	ofstream outputFileSum("Eukaryote_Summary.dat");

	if(!outputFilet1.is_open()) {
		cout << "Error opening output file for Table 1 !!" << endl;
		return -1;		
	}
	if(!outputFile.is_open()) {
		cout << "Error opening output file !!" << endl;
		return -1;
 	}
	if(!outputFileSum.is_open()) {
		cout << "Error opening output Summary file !!" << endl;
		return -1;		
	}
	
	// data structures for summary
	vector<organism> orgSummary;
	map<string,int> orgMap; //  map <OrgID, vectorIndex>
	
	// write header for Table 1
	outputFilet1 << "AC" << "\t" << "PubMedId" << "\t" << "LocClass" << endl;
	outputFilet1 << "---------------------------------------------------------" << endl;
	
	// write header
	outputFile << "AC" << "\t" << "ID" << "\t" << "Organism" << "\t" << "OrganismId" << "\t";
	outputFile << "#Localizations_exp" << "\t" << "#Localizations_nonExp" << "\t" <<  "Localization" << endl;
	outputFile << "--------------------------------------------------------------------------------" << endl;
	
	// write header for Summary file
	outputFileSum << "Organism" << "\t" << "OrganismId" << "\t" << "#Localizations_exp" << "\t" << "#Localizations_nonExp" << "\t" << "#No_Localizations" << endl;
	outputFileSum << "---------------------------------------------------------------------------------------------" << endl;
	
	for(int i=1; i<argc; i++) {
		string fileName = argv[i];
	//	string outFileNamet1 = fileName.substr(0,fileName.find(".dat",0)) + "_Table1.dat";
	//	string outFileNamet2 = fileName.substr(0,fileName.find(".dat",0)) + "_Table2.dat";
		
		cout << "Parsing sprot file " << fileName << " !!!" << endl;
		
		ifstream inputFile(fileName.data());
	//	ofstream outputFilet1(outFileNamet1.data());
	//	ofstream outputFile(outFileNamet2.data());
	//	ofstream outputFileSum("orgSummary.dat");
		
		if(!inputFile.is_open()) {
			cout << "Error opening input file !!" << endl;
			return -1;
		}
	//	if(!outputFilet1.is_open()) {
	//		cout << "Error opening output file for Table 1 !!" << endl;
	//		return -1;		
	//	}
	//	if(!outputFile.is_open()) {
	//		cout << "Error opening output file !!" << endl;
	//		return -1;
	// 	}
	//	if(!outputFileSum.is_open()) {
	//		cout << "Error opening output Summary file !!" << endl;
	//		return -1;		
	//	}
		
		// data structures for summary
	//	vector<organism> orgSummary;
	//	map<string,int> orgMap; //  map <OrgID, vectorIndex>
		
		string inputLine;
		
		enum states { checkID, checkAC, nameOrganism, idOrganism, pubmedIDs, readLocalization};
		int currentState = checkID;
		
		//Identifiers
		string idIdentifier = "ID ";
		string acIdentifier = "AC ";
		string orgIdentifier = "OS ";
		string orgIdIdentifier = "OX ";
		string pubmedIdentifier = "RN ";
		string locIdentifier = "CC -!- SUBCELLULAR LOCATION";
		
		subcelEntry entry;
		
	//	// write header for Table 1
	//	outputFilet1 << "AC" << "\t" << "PubMedId" << "\t" << "LocClass" << endl;
	//	outputFilet1 << "---------------------------------------------------------" << endl;
	//	
	//	// write header
	//	outputFile << "AC" << "\t" << "ID" << "\t" << "Organism" << "\t" << "OrganismId" << "\t";
	//	outputFile << "#Localizations_nonExp" << "\t" << "#Localizations_exp" << "\t" <<  "Localization" << endl;
	//	outputFile << "--------------------------------------------------------------------------------" << endl;
	//	
	//	// write header for Summary file
	//	outputFileSum << "Organism" << "\t" << "OrganismId" << "\t" << "#Localizations_exp" << "\t" << "#Localizations_nonExp" << "\t" << "#No_Localizations" << endl;
	//	outputFileSum << "---------------------------------------------------------------------------------------------" << endl;
		
		string RC="";
		
		// parse line by line to convert first space to tab
		while( getline(inputFile,inputLine) ) {
			
			// trim multiple whitespaces to  one whitespace
			while(inputLine.find("  ",0) != string::npos ) {
				inputLine.replace(inputLine.find("  ",0),2," ");
			}
					
			// Additional check to make sure state transitions are working fine
			if(inputLine.substr(0,3) == "ID " && currentState != checkID ) {
				cout << "We have a problem at inputLine: " << inputLine << endl;
				break;
			}
			
			if( currentState == checkID) {
				
				if(inputLine.substr(0,3) == idIdentifier) {
					std::vector<string> strs;
					boost::split(strs, inputLine, boost::is_any_of(" "));
					
					string id = strs[1];				
					entry.setID(id);
					currentState = checkAC;
				}
				
			} else if(currentState == checkAC) {
				
				if(inputLine.substr(0,3) == acIdentifier) {
					std::vector<string> strs;
					boost::split(strs, inputLine, boost::is_any_of(" "));
					
					string ac = strs[1];				
					ac = ac.replace(ac.find_first_of(';'),1,"");
					entry.setAC(ac);
					currentState = nameOrganism;
				}
				
			} else if(currentState == nameOrganism) {
				
				if(inputLine.substr(0,3) == orgIdentifier) {
					string orgName = inputLine.substr(orgIdentifier.length(), string::npos);
					
					orgName = orgName.substr(0,orgName.find_first_of('('));
					entry.setOS(orgName);				
					currentState = idOrganism;
				}
				
			} else if(currentState == idOrganism) {
				
				if(inputLine.substr(0,3) == orgIdIdentifier) {
					std::vector<string> strs;
					boost::split(strs, inputLine, boost::is_any_of(" "));
					
					string tempStr = strs[1];
					std::vector<string> strs1;
					boost::split(strs1, tempStr, boost::is_any_of("="));
					
					string orgId = strs1[1];
					orgId = orgId.replace(orgId.find_first_of(';'),1,"");
					entry.setOSID(orgId);
					
					if(orgMap.count(orgId) != 1) { // organism not found
						organism newOrganism;
						newOrganism.setOSId(orgId);
						newOrganism.setOSName(entry.getOrgName());
						
						orgSummary.push_back(newOrganism);					
						orgMap[orgId] = orgSummary.size() - 1; 
					}
					
					currentState = pubmedIDs;
				}
							
			} else if(currentState == pubmedIDs) {
				
				if(inputLine.substr(0,3) != "CC ") {
					
					if(pubmedIdentifier == "RX " && inputLine.substr(0,3) == "RC " ) {
						RC = "";
//						std::vector<string> strs;
//						boost::split(strs, inputLine, boost::is_any_of(" "));
//						
//						string tissue = strs[1];
//						std::vector<string> strs1;
//						boost::split(strs1, tissue, boost::is_any_of("="));
						
						if(inputLine.find('=',0) != string::npos) {
							RC = inputLine.substr(inputLine.find_first_of('=')+1,string::npos);
							if(RC.find(";",0) != string::npos ) {
								RC.replace(RC.find_last_of(';'),1,"");	
							}
						}
						
//						if(strs1[0]=="TISSUE") {
//							RC = strs1[1];
//							if(RC.find(";",0) != string::npos ) {
//								RC.replace(RC.find_last_of(';'),1,"");	
//							}						
//						}					
					}
					
					if(inputLine.substr(0,3) == pubmedIdentifier) {
						
						if(pubmedIdentifier.find("RN ") != string::npos) {
							pubmedIdentifier = "RP ";
							currentState = pubmedIDs;
						} else if (pubmedIdentifier.find("RP ") != string::npos) {
							
							bool locationFound = false;
							
							while(inputLine.find("RP ",0) != string::npos) {
								if(inputLine.find("SUBCELLULAR LOCATION") != string::npos) {
									locationFound = true;
									break;
								}
								getline(inputFile,inputLine);
							}
							
							if(locationFound){
								pubmedIdentifier = "RX ";
							} else {
								pubmedIdentifier = "RN ";	
							}
							
							currentState = pubmedIDs;
					
						} else if(pubmedIdentifier.find("RX ") != string::npos) {
							std::vector<string> strs;
							boost::split(strs, inputLine, boost::is_any_of(" "));
							
							string pubKeyVal = strs[1];
							std::vector<string> strs1;
							boost::split(strs1, pubKeyVal, boost::is_any_of("="));
						
							string pubmedId = "";
							if(strs1[0]=="PubMed") {
								pubmedId = strs1[1];
							} else {
								pubmedId = "NO_REFERENCE;";
							}
							pubmedId = pubmedId.replace(pubmedId.find_first_of(';'),1,"");
							
							if(!RC.empty()) {
								pubmedId.append("," + RC);
								RC.clear();//reset RC
							}
							
							entry.addPubMedId(pubmedId);
							
							pubmedIdentifier = "RN ";
							currentState = pubmedIDs;
						}				
					}
				} else { // change state, "CC " started
					pubmedIdentifier = "RN "; // resetting for next ID
					currentState = readLocalization;
				}
				
			} else if(currentState == readLocalization) {
				
				if(inputLine.substr(0,locIdentifier.length()) == locIdentifier) {
					
					string subCelText = "";
					while(true) {
						subCelText.append(inputLine.substr(2, string::npos));
						getline(inputFile,inputLine);
						while(inputLine.find("  ",0) != string::npos ) {
							inputLine.replace(inputLine.find("  ",0),2," ");
						}
						if(inputLine.find("CC -",0) != string::npos) {
							if(inputLine.find("CC -!- SUBCELLULAR LOCATION",0) == string::npos) {
								break;	
							} 
						}
					}
					
					while(subCelText.find("-!- SUBCELLULAR LOCATION:",0) != string::npos) {
						subCelText.replace(subCelText.find("-!- SUBCELLULAR LOCATION:",0),25,"" );
					}
					
					currentState = checkID; // back to searching for ID
					
					// parse subCelText								
					boost::trim(subCelText);
					std::vector<string> strs;
					boost::split(strs, subCelText, boost::is_any_of("."));
					
					vector<string>::iterator iter;
					for(iter=strs.begin();iter!=strs.end();++iter) {
						string token = *iter;
						if(token.empty()) {
							continue;
						}
						if(token.find("Note=", 0) != string::npos ) {
							break; // discontinue after "Note=" is encountered
						}
						
						if(token.find("(Probable)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
							
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							}						
							continue;
						}
						if(token.find("(By similarity)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
							
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							}
							continue;
						}
						if(token.find("(Possible)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
	
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							}						
							continue;
						}
						if(token.find("(Potential)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
	
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							}
							continue;
						}
						
						// we have an experimental subcellular location
						entry.addLocation(token);
						entry.incrementExpLoc();
						
						if(orgMap.count(entry.getOrgId()) == 1) {
							orgSummary[orgMap[entry.getOrgId()]].incExpLoc();
						}
					}
					
					// write to output file
					if(entry.getExpLoc() != 0 || entry.getNonExpLoc() != 0) {
						//write pubmedId's
						outputFilet1 << entry.getPubMedEntries();
						
						// write Table 2
						outputFile << entry.toString() << endl;
		//				cout << entry.toString() << endl;
					} else {
						// CC -!- SUBCE.. section is present but no entry
						if(orgMap.count(entry.getOrgId()) == 1) {
							orgSummary[orgMap[entry.getOrgId()]].incNoLoc();
						}					
					}
					entry.clear();
				}
				
				if(inputLine.substr(0,3) != "CC ") {
					// could not find subcellular localization for this protein
					currentState = checkID; // back to searching for ID
					
					// write to output file
	//				outputFile << entry.toString() << endl;
	//				cout << entry.toString() << endl;
	
					if(orgMap.count(entry.getOrgId()) == 1) { 
						// CC -!- not found
						orgSummary[orgMap[entry.getOrgId()]].incNoLoc();
					}					
					
					entry.clear();
				}			
			}		
		}
		
		inputFile.close();
	}

	// write to outputFileSummary
	vector<organism>::iterator iter;
	for(iter=orgSummary.begin();iter != orgSummary.end(); ++iter) {
		outputFileSum << (*iter).toString();
	}

	outputFilet1.close();
	outputFile.close();
	outputFileSum.close();
	
	return 0;
}
