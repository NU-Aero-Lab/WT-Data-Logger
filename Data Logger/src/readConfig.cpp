#include "readConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Interface.h>
#include <sstream>
#include <Startup.h>
#include <typeinfo>
#include <fstream>
#include <string>
using namespace std;

void readConfig::setAllConfig()
{

    ifstream myFile;
	myFile.open("Config.txt");
	if (!myFile)
	{
		cout << "Unable to open file Config.txt";
		cout << endl;
		exit(1);
	}

    // set the input variables to the corresponding "set variables" respectively
	myFile >> numofChan >> samplePerChan >> sampleInterval >> sampleMode >> physChan >> Iteration;

    sampleRate = getSamplePerChan()*getNumofChan();

    sampleInterval = getSampleRate()/sampleInterval;

    if (sampleMode==1)
    {
        sampleMode = 10178;
    }

    else if (sampleMode==2)
    {
        sampleMode = 10123;
    }

    if (physChan=="0")
    {
        physChan = "Dev1/ai0";
        strPhysChan = physChan.c_str(); // converts from string to const char* []
    }
    else
    {
        physChan = "Dev1/ai" + physChan;
        strPhysChan = physChan.c_str();
    }

    cout << "Number of Channels: " << numofChan << endl;
    cout << "Sample Per Channel: " << samplePerChan << endl;
    cout << "Total Number of Samples: " << numofChan*samplePerChan << endl;
    cout << "Sample Interval (s): " << 1/sampleInterval << endl;
    cout << "Sample Mode: " << sampleMode << endl;
    cout << "Channels Range: " << physChan << endl;
    cout << "Iterations: " << Iteration << endl;

}

int readConfig::getSampleRate()
{
    return sampleRate;
}

int readConfig::getSamplePerChan()
{
    return samplePerChan;
}

int readConfig::getNumofChan()
{
    return numofChan;
}

int readConfig::getSamplingMode()
{
    return sampleMode;
}

const char* readConfig::getPhysChan()
{
    return strPhysChan;
}

float readConfig::getSamplingFreq()
{
    return sampleInterval;
}

double *readConfig::getDataArray()
{
    size = (getNumofChan()+1)*getSamplePerChan();
    double dataArray[size];
    return dataArray;
}

int readConfig::getIteration()
{
    return Iteration;
}

void readConfig::setupInfo()
{
    std::cout << "Sample Rate: " << sampleRate << endl;
    std::cout << "Samples Per Channel: " << samplePerChan << endl;

    // have a command to display all the selected data if incorrect choose to go back and edit them.
}

