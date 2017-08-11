#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Interface.h>
#include <sstream>
#include <Startup.h>
#include <typeinfo>
#include <fstream>
#include <iomanip>

void Interface::setSampleRate()
{
    sampleRate = getSamplePerChan()*getNumofChan();
    printf("Total Sample Rate: ");
    std::cout << sampleRate << endl;
}

void Interface::setSamplePerChan()
{
    printf("Samples Per Channel (Sample Rate): ");
    std::cin >> samplePerChan;
}

void Interface::setNumofChan()
{
    printf("Number of Channels to Sample: ");
    std::cin >> numofChan;
}

void Interface::setsamplingMode()
{
    cout << "Sample Mode: " << endl;
    cout << "1. Finite Samples  " << "2. Continuous Samples" << endl;
    cin >> sampleMode;


    if (sampleMode==1)
    {
        sampleMode = 10178;
    }

    else if (sampleMode==2)
    {
        sampleMode = 10123;
    }
}

void Interface::setPhysChan()
{
    cout << "Channels Range: ";
    cin >> physChan;

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
}

void Interface::setSampleInterval()
{
    printf("Sample Interval(s): ");
    std::cin >> sampleInterval;

    sampleInterval = getSampleRate()/sampleInterval;
    cout << "Sampling Frequency (s): " << (1/sampleInterval) << endl;
}

void Interface::setIteration()
{
    printf("Set Number of Iterations: ");
    cin >> Iteration;
}

int Interface::getSampleRate()
{
    return sampleRate;
}

int Interface::getSamplePerChan()
{
    return samplePerChan;
}

int Interface::getNumofChan()
{
    return numofChan;
}

int Interface::getSamplingMode()
{
    return sampleMode;
}

const char* Interface::getPhysChan()
{
    return strPhysChan;
}

float Interface::getSamplingFreq()
{
    return sampleInterval;
}

double *Interface::getDataArray()
{
    size = (getNumofChan()+1)*getSamplePerChan();
    double dataArray[size];
    return dataArray;
}

int Interface::getIteration()
{
    return Iteration;
}


/*********************************************/
	// Traverse Controls
/*********************************************/
void Interface::setxDrive()
{
    printf("1. Drive x1 & x2\n") ;
    printf("2. Drive x1     \n");
    printf("3. Drive x2     \n");
    cin >> xDrive;

    if      (xDrive=="1")
    {
        xDrive = "Dev1/port0/line4:5";
        strxDrive = xDrive.c_str();
        x1Direction = "Dev1/port0/line6:7";
        strx1Direction = x1Direction.c_str();

        setSamplePerChanToX1();
        setSamplePerChanToX2();
        setx1();
        setx2();

/*********************************************/
//x1 configuration
/*********************************************/
        if (getSamplePerChanToX1()>=0)
            {
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                // traverse limit
                if (x1> 38000 || x1<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                voltonx1 = 1;
            }

        else if (getSamplePerChanToX1()<0)
            {
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                //traverse limit
                if (x1> 38000 || x1<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                numOfRevsx1 = 0-numOfRevsx1;
                voltonx1 = 2;
            }

/*********************************************/
//x2 configuration
/*********************************************/
        if (getSamplePerChanToX2()>=0)
            {
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

                // traverse limit condition
                if (x2> 38000 || x2<0)
                {
                    cout << "x2 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                voltonx2 = 1;

            }

        else if (getSamplePerChanToX2()<0)
            {
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

                if (x2> 38000 || x2<0)
                {
                    cout << "x2 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                numOfRevsx2 = 0-numOfRevsx2;
                voltonx2 = 2;
            }

    }

    else if (xDrive=="2")
    {
        xDrive = "Dev1/port0/line4";
        strxDrive = xDrive.c_str();
        x1Direction = "Dev1/port0/line6";
        strx1Direction = x1Direction.c_str();

        setSamplePerChanToX1();
        setx1();

        if (getSamplePerChanToX1()>=0)
            {
                // calculates new x1
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                // traverse limit condition
                if (x1> 38000 || x1<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction of travel based on sign
                voltonx1 = 1;
                cout << '\n';
            }
        else if (getSamplePerChanToX1()<0)
            {
                // calculates new x1
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                // traverse limit condition
                if (x1> 38000 || x1<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction of travel based on sign
                numOfRevsx1 = 0-numOfRevsx1;
                voltonx1 = 2;
                cout << '\n';
            }

    }

    else if (xDrive=="3")
    {
        xDrive = "Dev1/port0/line5";
        strxDrive = xDrive.c_str();
        x2Direction = "Dev1/port0/line7";
        strx2Direction = x2Direction.c_str();

        setSamplePerChanToX2();
        setx2();

        if (getSamplePerChanToX2()>=0)
            {
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

                // traverse limit condition
                if (x2> 38000 || x2<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                voltonx2 = 1;
            }

        else if (getSamplePerChanToX2()<0)
            {
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

                if (x2> 38000 || x2<0)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         0 < x < +38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                numOfRevsx2 = 0-numOfRevsx2;
                voltonx2 = 2;
            }
    }

}

void Interface::setyDrive()
{
     yDrive = "Dev1/port1/line7";
     stryDrive = yDrive.c_str();
     yDirection = "Dev1/port1/line6";
     stryDirection = yDirection.c_str();

     setSamplePerChanToY();
     sety();

     if (getSamplePerChanToY()>=0)
            {
                y = y+getSamplePerChanToY();
                cout << "   new y = " << y << " Pulses" << endl;

                // traverse limit
                if (y> 33000 || y<0)
                {
                    cout << "y traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 33000 (i.e. 82.5revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                voltony = 1;
            }

    else if (getSamplePerChanToY()<0)
            {
                y = y+getSamplePerChanToY();
                cout << "   new y = " << y << " Pulses" << endl;

                //traverse limit
                if (y>= 33000 || y<0)
                {
                    cout << "y traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 33000 (i.e. 82.5revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                numOfRevsy = 0-numOfRevsy;
                voltony = 2;
            }
}

void Interface::setzDrive()
{
//     zDrive = "Dev2/port1/line1";
     strzDrive = zDrive.c_str();
     zDirection = "Dev2/port1/line3";
     strzDirection = zDirection.c_str();

     setSamplePerChanToZ();
     setz();

     if (getSamplePerChanToZ()>=0)
        {
            z = z+getSamplePerChanToZ();
            cout << "   new z1 = " << z << " Pulses" << endl;

            // traverse limit
            if (z>= 37600 || z<=-37600)
            {
                cout << "The traverse limit is exceeded" << '\n';
                cout << "Max number of pulses is:         +/- 37600 (i.e. 95revs)" << '\n' << '\n';
                cout << "Check or reduce the Revs" << '\n';
                cout << "---- Terminating ----" << '\n';
                exit(1);
            }

            ofstream out ("z1.txt");
            out << z;
            out.close();
            voltonz = 1;
            cout << "   Rotation in Clock-wise Direction..." << '\n';
        }
    else if (getSamplePerChanToZ()<0)
        {
            z = z+getSamplePerChanToZ();
            cout << "   new z1 = " << z << " Pulses" << endl;

            //traverse limit
            if (z>= 37600 || z<=-37600)
            {
                cout << "The traverse limit is exceeded" << '\n';
                cout << "Max number of pulses is:         +/- 37600 (i.e. 95revs)" << '\n' << '\n';
                cout << "Check or reduce the Revs" << '\n';
                cout << "---- Terminating ----" << '\n';
                exit(1);
            }

            ofstream out ("z1.txt");
            out << z;
            out.close();
            numOfRevsz = 0-numOfRevsz;
            voltonz = 2;
            cout << "   Rotation in Anti-clockwise Direction..." << '\n';
        }
}

void Interface::setxAutoDrive()
{
    xDrive = "Dev1/port1/line0:1";
    strxDrive = xDrive.c_str();
    x1Direction = "Dev1/port1/line2";
    strx1Direction = x1Direction.c_str();
    x2Direction = "Dev1/port1/line3";
    strx2Direction = x2Direction.c_str();

    setx1();
    setx2();

/*********************************************/
//x1 configuration
/*********************************************/
        if (getSamplePerChanToX1()>=0)
            {
                // calculates new x1
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                // traverse limit
               if (x1<0 || x1>38000)// || x1<=-17200)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction based on sign
                voltonx1 = 1;

            }

        else if (getSamplePerChanToX1()<0)
            {
                // calculates new x1
                x1 = x1+getSamplePerChanToX1();
                cout << "   new x1 = " << x1 << " Pulses" << endl;

                //traverse limit
               if (x1<0 || x1>38000)// || x1<=-17200)
                {
                    cout << "x1 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction based on sign
                numOfRevsx1 = 0-numOfRevsx1;
                voltonx1 = 2;
            }

/*********************************************/
//x2 configuration
/*********************************************/
        if (getSamplePerChanToX2()>=0)
            {
                // calculates new x2
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

                // traverse limit condition
                if (x2<0 || x2>38000)// || x1<=-17200)
                {
                    cout << "x2 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction based on sign
                voltonx2 = 1;
            }

        else if (getSamplePerChanToX2()<0)
            {
                // calculates new x2
                x2 = x2+getSamplePerChanToX2();
                cout << "   new x2 = " << x2 << " Pulses" << endl;

               if (x2<0 || x2>38000)// || x1<=-17200)
                {
                    cout << "x2 traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 38000 (i.e. 95revs)" << '\n' << '\n';
                    cout << "---- Terminating ----" << '\n';
                    exit(1);
                }

                // sets direction based on sign
                numOfRevsx2 = 0-numOfRevsx2;
                voltonx2 = 2;
            }

}

void Interface::setyAutoDrive()
{
    yDrive = "Dev2/port1/line0";
    stryDrive = yDrive.c_str();
    yDirection = "Dev2/port1/line2";
    stryDirection = yDirection.c_str();

    sety();

    if (getSamplePerChanToY()>=0)
            {
                y = y+getSamplePerChanToY();
                cout << "   new y = " << y << " Pulses" << endl;

                // traverse limit
                if (y> 33000 || y<0)
                {
                    cout << "y traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 33000 (i.e. 82.5revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                voltony = 1;

            }

    else if (getSamplePerChanToY()<0)
            {
                y = y+getSamplePerChanToY();
                cout << "   new y = " << y << " Pulses" << endl;

                //traverse limit
                if (y> 33000 || y<0)
                {
                    cout << "y traverse limit is exceeded" << '\n';
                    cout << "Max number of pulses is:         +/- 33000 (i.e. 82.5revs)" << '\n' << '\n';
                    cout << "---- Software Terminating ----" << '\n';
                    exit(1);
                }

                // set direction
                numOfRevsy = 0-numOfRevsy;
                voltony = 2;
            }
}

void Interface::setzAutoDrive()
{
//    zDrive = "Dev2/port1/line1";
    strzDrive = zDrive.c_str();
    zDirection = "Dev2/port1/line3";
    strzDirection = zDirection.c_str();

    setz();

    if (getSamplePerChanToZ()>=0)
        {
            z = z+getSamplePerChanToZ();
            cout << "   new z1 = " << z << " Pulses" << endl;

            // traverse limit
            if (z>= 37600 || z<=-37600)
            {
                cout << "The traverse limit is exceeded" << '\n';
                cout << "Max number of pulses is:         +/- 37600 (i.e. 95revs)" << '\n' << '\n';
                cout << "Check or reduce the Revs" << '\n';
                cout << "---- Terminating ----" << '\n';
                exit(1);
            }

            ofstream out ("z1.txt");
            out << z;
            out.close();
            voltonz = 1;
            cout << "   Rotation in Clock-wise Direction..." << '\n';
        }

    else if (getSamplePerChanToZ()<0)
        {
            z = z+getSamplePerChanToZ();
            cout << "   new z1 = " << z << " Pulses" << endl;

            //traverse limit
            if (z>= 37600 || z<=-37600)
            {
                cout << "The traverse limit is exceeded" << '\n';
                cout << "Max number of pulses is:         +/- 37600 (i.e. 95revs)" << '\n' << '\n';
                cout << "Check or reduce the Revs" << '\n';
                cout << "---- Terminating ----" << '\n';
                exit(1);
            }

            ofstream out ("z1.txt");
            out << z;
            out.close();
            numOfRevsz = 0-numOfRevsz;
            voltonz = 2;
            cout << "   Rotation in Anti-clockwise Direction..." << '\n';
        }
}

void Interface::setSamplePerChanToX1()
{
    cout << "Set number of revs in x1: ";
    cin >> numOfRevsx1;

    numOfRevsx1 = numOfRevsx1 * 400; // 400 is the pulse per rev setting of the Microstep drive

    cout << "Number of Pulses Calculated for x: " << numOfRevsx1 << " Pulses" << endl;
}

void Interface::setSamplePerChanToX2()
{
    cout << "Set number of revs in x2: ";
    cin >> numOfRevsx2;

    numOfRevsx2 = numOfRevsx2 * 400; // 400 is the pulse per rev setting of the Microstep drive

    cout << "Number of Pulses Calculated for x: " << numOfRevsx2 << " Pulses" << endl;
}

void Interface::setSamplePerChanToY()
{
    cout << "Set number of revs in y: ";
    cin >> numOfRevsy;

    numOfRevsy = numOfRevsy * 400;

    cout << "Number of Pulses Calculated for y: " << numOfRevsy << " Pulses" << endl;
}

void Interface::setSamplePerChanToZ()
{
    cout << "Set number of revs in z: ";
    cin >> numOfRevsz;

    numOfRevsz = numOfRevsz * 400;

    cout << "Number of Pulses Calculated for z: " << numOfRevsz << " Pulses" << endl;
}

void Interface::setx1()
{
    ifstream myfile ("x1.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,linex1) )
            {
                cout << "   imported x1 = " << linex1 << " Pulses" << '\n';
            }
    }

    istringstream (linex1) >> x1; // converts string to int
    istringstream (linex1) >> x1Count;
}

void Interface::setx2()
{
    ifstream myfile ("x2.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,linex2) )
            {
                cout << "   imported x2 = " << linex2 << " Pulses" << '\n';
            }
    }

    istringstream (linex2) >> x2; // converts string to int
    istringstream (linex2) >> x2Count; // converts string to int
}

void Interface::sety()
{
    ifstream myfile ("y.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,liney) )
            {
                cout << "   imported y = " << liney << " Pulses" << '\n';
            }
    }

    istringstream (liney) >> y; // converts string to int
    istringstream (liney) >> yCount;
}

void Interface::setz()
{
    ifstream myfile ("z.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,linez) )
            {
                cout << "   imported z = " << linez << " Pulses" << '\n';
            }
    }

    istringstream (linez) >> z; // converts string to int
}

void Interface::setautoMotion()
{

    ifstream motion;
	motion.open("autoMotion.txt");
	if (!motion)
	{
		cout << "Unable to open file Config.txt";
		cout << endl;
		exit(1);
	}

    //Reading the data
    cout << '\n' << endl;
    while (motion.good())
    {
        while(getline(motion, lineA))
        {
            istringstream streamA(lineA);
            colA = 0;
            while(streamA >> counter)
            {
                arrayA[rowA][colA] = counter;
                colA++;

            }
            rowA++;
        }
    }

    //Display data
    cout << "# of Rows --->" << rowA << '\n';
    cout << "# of Columns --->" << colA << '\n';
    cout << '\n' << endl;
    for (int i =0; i<rowA; i++)
    {
        for (int j =0; j<colA; j++)
        {
            cout << setw(7) << arrayA[i][j] << " ";
        }
        cout << '\n';
    }

}

void Interface::resetPosX()
{

    setx1();
    setx2();

    // reset x1 and x2
    if      (x1==0 || x2==0)
    {
        cout << "The x-axis is already at home setting... " << endl;
        cout << '\n' << endl;
    }
    else if (x1>0)
    {
        x1_new = x1+35200;
        numOfRevsx1 = x1_new;
        cout << "new num of Revs: " << numOfRevsx1 << endl;
        voltonx1 = 2;
        x1 = x1_new-numOfRevsx1;
        cout << "   new x1 = " << x1 << " Pulses" << endl;
        resetX=1;
    }
    else if (x2>0)
    {
        cout << "x2_before: " << x2 << endl;
        x2_new = x2;
        cout << "x2_new: " << x2_new << endl;
        numOfRevsx2 = x2_new;
        cout << "new num of Revs: " << numOfRevsx2 << endl;
        voltonx2 = 2;
        x2 = x2_new-numOfRevsx2;
        cout << "   new x2 = " << x2 << " Pulses" << endl;
        resetX=1;
    }

    xDrive = "Dev1/port1/line0:1";
    strxDrive = xDrive.c_str();
    x1Direction = "Dev1/port1/line2:3";
    strx1Direction = x1Direction.c_str();

}

void Interface::resetPosY()
{
    sety();

    // reset x1 and x2
    if      (y==0)
    {
        cout << "The y-axis is already at home setting... " << endl;
        cout << '\n' << endl;
    }
    else if (y>0)
    {
        y_new = y+35200;
        numOfRevsy = y_new;
        cout << "new num of Revs: " << numOfRevsy << endl;
        voltony = 2;
        y = y_new-numOfRevsy;
        cout << "   new y = " << y << " Pulses" << endl;
        resetY=1;
    }

    yDrive = "Dev2/port1/line0";
    stryDrive = yDrive.c_str();
    yDirection = "Dev2/port1/line2";
    stryDirection = yDirection.c_str();

}

int Interface::getresetX()
{
    return resetX;
}

int Interface::getresetY()
{
    return resetY;
}

const char* Interface::getxDrive()
{
    return strxDrive;
}

const char* Interface::getyDrive()
{
    return stryDrive;
}

const char* Interface::getzDrive()
{
    return strzDrive;
}

string      Interface::getxDrivectr()
{
    return xDrive;
}

string      Interface::getyDrivectr()
{
    return yDrive;
}

const char* Interface::getx1Direction()
{
    return strx1Direction;
}

const char* Interface::getx2Direction()
{
    return strx2Direction;
}

const char* Interface::getyDirection()
{
    return stryDirection;
}

const char* Interface::getzDirection()
{
    return strzDirection;
}

double Interface::getSamplePerChanToX1()
{
    return numOfRevsx1;
}

double Interface::getSamplePerChanToX2()
{
    return numOfRevsx2;
}

double Interface::getSamplePerChanToY()
{
    return numOfRevsy;
}

double Interface::getSamplePerChanToZ()
{
    return numOfRevsz;
}

long Interface::getvoltonx1()
{
    return voltonx1;
}

long Interface::getvoltonx2()
{
    return voltonx2;
}

long Interface::getvoltony()
{
    return voltony;
}

long Interface::getvoltonz()
{
    return voltonz;
}

// delete this if not used!
void Interface::setupInfo()
{
    std::cout << "Sample Rate: " << sampleRate << endl;
    std::cout << "Samples Per Channel: " << samplePerChan << endl;

    // have a command to display all the selected data if incorrect choose to go back and edit them.
}
