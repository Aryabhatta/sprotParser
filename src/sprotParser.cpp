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
#include <set>
#include <boost/algorithm/string.hpp>
#include "subcelEntry.h"
#include "organism.h"
using namespace std;
using namespace boost;

int parseSprot(int argc, char ** argv) {
	
	if(argc < 2) {
		cout << "Usage:" << endl << endl;
		cout << "./sprotParser [filename1] [filename2] [filename3] ..." << endl << endl;
		cout << "[filename] : uniprot_sprot_*****.dat" << endl << endl;
		return 0;
	}

	string outFileNamet1 = "Archaea_Table1.dat";
	string outFileNamet2 = "Archaea_Table2.dat";
	ofstream outputFilet1(outFileNamet1.data());
	ofstream outputFile(outFileNamet2.data());
	ofstream outputFileSum("Archaea_Summary.dat");

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
	
	// check for completeness
	set<string> HumanProteins;
	
	// write header for Table 1
	outputFilet1 << "AC" << "\t" << "OrganismId" << "\t" << "PubMedId" << "\t" << "LocClass" << endl;
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
		
		cout << "Parsing sprot file " << fileName << " !!!" << endl;
		
		ifstream inputFile(fileName.data());
		
		if(!inputFile.is_open()) {
			cout << "Error opening input file !!" << endl;
			return -1;
		}
		
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
				
			} /*else*/ if(currentState == checkAC) {
				
				if(inputLine.substr(0,3) == acIdentifier) {
					std::vector<string> strs;
					boost::split(strs, inputLine, boost::is_any_of(" "));
					
					string ac = strs[1];				
					ac = ac.replace(ac.find_first_of(';'),1,"");
					entry.setAC(ac);
//					if(HumanProteins.count(ac) != 1) {
//						HumanProteins.insert(ac);	
//					} else {
//						cout << "Duplicate Entry : " << ac << endl;
//					}
					
					currentState = nameOrganism;
				}
				
			} /*else*/ if(currentState == nameOrganism) {
				
				if(inputLine.substr(0,3) == orgIdentifier) {
					string orgName = inputLine.substr(orgIdentifier.length(), string::npos);
					
					orgName = orgName.substr(0,orgName.find_first_of('('));
					entry.setOS(orgName);				
					currentState = idOrganism;
				}
				
			} /*else*/ if(currentState == idOrganism) {
				
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
							
			} /*else*/ if(currentState == pubmedIDs) {
				
				if(inputLine.substr(0,3) != "CC ") {
					
					if(pubmedIdentifier == "RX " && inputLine.substr(0,3) == "RC " ) {
						RC = "";
						
						if(inputLine.find('=',0) != string::npos) {
							RC = inputLine.substr(inputLine.find_first_of('=')+1,string::npos);
							if(RC.find(";",0) != string::npos ) {
								RC.replace(RC.find_last_of(';'),1,"");	
							}
						}
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
				
			}  
			if(currentState == readLocalization) {
				
				bool noLocEntrydone = false;
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
							} else {
								cout << "Warning: OrgMap doesn't contain " << entry.getOrgId() << endl;
							}
							continue;
						}
						if(token.find("(By similarity)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
							
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							} else {
								cout << "Warning: OrgMap doesn't contain " << entry.getOrgId() << endl;
							}
							continue;
						}
						if(token.find("(Possible)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
	
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							} else {
								cout << "Warning: OrgMap doesn't contain " << entry.getOrgId() << endl;
							}						
							continue;
						}
						if(token.find("(Potential)",0) != string::npos ) {
							entry.addLocation(token);
							entry.incrementNonExpLoc();
	
							if(orgMap.count(entry.getOrgId()) == 1) {
								orgSummary[orgMap[entry.getOrgId()]].incNonExpLoc();
							} else {
								cout << "Warning: OrgMap doesn't contain " << entry.getOrgId() << endl;
							}
							continue;
						}
						
						// we have an experimental subcellular location
						entry.addLocation(token);
						entry.incrementExpLoc();
						
						if(orgMap.count(entry.getOrgId()) == 1) {
							orgSummary[orgMap[entry.getOrgId()]].incExpLoc();
						} else {
							cout << "Warning: OrgMap doesn't contain " << entry.getOrgId() << endl;
						}
					}
					
					// write to output file
					if(entry.getExpLoc() != 0 || entry.getNonExpLoc() != 0) {

						//write pubmedId's
						outputFilet1 << entry.getPubMedEntries();
						// write Table 2
						outputFile << entry.toString() << endl;
		
					} else {
						// CC -!- SUBCE.. section is present but no entry
						if(orgMap.count(entry.getOrgId()) == 1) {
							orgSummary[orgMap[entry.getOrgId()]].incNoLoc();
							noLocEntrydone = true;
						}											
					}
					entry.clear();
				}
				
				if(inputLine.substr(0,3) != "CC ") {
					// could not find subcellular localization for this protein
					currentState = checkID; // back to searching for ID
				
					if(!noLocEntrydone) {
						if(orgMap.count(entry.getOrgId()) == 1) { 
							// CC -!- not found
							orgSummary[orgMap[entry.getOrgId()]].incNoLoc();
						}
					} else {
						cout << "Warning: Making an entry twice for no location !" << endl;
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
	
	// check for completeness
//	ifstream referenceFile("mammals_list.dat");
//	if(! referenceFile.is_open()) {
//		cout << "Error opening reference file" << endl;
//	}
//	string inputLine;
//	set<string> listProteins;
//	cout << "Following proteins are not found !" << endl;
//	while( getline(referenceFile,inputLine) ) {
//		if(listProteins.count(inputLine) != 1) {
//			listProteins.insert(inputLine);
//		} else {
//			cout << "Duplicate Entry in list: " << inputLine << endl;
//		}
//			
//		if(HumanProteins.count(inputLine) != 1) {
//			cout << inputLine << endl;
//		}
//	}	
//	referenceFile.close();
		

	outputFilet1.close();
	outputFile.close();
	outputFileSum.close();
	
	return 0;
}

string processLine(string inputLine) {
	string newLine;
	while(inputLine.find("  ",0) != string::npos ) {
		inputLine.replace(inputLine.find("  ",0),2," ");
	}
	
	newLine.append(inputLine);
	return newLine;
}

void countPotentialProteins( string filename ) {
	
//	set<string> uniprotProteins;
//	
//	ifstream referenceFile("human_list.dat");
//	if(!referenceFile.is_open()) {
//		cout << "Error opening file !" << endl;
//	}
//	string newLine;
//	while(getline(referenceFile, newLine)) {
//		uniprotProteins.insert(newLine);
//	}
//	
//	cout << "The size of proteins from Uniprot is : " << uniprotProteins.size() << endl;	
//	
//	
//	referenceFile.close();
	
	ifstream inputFile(filename.data());
	if(!inputFile.is_open())
	{
		cout << "Error opening input file " << filename << " !" << endl << endl;
	}
	string inputLine;
	
	string identifierID = "AC ";
	string identifierLoc = "CC -!- SUBCELLULAR LOCATION";
	string identifierLocRed = "CC -!- SUBCELLULAR LOCATION: Note";
	string checkIdent = identifierID;
	
	
	int noProteins = 0;
	bool idFound = false;
	bool locFound = false;
	string ac;
	while(getline(inputFile,inputLine)){
		inputLine = processLine(inputLine);
		
		if(inputLine.substr(0,3) == identifierID && checkIdent == identifierLoc) {
			// this means that subcellular location is not found for this particular ID
			checkIdent = identifierID;
		}		
		
		if(inputLine.substr(0,checkIdent.length()) == checkIdent ) {
			if(checkIdent == identifierID) {
				std::vector<string> strs;
				boost::split(strs, inputLine, boost::is_any_of(" "));
				ac = strs[1];				
				ac = ac.replace(ac.find_first_of(';'),1,"");
				
				while(true) {
					getline(inputFile, inputLine);
					if( inputLine.substr(0,checkIdent.length()) != checkIdent ) {
						break;
					}
				}
				checkIdent = identifierLoc;
			} else if(checkIdent == identifierLoc) {
				checkIdent = identifierID;
				
				if( inputLine.substr(0, identifierLocRed.length()) != identifierLocRed) {
					noProteins ++;	
				}				
//				if(uniprotProteins.count(ac) != 1) {
//					cout << "New Protein ?:" << ac << endl;
//				}
			}
	}
		}
	
	// Print results
	cout << endl << "Total No of potential proteins found in the file " << filename << ": " << noProteins << endl;
	
	inputFile.close();
}

int main(int argc, char ** argv) {

	countPotentialProteins("uniprot_sprot_archaea.dat");
	parseSprot(argc, argv);
	return 0;
}