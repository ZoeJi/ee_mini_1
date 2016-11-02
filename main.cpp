#include <iostream>
#include <fstream>
#include "gate.h"
#include "gateList.h"
#include <list>
#include "gateType.h"

using namespace std;

/*
 * getIOname is used to extract the name of primary input or output
 */
void write_result(int inputCount, int outputCount, string fileName, gateList* myGateList);
string getIOname(string str);
string readGateType(string str);
string readGateName(string str);
vector<string> readGateFanin(string str);
gate* searchMetGates(string name, list<gate*>* temp);



int main(int argc, char *argv[]) {

    string aline;
    string gateName;
    string thisGateType;
    gateType* newGateType;
    gate* newGate;
    gate* fanoutGate;
    gate* outputGate;
    gateList myGateList;
    list<gate*> temp;
    vector<string> faninVect;
    vector<string>::iterator j;

    int inputCount = 0, outputCount = 0;
    ifstream fileInput ( argv[2] );

    // init myGateList
    newGate = new gate("OUTP", "");
    myGateList.getGatelist()->push_back(newGate);

    if ( argc != 3 ) // argc should be 3 for correct execution
        cout<<"usage: "<< argv[0] <<" <read command> <filename>\n <read command> options: read_ckt, read_nldm\n";
    else {
        if(strncmp(argv[1],"read_ckt", 9) == 0){
            if ( !fileInput.is_open() )
                cout<<"Could not open file\n";
            else {
                while(!fileInput.eof()) {
                    getline(fileInput,aline);
                    if(isalpha(aline[0]) || (aline[0]>='0' && aline[0]<= '9')) {
                        if(!aline.empty()) {
                            if(aline[0]!='#') {
                                // inputs
                                if(aline.substr(0,5)=="INPUT") {
                                    inputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "INP");
                                    myGateList.getGatelist()->push_back(newGate);
                                }
                                // outputs
                                else if(aline.substr(0,6)=="OUTPUT") {
                                    outputCount++;
                                    gateName = getIOname(aline);
                                    newGate = new gate(gateName, "OUTP");
                                    myGateList.getGatelist()->push_back(newGate);
                                }
                                // other logical gates
                                else {
                                    // gate type count
                                    thisGateType = readGateType(aline);
                                    newGateType = myGateList.searchGateType(thisGateType);
                                    if(newGateType == NULL) {
                                        myGateList.getTypeList()->push_back(new gateType(thisGateType));
                                        newGateType = myGateList.searchGateType(thisGateType);
                                        newGateType->typeCountIncr();
                                    }
                                    else {
                                        newGateType->typeCountIncr();
                                    }

                                    // gate fanout, fanin
                                    gateName = readGateName(aline);
                                    thisGateType = readGateType(aline);
                                    faninVect = readGateFanin(aline);

//                                    newGate = searchMetGates(gateName, &temp);

                                    // if this gate did not meet before
                                    if(newGate == NULL) {
                                        newGate = new gate(gateName, thisGateType, faninVect);
                                        newGate->setTypeName(thisGateType);
                                        myGateList.getGatelist()->push_back(newGate);
                                    }
                                    else{
                                    // met this gate before
//                                        myGateList.getGatelist()->push_back(newGate);
                                        newGate = new gate(gateName, thisGateType, faninVect);
                                        newGate->addFanoutGates("OUTP");
                                        myGateList.getGatelist()->push_back(newGate);
                                    }

//                                    // handle fanout
//                                    for( j = faninVect.begin(); j != faninVect.end(); j++){
//                                        fanoutGate = myGateList.searchGate((*j));
//                                        fanoutGate->addFanoutGates(gateName);
//                                    }

                                    // if this gate has meet before but is output gate
//                                    else {
//                                        if(newGate->getTypeName() == "output"){
//                                            myGateList.getGatelist()->push_back(newGate);
//                                            newGate = new gate();
//
//                                        }
//                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
        else if(strncmp(argv[1],"read_nldm", 9) == 0){

        }

        write_result(inputCount, outputCount, argv[2], &myGateList);
    }
}

void write_result(int inputCount, int outputCount, string fileName, gateList* myGateList){
    string resultFile = fileName + ".out";
    ofstream result;
    list<gateType*>::iterator k;
    list<gate*>::iterator j;
    vector<string>::iterator i;
    gate* thisGate;

    result.open(resultFile.c_str());

    result << "------------------------------------------------" << endl;
    result << inputCount << " primary inputs" << endl;
    result << outputCount << " primary outputs"<< endl;
    for( k = myGateList->getTypeList()->begin(); k != myGateList->getTypeList()->end(); k++) {
        result<<(*k)->getTypeCount() << ' ' << (*k)->getTypeName()<< ' ' << "gates"<< endl;
    }

    result << endl;
    result << "Fanout..." << endl;

//    for( j = myGateList->getGatelist()->begin(); j != myGateList->getGatelist()->end(); j++){
//        if((*j)->getTypeName() != "INP" && (*j)->getTypeName() != "OUTP" && (*j)->getTypeName() != ""){
//            result << (*j)->getTypeName() << "-" << (*j)->getGateName() << ": ";
//            for(i = (*j)->getFanoutVector().begin(); i != (*j)->getFanoutVector().end(); i++){
//                thisGate = myGateList->searchGate((*i));
//
//                result << thisGate->getTypeName() << "-" << (*i) << ", ";
//            }
//            result << endl;
//        }
//    }

    result << endl;
    result << "Fanin..." << endl;

    for( j = myGateList->getGatelist()->begin(); j != myGateList->getGatelist()->end(); j++){
        if((*j)->getTypeName() != "INP" && (*j)->getTypeName() != "OUTP" && (*j)->getTypeName() != ""){
            result << (*j)->getTypeName() << "-" << (*j)->getGateName() << ": ";
            for(i = (*j)->getFaninVector().begin(); i != (*j)->getFaninVector().end(); i++){
                if(myGateList->searchGate((*i)) == NULL){
                    result << endl << "NULLLLLLLLLLLLLLLLLLLLLLLLLLL" << endl;
                }
                else {

//                    result << thisGate->getTypeName() << "-" << (*i) << ", ";
                }
            }
            result << endl;
        }
    }

    result << "------------------------------------------------" << endl;
    result.close();
    cout << "Result is output to the file: " << resultFile << endl;
}

string getIOname(string str) {
    int head,tail;
    head=str.find('(')+1;
    tail=str.find(')');
    tail=tail-head;
    str =str.substr(head,tail);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

string readGateType(string str)
{
    int head,tail;
    head=str.find('=')+1;
    tail=str.find('(');
    tail=tail-head;
    str =str.substr(head,tail);
    /*discards spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;
}

string readGateName(string str)
{
    int a;
    a=str.find('=');
    str=str.substr(0,a);
    /*discard spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    return str;

}

vector<string> readGateFanin(string str)
{
    int a;
    string subStr;
    vector<string> vector1;
    str=getIOname(str);
    while(str.find(',')!=-1)
    {
        a=str.find(',');
        subStr=str.substr(0,a);
        /*discards spaces at the head of the string*/
        while(*(subStr.begin())==' ')
        {
            subStr.erase(subStr.begin());
        }
        /*discards spaces at the tail of the string*/
        while(*(subStr.rbegin())==' ')
        {
            subStr=subStr.substr(0,subStr.size()-1);
        }
        vector1.push_back(subStr);
        str.erase(0,a+1);
    }
    /*discards spaces at the head of the string*/
    while(*(str.begin())==' ')
    {
        str.erase(str.begin());
    }
    /*discards spaces at the tail of the string*/
    while(*(str.rbegin())==' ')
    {
        str=str.substr(0,str.size()-1);
    }
    vector1.push_back(str);
    return vector1;
}

gate* searchMetGates(string name, list<gate*>* temp)
{
    list<gate*>::iterator i;
    gate* remove;
    for(i=temp->begin();i!=temp->end();i++)
    {
        if((*i)->getGateName() == name)
        {
            remove=*i;
            temp->erase(i);
            return remove;
        }
    }
    return NULL;
}