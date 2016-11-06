//
// Created by Qianying Ji on 10/10/16.
//

#include "gate.h"
#include <string>
using namespace std;

gate::gate()
{
    gateName = "init";
    criticalPathPtr = NULL;
}

gate::gate(string name)
{
    gateName = name;
}

//gate::gate(string name, string type, vector<string> fanin) {
//    gateName = name;
//    gateType = type;
//    fanInGates = fanin;
//}

gate::gate(string name, string type) {
    gateName = name;
    gateType = type;
}

string gate::getGateName(){
    return gateName;
}

void gate::setTypeName(string name)
{
    gateType=name;
}

string gate::getTypeName()
{
    return gateType;
}

vector<gate*>* gate::getFainPtrs()
{
    return &fanInGates;
}

vector<gate*>* gate::getFanoutPtrs()
{
    return &fanOutGates;
}

void gate::setCriticalPtr(gate* ptr)
{
    criticalPathPtr = ptr;
}

gate* gate::getCriticalPtr() {
    return criticalPathPtr;
}

void gate::setaOut(double aout){
    aOut = aout;
}

void gate::setTauOut(double tauout){
    tauOut = tauout;
}

void gate::setCin(double cin){
    cIn = cin;
}

double gate::getCin(){
    return cIn;
}


double gate::getCout(){
    return cOut;
}

void gate::setCout(double cout){
    cOut = cout;
}

void gate::incrFaninNumber() {
    faninNumber++;
}

int gate::getFaninNumber() {
    return faninNumber;
}

void gate::incrCompletedFaninNumber() {
    completeFaninNum += 1;
}

int gate::getCompletedFaninNumber() {
    return completeFaninNum;
}

void gate::incrFanoutNumber(){
    fanoutNumber++;
}

int gate::getFanoutNumber(){
    return fanoutNumber;
}

double gate::getAout(){
    return aOut;
}

double gate::getTauOut() {
    return tauOut;
}

void gate::setTauIn(double in) {
    tauIn = in;
}

void gate::setaIn(double ain) {
    aIn = ain;
}

void gate::setD(double dd) {
    d = dd;
}
