/*********************************************************************
*
* WT Data Logger
* Created by: Hesham Ebrahim
*
*
*********************************************************************/
#include <ctime>
#include <windows.h>
#include <stdio.h>
#include <NIDAQmx.h>
#include <stdlib.h>
#include <iostream>
#include <Interface.h>
#include <Startup.h>
#include <fstream>
#include <string>
#include <serialClass.h>
#include <time.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>
using namespace std;

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else


int main(void)
{
	Startup start;
    cout << "*******************|WT dataLogger mk1|*******************" << '\n' << '\n';

	cout << "Select an Option:" << endl;
	cout << "   1. Manual Settings" << endl;
	cout << "   2. Configuration File" << endl;
	cout << "   3. Arduino Distance Measurement" << endl;
	cout << "   4. Multiple Devices" << endl;
	cout << "   5. Drive Stepper Motors" << endl;
	cout << "   6. Exit" << endl;

	int setting;
	cin >> setting;
	if (setting == 1)
    {
        start.config();
    }

    else if (setting == 2)
    {
        start.file();
    }

    else if (setting == 3)
    {
        start.distance();
    }

    else if (setting == 4)
    {
        start.multiDev();
    }

    else if (setting == 5)
    {
        start.digitalOut();
    }

    else if (setting == 6)
    {
        exit(1);
    }

    else if (setting == 0)
    {
        start.ESP();
    }
}
