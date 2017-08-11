#include <Interface.h>
#include <readConfig.h>
#include <NIDAQmx.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>8
#include <string>
#include <serialClass.h>
#include <windows.h>
#include <iomanip>
#include <bitset>

using namespace std;
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

void Startup::config()
{
    int32       error=0;
	TaskHandle  taskHandle=0;

    Interface commands;
    commands.setNumofChan();
	commands.setSamplePerChan();
	commands.setSampleRate();
	commands.setSampleInterval();
	commands.setsamplingMode();
	commands.setPhysChan();
	commands.setIteration();

    /*********************************************/
	// DAQmx Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle)); //creates a task
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,commands.getPhysChan(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL)); // create channel(s) to measure RMS voltage
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan())); // a digital edge produces each sample

    /*********************************************/
	// DAQmx TDMS Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxConfigureLogging(taskHandle,"C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\Log_1.tdms",DAQmx_Val_LogAndRead,"Voltage",DAQmx_Val_OpenOrCreate));

    /*********************************************/
	// DAQmx Start Code
	/*********************************************/
    DAQmxErrChk (DAQmxStartTask(taskHandle)); //starts the created task

    printf("Logging samples continuously. Press Enter to interrupt\n");

	static int  totalRead=0;
	int32       read;//=0;
	double *data;
    data = commands.getDataArray();


	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for (int i=0; i < commands.getIteration(); i = i+1)
    {
        DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,data,commands.getSampleRate(),&read,NULL));
        printf("Acquiring samples: Samples Read %d\r",(int)(totalRead+=read));
        cout << endl;
        time_t now = time(0);
        char* dt = ctime(&now);
        cout << dt << endl;
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time (&rawtime);
        timeinfo = localtime (&rawtime);

        strftime (buffer,80,"The time in Seconds: %S", timeinfo);
        puts (buffer);
	}


Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle); //stops the task
		DAQmxClearTask(taskHandle); //clears the task
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}

void Startup::file()
{
    int32       error=0;
	TaskHandle  taskHandle=0;
    SYSTEMTIME st;

    readConfig commands;
    commands.setAllConfig();

    /*********************************************/
	// DAQmx Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle)); //creates a task
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,commands.getPhysChan(),"",DAQmx_Val_Diff,-10.0,10.0,DAQmx_Val_Volts,NULL)); // create channel(s) to measure RMS voltage
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan())); // a digital edge produces each sample

    /*********************************************/
	// DAQmx TDMS Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxConfigureLogging(taskHandle,"C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\Log_1.tdms",DAQmx_Val_LogAndRead,"Voltage",DAQmx_Val_OpenOrCreate));

    /*********************************************/
	// DAQmx Start Code
	/*********************************************/
    DAQmxErrChk (DAQmxStartTask(taskHandle)); //starts the created task

    printf("Logging samples continuously. Press Enter to interrupt\n");

	static int  totalRead=0;
	int32       read;//=0;
	double      *data;
    data = commands.getDataArray();

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for (int i=0; i < commands.getIteration(); i = i+1)
    {
        DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,data,commands.getSampleRate(),&read,NULL));
        printf("Acquiring samples: Samples Read %d\r",(int)(totalRead+=read));
        cout << endl;
        GetSystemTime(&st);
        printf("System Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle); //stops the task
		DAQmxClearTask(taskHandle); //clears the task
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}

void Startup::distance()
{
    /*********************************************/
	// Set-up
	/*********************************************/
    int32       error=0;
	TaskHandle  taskHandle=0;
	SYSTEMTIME st;
    ofstream MyExcelFile;
    bool exitLoop = false; // condition for arduino while loop

    // open excel file
    MyExcelFile.open("C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\test1.csv");
    MyExcelFile << "Iteration, Distance, Voltage1, Voltage2, Time Stamp" << endl; // sets excel file headers

    Serial* SP = new Serial("\\\\.\\COM3"); // create new serial port
    if (SP->IsConnected())
		printf("Connected to Serial Port");
		cout << endl;

   printf("Distance Measurements Displayed on Arduino");
   cout << endl << "Press ESC to Record Data or Rit Ctrl to Terminate" << endl;

    // displays the incoming data from Arduino until terminated by user
    while (!exitLoop)
    {
        readResult = SP->ReadData(incomingData,dataLength);
        incomingData[readResult] = 0;
        printf("%s",incomingData); // print data to screen
        if (GetAsyncKeyState(VK_ESCAPE))
        {
            exitLoop = true;
        }
        else if (GetAsyncKeyState(VK_RCONTROL))
        {
            exit(1);
        }
    }

    // record pressure data
    readConfig commands;
    commands.setAllConfig();

    /*********************************************/
	// DAQmx Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle)); //creates a task
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,commands.getPhysChan(),"",DAQmx_Val_Diff,-10.0,10.0,DAQmx_Val_Volts,NULL)); // create channel(s) to measure RMS voltage
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan())); // a digital edge produces each sample

    /*********************************************/
	// DAQmx TDMS Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxConfigureLogging(taskHandle,"C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\Log_1.tdms",DAQmx_Val_LogAndRead,"Voltage",DAQmx_Val_OpenOrCreate));

    /*********************************************/
	// DAQmx Start Code
	/*********************************************/
    DAQmxErrChk (DAQmxStartTask(taskHandle)); //starts the created task

    printf("Logging samples continuously. Press Enter to interrupt\n");

	static int  totalRead=0;
	int32       read;//=0;
	double      *data;
    data = commands.getDataArray();


    readResult=0;
	/*********************************************/
	// Read Code
	/*********************************************/
	for (int i=0; i < commands.getIteration(); ++i)
	{
        //DAQ DAta
		DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,data,commands.getSampleRate(),&read,NULL));
        printf("Acquiring samples: Samples Read %d\r",(int)(totalRead+=read));
        cout << endl;

        // Organise Time for Recording
        GetSystemTime(&st); // time stamp
        printf("System Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        string strTime;
        char buffer [256];
        sprintf(buffer, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
        strTime = buffer;

        int arraySize = read*commands.getNumofChan()*commands.getIteration(); // specify array size for excel
        int col2 = 0;
        int col3 = 1;
        int col4 = 2;

        for (int j=0; j < (read*commands.getNumofChan()); ++j)
        {

            double distance[arraySize];
            distance[j] = ((data[j]-0.0358)/0.0048)*10; // calibrated distance measurement in mm

            if(col2==j)
            {
                MyExcelFile << i << "," << distance[j];
                col2 = col2+commands.getNumofChan();
            }

            if(col3==j)
            {
                MyExcelFile << "," << data[j];
                col3 = col3+commands.getNumofChan();
            }

            if(col4==j)
            {
                MyExcelFile << "," << data[j] << "," << strTime << endl;
                col4 = col4+commands.getNumofChan();
            }

        }
	}


	getchar();

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle); //stops the task
		DAQmxClearTask(taskHandle); //clears the task
		MyExcelFile.close();
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}

void Startup::multiDev()
{

    int32       error=0;
	TaskHandle  taskHandle=0;
	TaskHandle  taskHandle2=0;
	SYSTEMTIME st;

    readConfig commands;

    commands.setAllConfig();

    /*********************************************/
	// DAQmx Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle)); //creates a task
    DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,commands.getPhysChan(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL)); // create channel(s) to measure RMS voltage
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle2,"Dev2/ai0:4","",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan())); // a digital edge produces each sample
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle2,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan()));

    /*********************************************/
	// DAQmx TDMS Configure Code
	/*********************************************/
    DAQmxErrChk (DAQmxConfigureLogging(taskHandle,"C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\Log_1.tdms",DAQmx_Val_LogAndRead,"Voltage",DAQmx_Val_OpenOrCreate));
    DAQmxErrChk (DAQmxConfigureLogging(taskHandle2,"C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\Log_2.tdms",DAQmx_Val_LogAndRead,"Voltage",DAQmx_Val_OpenOrCreate));

    /*********************************************/
	// DAQmx Start Code
	/*********************************************/
    DAQmxErrChk (DAQmxStartTask(taskHandle)); //starts the created task
    DAQmxErrChk (DAQmxStartTask(taskHandle2));

    printf("Logging samples continuously. Press Enter to interrupt\n");

	static int  totalRead=0;
	static int  totalRead2=0;
	int32       read;
	int32       read2;
	double      *data;
    data = commands.getDataArray();
    double      *data2; // check that the data array accommodates for the num of channels in Dev2
    data2 = commands.getDataArray();

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for (int i=0; i < commands.getIteration(); i = i+1)
    {
        DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,data,commands.getSampleRate(),&read,NULL));

        printf("Acquiring samples: Samples Read %d\r",(int)(totalRead+=read));
        cout << endl;

        GetSystemTime(&st);
        printf("Sample Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        // Second Device
        DAQmxErrChk (DAQmxReadAnalogF64(taskHandle2,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,data2,commands.getSampleRate(),&read2,NULL));
        printf("Acquiring samples2: Samples Read %d\r",(int)(totalRead2+=read2));
        cout << endl;

        GetSystemTime(&st);
        printf("Sample2 Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle); //stops the task
		DAQmxClearTask(taskHandle); //clears the task
	}
	if( taskHandle2!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle2); //stops the task
		DAQmxClearTask(taskHandle2); //clears the task
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}

void Startup::digitalOut()
{
    // variables
	int         setting;
    int         error=0;
	TaskHandle	taskHandle=0;
	TaskHandle	taskHandle2=0;
	TaskHandle  taskHandle3=0;
	TaskHandle  taskHandle4=0;
	TaskHandle  taskHandle5=0;
	TaskHandle  taskHandle6=0;
	TaskHandle  taskHandle7=0;
	TaskHandle  taskHandle8=0;
	TaskHandle  taskHandle9=0;
	uInt32      datax=0xffffffff;
    int32		writtenx;
    uInt32      datay=0xffffffff;
    int32		writteny;
    uInt32      dataz=0xffffffff;
    int32		writtenz;

    uInt32      datax1dir=0xffffffff;
    int32		writtenx1dir;
    uInt32      datax2dir=0xffffffff;
    int32		writtenx2dir;

    cout << "Select an Option: " << endl;
	cout << "   1. Run Manually" << endl;
	cout << "   2. Run Automatically" << endl;
	cout << "   3. Reset (Home Setting)" << endl;

    cin >> setting;

    Interface commands;

    if (setting == 1)
    {
        commands.setxDrive();
        commands.setyDrive();
        commands.setzDrive();

        /*********************************************/
        // set x
        /*********************************************/
        if (commands.getSamplePerChanToX1()>0 || commands.getSamplePerChanToX2()>0)
        {

            // create tasks
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle3));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle4));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle6));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle7));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle8));

            // create pulse generation through digital pulses
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getxDrive(),"",DAQmx_Val_ChanForAllLines));

            // create digital output for direction (checks which direction was specified)
            if(commands.getxDrivectr()=="Dev1/port0/line5")
            {
                DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getx2Direction(),"",DAQmx_Val_ChanForAllLines));
            }
            else
            {
                DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getx1Direction(),"",DAQmx_Val_ChanForAllLines));
            }

            // create digital input to read pulses from encoder
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev1/port0/line0","",DAQmx_Val_ChanForAllLines)); // encoder x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle6,"Dev1/port0/line3","",DAQmx_Val_ChanForAllLines)); // encoder x2

            // create digital inputs for limit switches
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev1/port0/line1","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev1/port0/line2","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle7,"Dev1/port1/line0","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x2
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle8,"Dev1/port1/line1","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x2

            // setting direction
            cout << "Setting x_Motion Direction... " << endl;
            DAQmxErrChk (DAQmxStartTask(taskHandle2));
            if (commands.getxDrivectr()=="Dev1/port0/line4:5")
            {
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltonx1(),1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
            }

            else if (commands.getxDrivectr()=="Dev1/port0/line4")
            {
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltonx1(),1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
            }

            else if (commands.getxDrivectr()=="Dev1/port0/line5")
            {
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltonx2(),1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
            }


            // start tasks to rev motors and monitor encoders + limit switches
            cout << '\n' << '\n';
            cout << "Revving Motor in x..." << '\n' << '\n';
            cout << "Encoder Reading..." << '\n' << '\n';
            DAQmxErrChk (DAQmxStartTask(taskHandle));
            DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder x1
            DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch x1
            DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch x1
            DAQmxErrChk (DAQmxStartTask(taskHandle6)); // encoder x2
            DAQmxErrChk (DAQmxStartTask(taskHandle7)); // fwd limit switch x2
            DAQmxErrChk (DAQmxStartTask(taskHandle8)); // rwd limit switch x2

            // selecting x1 & x2 to drive
            if (commands.getxDrivectr()=="Dev1/port0/line4:5")
            {
                for (int i=0; i < commands.getSamplePerChanToX1(); ++i)
                {
                    // reading limit switches
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL)); // fwd limit x1
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL)); // rwd limit x1
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL)); // fwd limit x2
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL)); // rwd limit x2

                    cout << "checking   " << fwdLimitx1 << endl;
                    cout << "checking   " << fwdLimitx2 << endl;
                    cout << "checking   " << rwdLimitx1 << endl;
                    cout << "checking   " << rwdLimitx2 << endl;
                    // set limit switches
//                    if ((commands.x1Count!=0 && counterx1>50) || (commands.x1Count!=38700 && counterx1>50) || (commands.x2Count!=0 && counterx2>50) || (commands.x2Count!=38700 && counterx2>50))
//                    {
                        if (fwdLimitx1==0)
                        {
                            cout << "Foward x1 Limit switch reached" << '\n';
                            if (commands.voltonx1==1)
                            {
                                counterx1 = counterx1 * 3.125;
                                commands.x1Count = commands.x1Count+counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }

                            if (commands.voltonx1==2)
                            {
                                counterx1 = counterx1 * 3.125;
                                commands.x1Count = commands.x1Count-counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }
                            exit(1);
                        }
                        if (rwdLimitx1==0)
                        {
                            cout << "Rearward x1 limit switch reached" << '\n';
                            commands.x1Count=0;
                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                            ofstream out ("x1.txt");
                            out << commands.x1Count;
                            out.close();
                            exit(1);
                        }

                        if (fwdLimitx2==0)
                        {
                            cout << "Foward x2 Limit switch reached" << '\n';
                            if (commands.voltonx2==1)
                            {
                                counterx2 = counterx2 * 3.125;
                                commands.x2Count = commands.x2Count+counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }

                            if (commands.voltonx2==2)
                            {
                                counterx2 = counterx2 * 3.125;
                                commands.x2Count = commands.x2Count-counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }
                            exit(1);
                        }
                        if (rwdLimitx2==0)
                        {
                            cout << "Rearward x2 limit switch reached" << '\n';
                            commands.x2Count=0;
                            cout << "Recorded x2Count: " << commands.x2Count << '\n';
                            ofstream out ("x2.txt");
                            out << commands.x2Count;
                            out.close();
                            exit(1);
                        }
//                    }

                    // generate pulses
                    datax=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i) // delay 0.08s
                    datax = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading encoder x1 data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                    encoder_arrayx1[i] = datacounterx1;
                    if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                    {
                        counterx1++;
                    }

                    // reading encoder x2 data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                    encoder_arrayx2[i] = datacounterx2;
                    if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                    {
                        counterx2++;
                    }
                }

                // encoder x1 feedback
                counterx1 = counterx1 * 3.125;
                cout << "The number of x1 pulses counted = " << setw(6) << counterx1 << '\n';

                // encoder x2 feedback
                counterx2 = counterx2 * 3.125;
                cout << "The number of x2 pulses counted = " << setw(6) << counterx2 << '\n';

                //terminate and clear task
                DAQmxStopTask(taskHandle);
                DAQmxClearTask(taskHandle);

                while(counterx1!=commands.getSamplePerChanToX1())
                {
                    // recreate task only for x1
                    DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
                    DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0/line4","",DAQmx_Val_ChanForAllLines));
                    DAQmxErrChk (DAQmxStartTask(taskHandle));

                    // less pulses
                    if (counterx1<commands.getSamplePerChanToX1())
                    {
                        counterx1Diff = commands.getSamplePerChanToX1() - counterx1;
                        cout << "Delta: " << counterx1Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        for (int i=0; i < counterx1Diff; i++)
                            {
                                // limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                if (fwdLimitx1==0)
                                {
                                    cout << "Forward x1 limit switch reached" << '\n';
                                    if (commands.voltonx1==1)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count+counterx1;
                                        cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }

                                    if (commands.voltonx1==2)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count-counterx1;
                                        cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx1==0)
                                {
                                    cout << "Rearward x1 limit switch reached" << '\n';
                                    commands.x1Count=0;
                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                    ofstream out ("x1.txt");
                                    out << commands.x1Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading x1 encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                encoder_arrayx1[i] = datacounterx1;
                                if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                {
                                    new_counterx1++;
                                }
                            }

                            // x1 encoder feedback
                            counterx1 = counterx1 + (new_counterx1 * 3.125); // calculates new counterx
                            new_counterx1 = 0;
                            cout << "x1 final count: " << counterx1 << '\n';
                    }

                    // more pulses
                    if (counterx1>commands.getSamplePerChanToX1())
                    {
                        counterx1Diff = (counterx1 - commands.getSamplePerChanToX1());
                        cout << "Delta: " << counterx1Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        //set reverse direction
                        if (commands.getvoltonx1()==1)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }
                        else
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }

                            for (int i=0; i < counterx1Diff; i++)
                            {
                                // limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                if (fwdLimitx1==0)
                                {
                                    cout << "Forward x1 limit switch reached" << '\n';
                                    if (commands.voltonx1==1)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count+counterx1;
                                        cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }

                                    if (commands.voltonx1==2)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count-counterx1;
                                        cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx1==0)
                                {
                                    cout << "Rearward x1 limit switch reached" << '\n';
                                    commands.x1Count=0;
                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                    ofstream out ("x1.txt");
                                    out << commands.x1Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                encoder_arrayx1[i] = datacounterx1;
                                if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                {
                                    new_counterx1++;
                                }
                            }

                            // x1 encoder feedback
                            counterx1 = counterx1 - (new_counterx1 * 3.125);
                            new_counterx1 = 0;
                            cout << "Final Count after Reducing: " << counterx1 << '\n';

                            //reset direction
                            if (commands.getvoltonx1()==1)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                            else
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                    }

                    //terminate and clear tasks
                    DAQmxStopTask(taskHandle);
                    DAQmxClearTask(taskHandle);
                }

                while(counterx2!=commands.getSamplePerChanToX2())
                {
                    // recreate task only for x2
                    DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
                    DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0/line5","",DAQmx_Val_ChanForAllLines));
                    DAQmxErrChk (DAQmxStartTask(taskHandle));

                    // for less pulses
                    if (counterx2<commands.getSamplePerChanToX2())
                    {
                        counterx2Diff = commands.getSamplePerChanToX2() - counterx2;
                        cout << "Difference: " << counterx2Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        for (int i=0; i < counterx2Diff; i++)
                            {
                                // reading limit switches
                                if (fwdLimitx2==0)
                                {
                                    cout << "Foward x2 Limit switch reached" << '\n';
                                    if (commands.voltonx2==1)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count+counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }

                                    if (commands.voltonx2==2)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count-counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx2==0)
                                {
                                    cout << "Rearward x2 limit switch reached" << '\n';
                                    commands.x2Count=0;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading x2 encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                encoder_arrayx2[i] = datacounterx2;
                                if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                {
                                    new_counterx2++;
                                }
                            }

                            // encoder x2 feedback
                            counterx2 = counterx2 + (new_counterx2 * 3.125); // calculates new counterx
                            new_counterx2 = 0;
                            cout << "x2 final count: " << counterx2 << '\n';
                    }

                    // for more pulses
                    if (counterx2>commands.getSamplePerChanToX2())
                    {
                        counterx2Diff = (counterx2 - commands.getSamplePerChanToX2());
                        cout << "Delta: " << counterx2Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        //set reverse direction
                        if (commands.getvoltonx2()==1)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }
                        else
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }

                            for (int i=0; i < counterx2Diff; i++)
                            {
                                // reading limit switches
                                if (fwdLimitx2==0)
                                {
                                    cout << "Foward x2 Limit switch reached" << '\n';
                                    if (commands.voltonx2==1)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count+counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }

                                    if (commands.voltonx2==2)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count-counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx2==0)
                                {
                                    cout << "Rearward x2 limit switch reached" << '\n';
                                    commands.x2Count=0;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                encoder_arrayx2[i] = datacounterx2;
                                if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                {
                                    new_counterx2++;
                                }
                            }

                            // encoder feedback
                            counterx2 = counterx2 - (new_counterx2 * 3.125);
                            new_counterx2 = 0;
                            cout << "x2 final count: " << counterx2 << '\n';

                            //reset direction
                            if (commands.getvoltonx2()==1)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                            else
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                    }

                    //terminate and clear tasks
                    DAQmxStopTask(taskHandle);
                    DAQmxClearTask(taskHandle);
                }

                // recording x1 data to file
                if (commands.voltonx1==1)
                {
                    commands.x1Count = commands.x1Count+counterx1;
                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                    ofstream out ("x1.txt");
                    out << commands.x1Count;
                    out.close();
                }

                if (commands.voltonx1==2)
                {
                    commands.x1Count = commands.x1Count-counterx1;
                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                    ofstream out ("x1.txt");
                    out << commands.x1Count;
                    out.close();
                }

                // recording x2 data to file
                if (commands.voltonx2==1)
                {
                    commands.x2Count = commands.x2Count+counterx2;
                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                    ofstream out ("x2.txt");
                    out << commands.x2Count;
                    out.close();
                }

                if (commands.voltonx2==2)
                {
                    commands.x2Count = commands.x2Count-counterx2;
                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                    ofstream out ("x2.txt");
                    out << commands.x2Count;
                    out.close();
                }

            }

            // setting only x1 to drive
            if (commands.getxDrivectr()=="Dev1/port0/line4")
            {
                // generate pulses, read encoder, limit switches
                for (int i=0; i < commands.getSamplePerChanToX1(); ++i)
                {
                    // activate/deactivate limit switches depending on home setting
                    if ((commands.x1Count!=0 && counterx1>50) || (commands.x1Count!=38700 && counterx1>50))
                    {
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                        if (fwdLimitx1==0)
                        {
                            cout << "Forward x1 Limit switch reached" << '\n' << '\n';
                            if (commands.voltonx1==1)
                            {
                                counterx1 = counterx1 * 3.125;
                                commands.x1Count = commands.x1Count+counterx1;
                                cout << "x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }

                            if (commands.voltonx1==2)
                            {
                                counterx1 = counterx1 * 3.125;
                                commands.x1Count = commands.x1Count-counterx1;
                                cout << "x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }
                            exit(1);
                        }
                        if (rwdLimitx1==0)
                        {
                            cout << "Rearward x1 limit switch reached" << '\n';
                            commands.x1Count=0;
                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                            ofstream out ("x1.txt");
                            out << commands.x1Count;
                            out.close();
                            exit(1);
                        }
                    }

                    // generate pulse
                    datax=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i) // delay
                    datax = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading x1 encoder data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                    encoder_arrayx1[i] = datacounterx1;
                    if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                    {
                        counterx1++;
                    }
                }

                // x1 encoder feedback
                counterx1 = counterx1 * 3.125;
                cout << "The number of pulses counted = " << setw(6) << counterx1 << '\n';

                // correct x1 stepper motor position based on encoder feedback
                while(counterx1!=commands.getSamplePerChanToX1())
                {
                    // less pulses
                    if (counterx1<commands.getSamplePerChanToX1())
                    {
                        counterx1Diff = commands.getSamplePerChanToX1() - counterx1;
                        cout << "Delta: " << counterx1Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        for (int i=0; i < counterx1Diff; i++)
                            {
                                // limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                if (fwdLimitx1==0)
                                {
                                    cout << "Forward x1 limit switch reached" << '\n';
                                    if (commands.voltonx1==1)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count+counterx1;
                                        cout << "x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }

                                    if (commands.voltonx1==2)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count-counterx1;
                                        cout << "x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx1==0)
                                {
                                    cout << "Rearward x1 limit switch reached" << '\n';
                                    commands.x1Count=0;
                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                    ofstream out ("x1.txt");
                                    out << commands.x1Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulse
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1500000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1500000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                encoder_arrayx1[i] = datacounterx1;
                                if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                {
                                    new_counterx1++;
                                }
                            }

                            counterx1 = counterx1 + (new_counterx1 * 3.125); // calculates new counterx
                            new_counterx1 = 0;
                            cout << "x1 final count: " << counterx1 << '\n';
                    }

                    // more pulses
                    if (counterx1>commands.getSamplePerChanToX1())
                    {
                        counterx1Diff = (counterx1 - commands.getSamplePerChanToX1());
                        cout << "Delta: " << counterx1Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        //set reverse direction
                        if (commands.getvoltonx1()==1)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }
                        else
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }

                            for (int i=0; i < counterx1Diff; i++)
                            {
                                // limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                if (fwdLimitx1==0)
                                {
                                    cout << "Forward x1 limit switch reached" << '\n';
                                    if (commands.voltonx1==1)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count+counterx1;
                                        cout << "x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }

                                    if (commands.voltonx1==2)
                                    {
                                        counterx1 = counterx1 * 3.125;
                                        commands.x1Count = commands.x1Count-counterx1;
                                        cout << "x1Count: " << commands.x1Count << '\n';
                                        ofstream out ("x1.txt");
                                        out << commands.x1Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx1==0)
                                {
                                    cout << "Rearward x1 limit switch reached" << '\n';
                                    commands.x1Count=0;
                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                    ofstream out ("x1.txt");
                                    out << commands.x1Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                encoder_arrayx1[i] = datacounterx1;
                                if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                {
                                    new_counterx1++;
                                }
                            }

                            // encoder feedback
                            counterx1 = counterx1 - (new_counterx1 * 3.125);
                            new_counterx1 = 0;
                            cout << "x1 final count: " << counterx1 << '\n';

                            //reset direction
                            if (commands.getvoltonx1()==1)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                            else
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                    }
                }

                // recording x1 data to file
                if (commands.voltonx1==1)
                {
                    commands.x1Count = commands.x1Count+counterx1;
                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                    ofstream out ("x1.txt");
                    out << commands.x1Count;
                    out.close();
                }

                if (commands.voltonx1==2)
                {
                    commands.x1Count = commands.x1Count-counterx1;
                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                    ofstream out ("x1.txt");
                    out << commands.x1Count;
                    out.close();
                }

            }

            // setting only x2 to drive
            if (commands.getxDrivectr()=="Dev1/port0/line5")
            {
                // generate pulses, read encoder, limit switches
                for (int i=0; i < commands.getSamplePerChanToX2(); ++i)
                {
                    // activate/deactivate limit switches depending on home setting
//                    if ((commands.x2Count!=0 && counterx2>50) || (commands.x2Count!=38700 && counterx2>50))
//                    {
                        // limit switches
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL));

                        cout << "checking       " << rwdLimitx2 << endl;

                        // reading limit switches
                        if (fwdLimitx2==0)
                        {
                            cout << "Foward x2 Limit switch reached" << '\n';
                            if (commands.voltonx2==1)
                            {
                                counterx2 = counterx2 * 3.125;
                                commands.x2Count = commands.x2Count+counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }
                            if (commands.voltonx2==2)
                            {
                                counterx2 = counterx2 * 3.125;
                                commands.x2Count = commands.x2Count-counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }
                            exit(1);
                        }

                        if (rwdLimitx2==0)
                        {
                            cout << "Rearward x2 limit switch reached" << '\n';
                            commands.x2Count=0;
                            cout << "Recorded x2Count: " << commands.x2Count << '\n';
                            ofstream out ("x2.txt");
                            out << commands.x2Count;
                            out.close();
                            exit(1);
                        }

//                    }

                    // generate pulse
                    datax=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i) // delay
                    datax = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading x2 encoder data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                    encoder_arrayx2[i] = datacounterx2;
                    if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                    {
                        counterx2++;
                    }
                }

                // x2 encoder feedback
                counterx2 = counterx2 * 3.125;
                cout << "The number of pulses counted = " << setw(6) << counterx2 << '\n';

                // correct x1 stepper motor position based on encoder feedback
                while(counterx2!=commands.getSamplePerChanToX2())
                {
                    // for less pulses
                    if (counterx2<commands.getSamplePerChanToX2())
                    {
                        counterx2Diff = commands.getSamplePerChanToX2() - counterx2;
                        cout << "Difference: " << counterx2Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        for (int i=0; i < counterx2Diff; i++)
                            {
                                // reading limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx2,NULL));

                                // reading limit switches
                                if (fwdLimitx2==0)
                                {
                                    cout << "Foward x2 Limit switch reached" << '\n';
                                    if (commands.voltonx2==1)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count+counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    if (commands.voltonx2==2)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count-counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx2==0)
                                {
                                    cout << "Rearward x2 limit switch reached" << '\n';
                                    commands.x2Count=0;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading x2 encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                encoder_arrayx2[i] = datacounterx2;
                                if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                {
                                    new_counterx2++;
                                }
                            }

                            // encoder x2 feedback
                            counterx2 = counterx2 + (new_counterx2 * 3.125); // calculates new counterx
                            new_counterx2 = 0;
                            cout << "x2 final count: " << counterx2 << '\n';
                    }

                    // for more pulses
                    if (counterx2>commands.getSamplePerChanToX2())
                    {
                        counterx2Diff = (counterx2 - commands.getSamplePerChanToX2());
                        cout << "Delta: " << counterx2Diff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        //set reverse direction
                        if (commands.getvoltonx2()==1)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }
                        else if (commands.getvoltonx2()==2)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                        }

                            for (int i=0; i < counterx2Diff; i++)
                            {
                                // reading limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx2,NULL));

                                // reading limit switches
                                if (fwdLimitx2==0)
                                {
                                    cout << "Foward x2 Limit switch reached" << '\n';
                                    if (commands.voltonx2==1)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count+counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    if (commands.voltonx2==2)
                                    {
                                        counterx2 = counterx2 * 3.125;
                                        commands.x2Count = commands.x2Count-counterx2;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimitx2==0)
                                {
                                    cout << "Rearward x2 limit switch reached" << '\n';
                                    commands.x2Count=0;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                encoder_arrayx2[i] = datacounterx2;
                                if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                {
                                    new_counterx2++;
                                }
                            }

                            // encoder feedback
                            counterx2 = counterx2 - (new_counterx2 * 3.125);
                            new_counterx2 = 0;
                            cout << "x2 final count: " << counterx2 << '\n';

                            //reset direction
                            if (commands.getvoltonx2()==1)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                            else if (commands.getvoltonx2()==2)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                            }
                    }

                }

                // recording x2 data to file
                if (commands.voltonx2==1)
                {
                    commands.x2Count = commands.x2Count+counterx2;
                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                    ofstream out ("x2.txt");
                    out << commands.x2Count;
                    out.close();
                }

                if (commands.voltonx2==2)
                {
                    commands.x2Count = commands.x2Count-counterx2;
                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                    ofstream out ("x2.txt");
                    out << commands.x2Count;
                    out.close();
                }

            }

            //terminate tasks
            DAQmxStopTask(taskHandle);
            DAQmxStopTask(taskHandle2);
            DAQmxStopTask(taskHandle3);
            DAQmxStopTask(taskHandle4);
            DAQmxStopTask(taskHandle5);
            DAQmxStopTask(taskHandle6);

            DAQmxStopTask(taskHandle7);
            DAQmxStopTask(taskHandle8);
            DAQmxClearTask(taskHandle);
            DAQmxClearTask(taskHandle2);
            DAQmxClearTask(taskHandle3);
            DAQmxClearTask(taskHandle4);
            DAQmxClearTask(taskHandle5);
            DAQmxClearTask(taskHandle6);
            DAQmxClearTask(taskHandle7);
            DAQmxClearTask(taskHandle8);
        }

        /*********************************************/
        // Set y
        /*********************************************/
        if (commands.getSamplePerChanToY()>0)
        {
            uInt32      datay=0xffffffff;
            int32		writteny;

            // create tasks
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle3));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle4));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));


            // create pulse generation through digital pulses
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getyDrive(),"",DAQmx_Val_ChanForAllLines));

            // create digital output for direction
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getyDirection(),"",DAQmx_Val_ChanForAllLines));

            // create digital input to read pulses from encoder
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev1/port1/line2","",DAQmx_Val_ChanForAllLines)); // encoder y

            // create digital inputs for limit switches
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev1/port1/line5","",DAQmx_Val_ChanForAllLines)); // fwd limit switches y
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev1/port1/line4","",DAQmx_Val_ChanForAllLines)); // rwd limit switches y

            // setting direction
            cout << "Setting y_Motion Direction... " << endl;
            DAQmxErrChk (DAQmxStartTask(taskHandle2));
            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltony(),1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));

            // start tasks to rev motors and monitor encoders + limit switches
            cout << '\n' << '\n';
            cout << "Revving Motor in y..." << '\n' << '\n';
            cout << "Encoder Reading..." << '\n' << '\n';
            DAQmxErrChk (DAQmxStartTask(taskHandle));
            DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder y
            DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch y
            DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch y


            // generate pulses, read encoder, limit switches
            for (int i=0; i < commands.getSamplePerChanToY(); ++i)
            {
                // activate/deactivate limit switches depending on home setting
                if ((commands.yCount!=0 && countery>50) || (commands.yCount!=33000 && countery>50))
                    {
                        // limit switches
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimity,1,&readfwdLimity,NULL));
                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimity,1,&readrwdLimity,NULL));

                        if (fwdLimity==0)
                        {
                            cout << "Foward y Limit switch reached" << '\n';
                            if (commands.voltony==1)
                            {
                                cout << "checking        " << commands.yCount << '\n';
                                countery = countery * 3.125;
                                commands.yCount = commands.yCount+countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }

                            if (commands.voltony==2)
                            {
                                countery = countery * 3.125;
                                commands.yCount = commands.yCount-countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }
                            exit(1);
                        }

                        if (rwdLimity==0)
                        {
                            cout << "Rearward y limit switch reached" << '\n';
                            commands.yCount=0;
                            cout << "Recorded yCount: " << commands.yCount << '\n';
                            ofstream out ("y.txt");
                            out << commands.yCount;
                            out.close();
                            exit(1);
                        }

                    }

                    // generate pulse
                    datay=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                    for (int i=0; i < 200000; ++i) // delay
                    datay = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading x2 encoder data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                    encoder_arrayy[i] = datacountery;
                    if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                    {
                        countery++;
                    }
                }

            // y encoder feedback
            countery = countery * 3.125;
            cout << "The number of pulses counted = " << setw(6) << countery << '\n';


            // correct y stepper motor position based on encoder feedback
            while(countery!=commands.getSamplePerChanToY())
            {
                // for less pulses
                if (countery<commands.getSamplePerChanToY())
                {
                    counteryDiff = commands.getSamplePerChanToY() - countery;
                    cout << "Difference: " << counteryDiff << endl;
                    cout << "Correcting Revs..." << '\n' << '\n';
                    Sleep(1000);

                    for (int i=0; i < counteryDiff; i++)
                        {
                                // reading limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimity,1,&readfwdLimity,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimity,1,&readrwdLimity,NULL));

                                if (fwdLimity==0)
                                {
                                    cout << "Foward y Limit switch reached" << '\n';
                                    if (commands.voltony==1)
                                    {
                                        countery = countery * 3.125;
                                        commands.yCount = commands.yCount+countery;
                                        cout << "Recorded yCount: " << commands.yCount << '\n';
                                        ofstream out ("y.txt");
                                        out << commands.yCount;
                                        out.close();
                                    }

                                    if (commands.voltony==2)
                                    {
                                        countery = countery * 3.125;
                                        commands.yCount = commands.yCount-countery;
                                        cout << "Recorded yCount: " << commands.yCount << '\n';
                                        ofstream out ("y.txt");
                                        out << commands.yCount;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimity==0)
                                {
                                    cout << "Rearward y limit switch reached" << '\n';
                                    commands.yCount=0;
                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                    ofstream out ("y.txt");
                                    out << commands.yCount;
                                    out.close();
                                    exit(1);
                                }



                                // generate pulses
                                datay=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datay = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading y encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                                encoder_arrayy[i] = datacountery;
                                if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                                {
                                    new_countery++;
                                }
                            }

                            // encoder y feedback
                            countery = countery + (new_countery * 3.125); // calculates new counterx
                            new_countery = 0;
                            cout << "y final count: " << countery << '\n';
                    }

                // for more pulses
                if (countery>commands.getSamplePerChanToY())
                {
                        counteryDiff = (countery - commands.getSamplePerChanToY());
                        cout << "Delta: " << counteryDiff << endl;
                        cout << "Correcting Revs..." << '\n' << '\n';
                        Sleep(1000);

                        //set reverse direction
                        if (commands.getvoltony()==1)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                        }
                        else if (commands.getvoltony()==2)
                        {
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                        }

                            for (int i=0; i < counteryDiff; i++)
                            {
                                // reading limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimity,1,&readfwdLimity,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimity,1,&readrwdLimity,NULL));

                                if (fwdLimity==0)
                                {
                                    cout << "Foward y Limit switch reached" << '\n';
                                    if (commands.voltony==1)
                                    {
                                        countery = countery * 3.125;
                                        commands.yCount = commands.yCount+countery;
                                        cout << "Recorded yCount: " << commands.yCount << '\n';
                                        ofstream out ("y.txt");
                                        out << commands.yCount;
                                        out.close();
                                    }

                                    if (commands.voltony==2)
                                    {
                                        countery = countery * 3.125;
                                        commands.yCount = commands.yCount-countery;
                                        cout << "Recorded yCount: " << commands.yCount << '\n';
                                        ofstream out ("y.txt");
                                        out << commands.yCount;
                                        out.close();
                                    }
                                    exit(1);
                                }

                                if (rwdLimity==0)
                                {
                                    cout << "Rearward y limit switch reached" << '\n';
                                    commands.yCount=0;
                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                    ofstream out ("y.txt");
                                    out << commands.yCount;
                                    out.close();
                                    exit(1);
                                }

                                // generate pulses
                                datay=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 1000000; ++i) // delay 0.08s
                                datay = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 1000000; ++i); // delay

                                // reading encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                                encoder_arrayy[i] = datacountery;
                                if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                                {
                                    new_countery++;
                                }
                            }

                            // encoder feedback
                            countery = countery - (new_countery * 3.125);
                            new_countery = 0;
                            cout << "y final count: " << countery << '\n';

                            //reset direction
                            if (commands.getvoltony()==1)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                            }
                            else if (commands.getvoltony()==2)
                            {
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                            }
                    }

                }


            // recording x2 data to file
            if (commands.voltony==1)
            {
                    commands.yCount = commands.yCount+countery;
                    cout << "Recorded yCount: " << commands.yCount << '\n';
                    ofstream out ("y.txt");
                    out << commands.yCount;
                    out.close();
            }

            if (commands.voltony==2)
            {
                    commands.yCount = commands.yCount-countery;
                    cout << "Recorded yCount: " << commands.yCount << '\n';
                    ofstream out ("y.txt");
                    out << commands.yCount;
                    out.close();
            }


            //terminate tasks
            DAQmxStopTask(taskHandle);
            DAQmxStopTask(taskHandle2);
            DAQmxClearTask(taskHandle);
            DAQmxClearTask(taskHandle2);
        }


        /*********************************************/
        // Set z
        /*********************************************/
        if(commands.getSamplePerChanToZ()>0)
        {
            commands.setz();

            DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle6));

            // create pulse generation through digital pulses
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle5,commands.getzDrive(),"",DAQmx_Val_ChanForAllLines));

            // create digital output for direction
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle6,commands.getzDirection(),"",DAQmx_Val_ChanForAllLines));

            cout << "Setting z_Motion Direction... " << endl;
            DAQmxErrChk (DAQmxStartTask(taskHandle6));
            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle6,commands.getvoltonz(),1,10.0,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));

            cout << "Revving Motor in z..." << '\n' << '\n';
            DAQmxErrChk (DAQmxStartTask(taskHandle5));

            for (int i=0; i < commands.getSamplePerChanToZ(); ++i)
            {
                dataz=0xffffffff;
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle5,1,1,-1,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));
                for (int i=0; i < 1000000; ++i) // delay 0.08s

                dataz = 0;
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle5,1,1,-1,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));
                for (int i=0; i < 1000000; ++i); // delay
            }

            //terminate tasks
            DAQmxStopTask(taskHandle5);
            DAQmxStopTask(taskHandle6);
            DAQmxClearTask(taskHandle5);
            DAQmxClearTask(taskHandle6);
        }


    }

    if (setting == 2)
    {
       // reads data from file
       commands.setautoMotion();

       // loop through the data
       for (int i =0; i<commands.rowA; i++)
       {
            for (int j =0; j<commands.colA; j++)
            {
                if (j<1)
                {
                    commands.numOfRevsx1 = commands.arrayA[i][j]*400; // 400 pulses per rev
                    commands.numOfRevsx2 = commands.arrayA[i][j]*400; // 400 pulses per rev
                    cout << "Number of Pulses Sent to x1 & x2: " << commands.numOfRevsx1 << setw(6) << commands.numOfRevsx2 << '\n';

                    commands.setxAutoDrive();
                    if (commands.getSamplePerChanToX1()>0 || commands.getSamplePerChanToX2()>0)
                    {

                        // create tasks
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle3));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle4));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle6));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle7));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle8));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle9));

                        // create pulse generation through digital pulses
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getxDrive(),"",DAQmx_Val_ChanForAllLines));

                        // create digital output for direction
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getx1Direction(),"",DAQmx_Val_ChanForAllLines));
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle7,commands.getx2Direction(),"",DAQmx_Val_ChanForAllLines));

                        // create digital input to read pulses from encoder
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev1/port0/line0","",DAQmx_Val_ChanForAllLines)); // encoder x1
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle6,"Dev1/port0/line3","",DAQmx_Val_ChanForAllLines)); // encoder x2

                        // create digital inputs for limit switches
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev1/port0/line1","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x1
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev1/port0/line2","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x1
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle9,"Dev2/port0/line1","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x2
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle8,"Dev2/port0/line0","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x2


                        // setting direction
                        cout << "Setting x_Motion Direction... " << endl;
                        DAQmxErrChk (DAQmxStartTask(taskHandle2));
                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltonx1(),1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle7,commands.getvoltonx2(),1,10.0,DAQmx_Val_GroupByChannel,&datax2dir,&writtenx2dir,NULL));

                        // start tasks to rev motors and monitor encoders + limit switches
                        cout << '\n' << '\n';
                        cout << "Revving Motor in x..." << '\n' << '\n';
                        cout << "Encoder Reading..." << '\n' << '\n';
                        DAQmxErrChk (DAQmxStartTask(taskHandle));
                        DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder x1
                        DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch x1
                        DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch x1
                        DAQmxErrChk (DAQmxStartTask(taskHandle6)); // encoder x2
                        DAQmxErrChk (DAQmxStartTask(taskHandle7)); // x2 direction
                        DAQmxErrChk (DAQmxStartTask(taskHandle9)); // fwd limit switch x2
                        DAQmxErrChk (DAQmxStartTask(taskHandle8)); // rwd limit switch x2


                        // selecting x1 & x2 to drive
                        if (commands.getxDrivectr()=="Dev1/port1/line0:1")
                        {
                            for (int i=0; i < commands.getSamplePerChanToX1(); ++i)
                            {
                                // reading limit switches
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle9,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL));

                                if ((commands.x1Count!=0 && counterx1>100) || (commands.x1Count!=38700 && counterx1>100) || (commands.x2Count!=0 && counterx2>100) || (commands.x2Count!=38700 && counterx2>100))
                                {
                                    if (fwdLimitx1==0)
                                    {
                                        cout << "Foward x1 Limit switch reached" << '\n';
                                        if (commands.voltonx1==1)
                                        {
                                            counterx1 = counterx1 * 3.125;
                                            commands.x1Count = commands.x1Count+counterx1;
                                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                            ofstream out ("x1.txt");
                                            out << commands.x1Count;
                                            out.close();
                                        }

                                        if (commands.voltonx1==2)
                                        {
                                            counterx1 = counterx1 * 3.125;
                                            commands.x1Count = commands.x1Count-counterx1;
                                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                            ofstream out ("x1.txt");
                                            out << commands.x1Count;
                                            out.close();
                                        }
                                        exit(1);
                                    }
                                    else if (rwdLimitx1==0)
                                    {
                                        cout << "Rearward x1 limit switch reached" << '\n';
                                        if (commands.voltonx1==1)
                                        {
                                            counterx1 = counterx1 * 3.125;
                                            commands.x1Count = commands.x1Count+counterx1;
                                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                            ofstream out ("x1.txt");
                                            out << commands.x1Count;
                                            out.close();
                                        }

                                        if (commands.voltonx1==2)
                                        {
                                            counterx1 = counterx1 * 3.125;
                                            commands.x1Count = commands.x1Count-counterx1;
                                            cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                            ofstream out ("x1.txt");
                                            out << commands.x1Count;
                                            out.close();
                                        }
                                        exit(1);
                                    }
                                    else if (fwdLimitx2==0)
                                    {
                                        cout << "Foward x2 Limit switch reached" << '\n';
                                        if (commands.voltonx2==1)
                                        {
                                            counterx2 = counterx2 * 3.125;
                                            commands.x2Count = commands.x2Count+counterx2;
                                            cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                            ofstream out ("x2.txt");
                                            out << commands.x2Count;
                                            out.close();
                                        }

                                        if (commands.voltonx2==2)
                                        {
                                            counterx2 = counterx2 * 3.125;
                                            commands.x2Count = commands.x2Count-counterx2;
                                            cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                            ofstream out ("x2.txt");
                                            out << commands.x2Count;
                                            out.close();
                                        }
                                        exit(1);
                                    }
                                    else if (rwdLimitx2==0)
                                    {
                                        cout << "Rearward x2 limit switch reached" << '\n';
                                        commands.x2Count=0;
                                        cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                        ofstream out ("x2.txt");
                                        out << commands.x2Count;
                                        out.close();
                                        exit(1);
                                    }

                                }
                                // generate pulses
                                datax=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 200000; ++i) // delay
                                datax = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                for (int i=0; i < 200000; ++i); // delay

                                // reading encoder x1 data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                encoder_arrayx1[i] = datacounterx1;
                                if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                {
                                    counterx1++;
                                }

                                // reading encoder x2 data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                encoder_arrayx2[i] = datacounterx2;
                                if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                {
                                    counterx2++;
                                }
                            }

                            // encoder x1 feedback
                            counterx1 = counterx1 * 3.125;
                            cout << "The number of x1 pulses counted = " << setw(6) << counterx1 << '\n';

                            // encoder x2 feedback
                            counterx2 = counterx2 * 3.125;
                            cout << "The number of x2 pulses counted = " << setw(6) << counterx2 << '\n';

                            //terminate and clear task
                            DAQmxStopTask(taskHandle);
                            DAQmxClearTask(taskHandle);

                            while(counterx1!=commands.getSamplePerChanToX1())
                            {
                                // recreate task only for x1
                                DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
                                DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port1/line0","",DAQmx_Val_ChanForAllLines));
                                DAQmxErrChk (DAQmxStartTask(taskHandle));

                                // less pulses
                                if (counterx1<commands.getSamplePerChanToX1())
                                {
                                    counterx1Diff = commands.getSamplePerChanToX1() - counterx1;
                                    cout << "Delta: " << counterx1Diff << endl;
                                    cout << "Correcting Revs..." << '\n' << '\n';
                                    Sleep(1000);

                                    for (int i=0; i < counterx1Diff; i++)
                                        {
                                            // limit switches
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                            if (fwdLimitx1==0)
                                            {
                                                cout << "Forward x1 limit switch reached" << '\n';
                                                if (commands.voltonx1==1)
                                                {
                                                    counterx1 = counterx1 * 3.125;
                                                    commands.x1Count = commands.x1Count+counterx1;
                                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                    ofstream out ("x1.txt");
                                                    out << commands.x1Count;
                                                    out.close();
                                                }

                                                if (commands.voltonx1==2)
                                                {
                                                    counterx1 = counterx1 * 3.125;
                                                    commands.x1Count = commands.x1Count-counterx1;
                                                    cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                    ofstream out ("x1.txt");
                                                    out << commands.x1Count;
                                                    out.close();
                                                }
                                                exit(1);
                                            }

                                            if (rwdLimitx1==0)
                                            {
                                            cout << "Rearward x1 limit switch reached" << '\n';
                                            if (commands.voltonx1==1)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count+counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }

                                            if (commands.voltonx1==2)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count-counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }
                                            exit(1);
                                        }

                                            // generate pulses
                                            datax=0xffffffff;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i) // delay 0.08s
                                            datax = 0;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i); // delay

                                            // reading x1 encoder data
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                            encoder_arrayx1[i] = datacounterx1;
                                            if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                            {
                                                new_counterx1++;
                                            }
                                        }

                                        // x1 encoder feedback
                                        counterx1 = counterx1 + (new_counterx1 * 3.125); // calculates new counterx
                                        new_counterx1 = 0;
                                        cout << "x1 final count: " << counterx1 << '\n';
                                }

                                // more pulses
                                if (counterx1>commands.getSamplePerChanToX1())
                                {
                                    counterx1Diff = (counterx1 - commands.getSamplePerChanToX1());
                                    cout << "Delta: " << counterx1Diff << endl;
                                    cout << "Correcting Revs..." << '\n' << '\n';
                                    Sleep(1000);

                                    //set reverse direction
                                    if (commands.getvoltonx1()==1)
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }
                                    else
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }

                                    for (int i=0; i < counterx1Diff; i++)
                                    {
                                        // limit switches
                                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL));
                                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL));

                                        if (fwdLimitx1==0)
                                        {
                                            cout << "Forward x1 limit switch reached" << '\n';
                                            if (commands.voltonx1==1)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count+counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }

                                            if (commands.voltonx1==2)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count-counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }
                                            exit(1);
                                        }

                                        if (rwdLimitx1==0)
                                        {
                                            cout << "Rearward x1 limit switch reached" << '\n';
                                            if (commands.voltonx1==1)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count+counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }

                                            if (commands.voltonx1==2)
                                            {
                                                counterx1 = counterx1 * 3.125;
                                                commands.x1Count = commands.x1Count-counterx1;
                                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                                ofstream out ("x1.txt");
                                                out << commands.x1Count;
                                                out.close();
                                            }
                                            exit(1);
                                        }

                                        // generate pulses
                                        datax=0xffffffff;
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                        for (int i=0; i < 1000000; ++i) // delay 0.08s
                                        datax = 0;
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                        for (int i=0; i < 1000000; ++i); // delay

                                        // reading encoder data
                                        DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                                        encoder_arrayx1[i] = datacounterx1;
                                        if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                                        {
                                            new_counterx1++;
                                        }
                                    }

                                    // x1 encoder feedback
                                    counterx1 = counterx1 - (new_counterx1 * 3.125);
                                    new_counterx1 = 0;
                                    cout << "Final Count after Reducing: " << counterx1 << '\n';

                                    //reset direction
                                    if (commands.getvoltonx1()==1)
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }
                                    else
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }
                                }

                                //terminate and clear tasks
                                DAQmxStopTask(taskHandle);
                                DAQmxClearTask(taskHandle);
                            }

                            while(counterx2!=commands.getSamplePerChanToX2())
                            {
                                // recreate task only for x2
                                DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
//                                DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port1/line1","",DAQmx_Val_ChanForAllLines));
                                DAQmxErrChk (DAQmxStartTask(taskHandle));

                                // for less pulses
                                if (counterx2<commands.getSamplePerChanToX2())
                                {
                                    counterx2Diff = commands.getSamplePerChanToX2() - counterx2;
                                    cout << "Difference: " << counterx2Diff << endl;
                                    cout << "Correcting Revs..." << '\n' << '\n';
                                    Sleep(1000);

                                    for (int i=0; i < counterx2Diff; i++)
                                        {
                                            // reading limit switches
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle9,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL)); // fwd limit switch x2
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx2,NULL)); // rwd limit switch x2

                                            if (fwdLimitx2==0)
                                            {
                                                cout << "Foward x2 Limit switch reached" << '\n';
                                                if (commands.voltonx2==1)
                                                {
                                                    counterx2 = counterx2 * 3.125;
                                                    commands.x2Count = commands.x2Count+counterx2;
                                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                    ofstream out ("x2.txt");
                                                    out << commands.x2Count;
                                                    out.close();
                                                }

                                                if (commands.voltonx2==2)
                                                {
                                                    counterx2 = counterx2 * 3.125;
                                                    commands.x2Count = commands.x2Count-counterx2;
                                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                    ofstream out ("x2.txt");
                                                    out << commands.x2Count;
                                                    out.close();
                                                }
                                                exit(1);
                                            }
                                            else if (rwdLimitx2==0)
                                            {
                                                cout << "Rearward x2 limit switch reached" << '\n';
                                                commands.x2Count=0;
                                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                ofstream out ("x2.txt");
                                                out << commands.x2Count;
                                                out.close();
                                                exit(1);
                                            }

                                            // generate pulses
                                            datax=0xffffffff;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i) // delay 0.08s
                                            datax = 0;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i); // delay

                                            // reading x2 encoder data
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                            encoder_arrayx2[i] = datacounterx2;
                                            if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                            {
                                                new_counterx2++;
                                            }
                                        }

                                        // encoder x2 feedback
                                        counterx2 = counterx2 + (new_counterx2 * 3.125); // calculates new counterx
                                        new_counterx2 = 0;
                                        cout << "x2 final count: " << counterx2 << '\n';
                                }

                                // for more pulses
                                if (counterx2>commands.getSamplePerChanToX2())
                                {
                                    counterx2Diff = (counterx2 - commands.getSamplePerChanToX2());
                                    cout << "Delta: " << counterx2Diff << endl;
                                    cout << "Correcting Revs..." << '\n' << '\n';
                                    Sleep(1000);

                                    //set reverse direction
                                    if (commands.getvoltonx2()==1)
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }
                                    else if (commands.getvoltonx2()==2)
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                    }

                                        for (int i=0; i < counterx2Diff; i++)
                                        {
                                            // reading limit switches
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL)); // fwd limit switch x2
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL)); // rwd limit switch x2

                                            if (fwdLimitx2==0)
                                            {
                                                cout << "Foward x2 Limit switch reached" << '\n';
                                                if (commands.voltonx2==1)
                                                {
                                                    counterx2 = counterx2 * 3.125;
                                                    commands.x2Count = commands.x2Count+counterx2;
                                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                    ofstream out ("x2.txt");
                                                    out << commands.x2Count;
                                                    out.close();
                                                }

                                                if (commands.voltonx2==2)
                                                {
                                                    counterx2 = counterx2 * 3.125;
                                                    commands.x2Count = commands.x2Count-counterx2;
                                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                    ofstream out ("x2.txt");
                                                    out << commands.x2Count;
                                                    out.close();
                                                }
                                                exit(1);
                                            }
                                            else if (rwdLimitx2==0)
                                            {
                                                cout << "Rearward x2 limit switch reached" << '\n';
                                                commands.x2Count=0;
                                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                                ofstream out ("x2.txt");
                                                out << commands.x2Count;
                                                out.close();
                                                exit(1);
                                            }

                                            // generate pulses
                                            datax=0xffffffff;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i) // delay 0.08s
                                            datax = 0;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                                            for (int i=0; i < 1000000; ++i); // delay

                                            // reading encoder data
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                                            encoder_arrayx2[i] = datacounterx2;
                                            if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                                            {
                                                new_counterx2++;
                                            }
                                        }

                                        // encoder feedback
                                        counterx2 = counterx2 - (new_counterx2 * 3.125);
                                        new_counterx2 = 0;
                                        cout << "x2 final count: " << counterx2 << '\n';

                                        //reset direction
                                        if (commands.getvoltonx2()==1)
                                        {
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                        }
                                        else if (commands.getvoltonx2()==2)
                                        {
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datax1dir,&writtenx1dir,NULL));
                                        }
                                }

                                //terminate and clear tasks
                                DAQmxStopTask(taskHandle);
                                DAQmxClearTask(taskHandle);
                            }

                            // recording x1 data to file
                            if (commands.voltonx1==1)
                            {
                                commands.x1Count = commands.x1Count+counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }

                            if (commands.voltonx1==2)
                            {
                                commands.x1Count = commands.x1Count-counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }

                            // recording x2 data to file
                            if (commands.voltonx2==1)
                            {
                                commands.x2Count = commands.x2Count+counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }

                            if (commands.voltonx2==2)
                            {
                                commands.x2Count = commands.x2Count-counterx2;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                            }

                            // reset variables
                            counterx1 = 0;
                            counterx2 = 0;
                        }


                        //terminate tasks
                        DAQmxStopTask(taskHandle);
                        DAQmxStopTask(taskHandle2);
                        DAQmxStopTask(taskHandle3);
                        DAQmxStopTask(taskHandle4);
                        DAQmxStopTask(taskHandle5);
                        DAQmxStopTask(taskHandle6);
                        DAQmxStopTask(taskHandle7);
                        DAQmxStopTask(taskHandle8);
                        DAQmxStopTask(taskHandle9);
                        DAQmxClearTask(taskHandle);
                        DAQmxClearTask(taskHandle2);
                        DAQmxClearTask(taskHandle3);
                        DAQmxClearTask(taskHandle4);
                        DAQmxClearTask(taskHandle5);
                        DAQmxClearTask(taskHandle6);
                        DAQmxClearTask(taskHandle7);
                        DAQmxClearTask(taskHandle8);
                        DAQmxClearTask(taskHandle9);
                    }

                }

                cout << "Sleeping" << '\n' << endl;
                Sleep(1000);

                if (j==1)
                {

                    commands.numOfRevsy = commands.arrayA[i][j]*400; // 400 pulses per rev
                    cout << "Number of Pulses Sent to y: " << commands.numOfRevsy << '\n';

                    commands.setyAutoDrive();
                    if (commands.getSamplePerChanToY()>0)
                    {
                        uInt32      datay=0xffffffff;
                        int32		writteny;

                        // create tasks
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle3));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle4));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));

                        // create pulse generation through digital pulses
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getyDrive(),"",DAQmx_Val_ChanForAllLines));

                        // create digital output for direction
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getyDirection(),"",DAQmx_Val_ChanForAllLines));

                        // create digital input to read pulses from encoder
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev2/port0/line2","",DAQmx_Val_ChanForAllLines)); // encoder y

                        // create digital inputs for limit switches
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev2/port0/line5","",DAQmx_Val_ChanForAllLines)); // fwd limit switches y
                        DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev2/port0/line4","",DAQmx_Val_ChanForAllLines)); // rwd limit switches y

                        // setting direction
                        cout << "Setting y_Motion Direction... " << endl;
                        DAQmxErrChk (DAQmxStartTask(taskHandle2));
                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltony(),1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));

                        // start tasks to rev motors and monitor encoders + limit switches
                        cout << '\n' << '\n';
                        cout << "Revving Motor in y..." << '\n' << '\n';
                        cout << "Encoder Reading..." << '\n' << '\n';
                        DAQmxErrChk (DAQmxStartTask(taskHandle));
                        DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder y
                        DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch y
                        DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch y


                        // generate pulses, read encoder, limit switches
                        for (int i=0; i < commands.getSamplePerChanToY(); ++i)
                        {
                            // activate/deactivate limit switches depending on home setting
                            if ((commands.yCount!=0 && countery>100) || (commands.yCount!=33000 && countery>100))
                                {
                                    // limit switches
                                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimity,1,&readfwdLimity,NULL));
                                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimity,1,&readrwdLimity,NULL));

                                    if (fwdLimity==0)
                                    {
                                        cout << "Foward y Limit switch reached" << '\n';
                                        if (commands.voltony==1)
                                        {
                                            cout << "checking        " << commands.yCount << '\n';
                                            countery = countery * 3.125;
                                            commands.yCount = commands.yCount+countery;
                                            cout << "Recorded yCount: " << commands.yCount << '\n';
                                            ofstream out ("y.txt");
                                            out << commands.yCount;
                                            out.close();
                                        }

                                        if (commands.voltony==2)
                                        {
                                            countery = countery * 3.125;
                                            commands.yCount = commands.yCount-countery;
                                            cout << "Recorded yCount: " << commands.yCount << '\n';
                                            ofstream out ("y.txt");
                                            out << commands.yCount;
                                            out.close();
                                        }
                                        exit(1);
                                    }

                                    if (rwdLimity==0)
                                    {
                                        cout << "Rearward y limit switch reached" << '\n';
                                        commands.yCount=0;
                                        cout << "Recorded yCount: " << commands.yCount << '\n';
                                        ofstream out ("y.txt");
                                        out << commands.yCount;
                                        out.close();
                                        exit(1);
                                    }


                                }

                                // generate pulse
                                datay=0xffffffff;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 200000; ++i) // delay
                                datay = 0;
                                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                for (int i=0; i < 200000; ++i); // delay

                                // reading x2 encoder data
                                DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                                encoder_arrayy[i] = datacountery;
                                if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                                {
                                    countery++;
                                }
                            }

                        // y encoder feedback
                        countery = countery * 3.125;
                        cout << "The number of pulses counted = " << setw(6) << countery << '\n';


                        // correct y stepper motor position based on encoder feedback
                        while(countery!=commands.getSamplePerChanToY())
                        {
                            // for less pulses
                            if (countery<commands.getSamplePerChanToY())
                            {
                                counteryDiff = commands.getSamplePerChanToY() - countery;
                                cout << "Difference: " << counteryDiff << endl;
                                cout << "Correcting Revs..." << '\n' << '\n';
                                Sleep(1000);

                                for (int i=0; i < counteryDiff; i++)
                                    {
                                            // reading limit switches
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx2,NULL));

                                            if (fwdLimity==0)
                                            {
                                                cout << "Foward y Limit switch reached" << '\n';
                                                if (commands.voltony==1)
                                                {
                                                    cout << "checking        " << commands.yCount << '\n';
                                                    countery = countery * 3.125;
                                                    commands.yCount = commands.yCount+countery;
                                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                                    ofstream out ("y.txt");
                                                    out << commands.yCount;
                                                    out.close();
                                                }

                                                if (commands.voltony==2)
                                                {
                                                    countery = countery * 3.125;
                                                    commands.yCount = commands.yCount-countery;
                                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                                    ofstream out ("y.txt");
                                                    out << commands.yCount;
                                                    out.close();
                                                }
                                                exit(1);
                                            }

                                            if (rwdLimity==0)
                                            {
                                                cout << "Rearward y limit switch reached" << '\n';
                                                commands.yCount=0;
                                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                                ofstream out ("y.txt");
                                                out << commands.yCount;
                                                out.close();
                                                exit(1);
                                            }


                                            // generate pulses
                                            datay=0xffffffff;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                            for (int i=0; i < 200000; ++i) // delay 0.08s
                                            datay = 0;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                            for (int i=0; i < 200000; ++i); // delay

                                            // reading y encoder data
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                                            encoder_arrayy[i] = datacountery;
                                            if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                                            {
                                                new_countery++;
                                            }
                                        }

                                        // encoder y feedback
                                        countery = countery + (new_countery * 3.125); // calculates new counterx
                                        new_countery = 0;
                                        cout << "y final count: " << countery << '\n';
                                }

                            // for more pulses
                            if (countery>commands.getSamplePerChanToY())
                            {
                                    counteryDiff = (countery - commands.getSamplePerChanToY());
                                    cout << "Delta: " << counteryDiff << endl;
                                    cout << "Correcting Revs..." << '\n' << '\n';
                                    Sleep(1000);

                                    //set reverse direction
                                    if (commands.getvoltony()==1)
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                    }
                                    else
                                    {
                                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                    }

                                        for (int i=0; i < counteryDiff; i++)
                                        {
                                            // reading limit switches
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL));
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL));

                                            if (fwdLimity==0)
                                            {
                                                cout << "Foward y Limit switch reached" << '\n';
                                                if (commands.voltony==1)
                                                {
                                                    cout << "checking        " << commands.yCount << '\n';
                                                    countery = countery * 3.125;
                                                    commands.yCount = commands.yCount+countery;
                                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                                    ofstream out ("y.txt");
                                                    out << commands.yCount;
                                                    out.close();
                                                }

                                                if (commands.voltony==2)
                                                {
                                                    countery = countery * 3.125;
                                                    commands.yCount = commands.yCount-countery;
                                                    cout << "Recorded yCount: " << commands.yCount << '\n';
                                                    ofstream out ("y.txt");
                                                    out << commands.yCount;
                                                    out.close();
                                                }
                                                exit(1);
                                            }

                                            if (rwdLimity==0)
                                            {
                                                cout << "Rearward y limit switch reached" << '\n';
                                                commands.yCount=0;
                                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                                ofstream out ("y.txt");
                                                out << commands.yCount;
                                                out.close();
                                                exit(1);
                                            }


                                            // generate pulses
                                            datay=0xffffffff;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                            for (int i=0; i < 200000; ++i) // delay 0.08s
                                            datay = 0;
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                            for (int i=0; i < 200000; ++i); // delay

                                            // reading encoder data
                                            DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                                            encoder_arrayy[i] = datacountery;
                                            if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                                            {
                                                new_countery++;
                                            }
                                        }

                                        // encoder feedback
                                        countery = countery - (new_countery * 3.125);
                                        new_countery = 0;
                                        cout << "y final count: " << countery << '\n';

                                        //reset direction
                                        if (commands.getvoltony()==1)
                                        {
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,1,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                        }
                                        else
                                        {
                                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,2,1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                                        }
                                }

                        }

                        // recording x2 data to file
                        if (commands.voltony==1)
                        {
                                commands.yCount = commands.yCount+countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }

                        if (commands.voltony==2)
                        {
                                commands.yCount = commands.yCount-countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }

                        countery = 0;

                    }

                    //terminate tasks
                    DAQmxStopTask(taskHandle);
                    DAQmxStopTask(taskHandle2);
                    DAQmxStopTask(taskHandle3);
                    DAQmxStopTask(taskHandle4);
                    DAQmxStopTask(taskHandle5);
                    DAQmxClearTask(taskHandle);
                    DAQmxClearTask(taskHandle2);
                    DAQmxClearTask(taskHandle3);
                    DAQmxClearTask(taskHandle4);
                    DAQmxClearTask(taskHandle5);
                }

                else if (j>1)
                {
                    commands.numOfRevsz = commands.arrayA[i][j]*400; // 400 pulses per rev
                    cout << "Number of Pulses Sent to z: " << commands.numOfRevsz << '\n';

                    commands.setzAutoDrive();
                    if(commands.getSamplePerChanToZ()>0)
                    {
                        commands.setz();

                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));
                        DAQmxErrChk (DAQmxCreateTask("",&taskHandle6));

                        // create pulse generation through digital pulses
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle5,commands.getzDrive(),"",DAQmx_Val_ChanForAllLines));

                        // create digital output for direction
                        DAQmxErrChk (DAQmxCreateDOChan(taskHandle6,commands.getzDirection(),"",DAQmx_Val_ChanForAllLines));

                        cout << "Setting z_Motion Direction... " << endl;
                        DAQmxErrChk (DAQmxStartTask(taskHandle6));
                        DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle6,commands.getvoltonz(),1,10.0,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));

                        cout << "Revving Motor in z..." << '\n' << '\n';
                        DAQmxErrChk (DAQmxStartTask(taskHandle5));

                        for (int i=0; i < commands.getSamplePerChanToZ(); ++i)
                        {
                            dataz=0xffffffff;
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle5,1,1,-1,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));
                            for (int i=0; i < 1000000; ++i) // delay 0.08s

                            dataz = 0;
                            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle5,1,1,-1,DAQmx_Val_GroupByChannel,&dataz,&writtenz,NULL));
                            for (int i=0; i < 1000000; ++i); // delay
                        }

                        //terminate tasks
                        DAQmxStopTask(taskHandle5);
                        DAQmxStopTask(taskHandle6);
                        DAQmxClearTask(taskHandle5);
                        DAQmxClearTask(taskHandle6);
                    }


                }

            }
            cout << '\n';
        }
    }

    if (setting == 3)
    {
        commands.resetPosX();
        commands.resetPosY();

        if (commands.resetX==1)
        {
            // create tasks
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle3)); // encoder x1
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle4)); // fwd limit switch x1
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle5)); // rwd limit switch x1
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle6)); // encoder x2
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle7)); // fwd limit switch x2
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle8)); // rwd limit switch x2

            // create pulse generation through digital pulses
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getxDrive(),"",DAQmx_Val_ChanForAllLines));

            // create digital output for direction
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getx1Direction(),"",DAQmx_Val_ChanForAllLines));

            // create digital input to read pulses from encoder
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev1/port0/line0","",DAQmx_Val_ChanForAllLines)); // encoder x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle6,"Dev1/port0/line3","",DAQmx_Val_ChanForAllLines)); // encoder x2

            // create digital inputs for limit switches
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev1/port0/line1","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev1/port0/line2","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x1
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle7,"Dev2/port0/line1","",DAQmx_Val_ChanForAllLines)); // fwd limit switch x2
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle8,"Dev2/port0/line0","",DAQmx_Val_ChanForAllLines)); // rwd limit switch x2


            // setting direction
            cout << "Setting x_Motion Direction... " << endl;
            DAQmxErrChk (DAQmxStartTask(taskHandle2));
            if (commands.getxDrivectr()=="Dev1/port1/line0:1")
            {
                DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltonx1(),1,10.0,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
            }

            // start tasks to rev motors and monitor encoders + limit switches
            cout << '\n' << '\n';
            cout << "Revving Motor in x..." << '\n' << '\n';
            cout << "Encoder Reading..." << '\n' << '\n';
            DAQmxErrChk (DAQmxStartTask(taskHandle));
            DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder x1
            DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch x1
            DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch x1
            DAQmxErrChk (DAQmxStartTask(taskHandle6)); // encoder x2
            DAQmxErrChk (DAQmxStartTask(taskHandle7)); // fwd limit switch x2
            DAQmxErrChk (DAQmxStartTask(taskHandle8)); // rwd limit switch x2

            for (int i=0; i < commands.getSamplePerChanToX1(); ++i)
            {
                    // reading limit switches
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx1,1,&readfwdLimitx1,NULL)); // fwd limit switch x1
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx1,1,&readrwdLimitx1,NULL)); // rwd limit switch x1
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle7,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimitx2,1,&readfwdLimitx2,NULL)); // fwd limit switch x2
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle8,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimitx2,1,&readrwdLimitx2,NULL)); // rwd limit switch x2

                    // reading limit switches
                    if ((counterx1>40 || fwdLimitx1==0) || (counterx2>40 || fwdLimitx2==0))
                    {
                        if (fwdLimitx1==0)
                        {
                            cout << "Foward x1 Limit switch reached" << '\n';
                            if (commands.voltonx1==1)
                            {
                                commands.x1Count = commands.x1Count+counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }

                            if (commands.voltonx1==2)
                            {
                                commands.x1Count = commands.x1Count-counterx1;
                                cout << "Recorded x1Count: " << commands.x1Count << '\n';
                                ofstream out ("x1.txt");
                                out << commands.x1Count;
                                out.close();
                            }
                            break;
                        }
                        if (rwdLimitx1==0)
                        {
                        cout << "Rearward x1 limit switch reached" << '\n';
                        commands.x1Count=0;
                        cout << "Recorded x1Count: " << commands.x1Count << '\n';
                        ofstream out ("x1.txt");
                        out << commands.x1Count;
                        out.close();
                        break;
                        }

                        if (fwdLimitx2==0)
                            {
                                cout << "Foward x2 Limit switch reached" << '\n';
                                if (commands.voltonx2==1)
                                {
                                    counterx2 = counterx2 * 3.125;
                                    commands.x2Count = commands.x2Count+counterx2;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                }

                                if (commands.voltonx2==2)
                                {
                                    counterx2 = counterx2 * 3.125;
                                    commands.x2Count = commands.x2Count-counterx2;
                                    cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                    ofstream out ("x2.txt");
                                    out << commands.x2Count;
                                    out.close();
                                }
                                break;
                            }
                        if (rwdLimitx2==0)
                            {
                                cout << "Rearward x2 limit switch reached" << '\n';
                                commands.x2Count=0;
                                cout << "Recorded x2Count: " << commands.x2Count << '\n';
                                ofstream out ("x2.txt");
                                out << commands.x2Count;
                                out.close();
                                break;
                            }
                    }

                    // generate pulses
                    datax=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i) // delay 0.08s
                    datax = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datax,&writtenx,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading x1 encoder data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx1,1,&readcounterx1,NULL));
                    encoder_arrayx1[i] = datacounterx1;
                    if(encoder_arrayx1[i]==1 && encoder_arrayx1[i-1]!=1)
                    {
                        counterx1++;
                    }

                    // reading encoder x2 data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle6,-1,-1,DAQmx_Val_GroupByChannel,&datacounterx2,1,&readcounterx2,NULL));
                    encoder_arrayx2[i] = datacounterx2;
                    if(encoder_arrayx2[i]==8 && encoder_arrayx2[i-1]!=8)
                    {
                        counterx2++;
                    }
            }



            //terminate tasks
            DAQmxStopTask(taskHandle);
            DAQmxStopTask(taskHandle2);
            DAQmxStopTask(taskHandle3);
            DAQmxStopTask(taskHandle4);
            DAQmxStopTask(taskHandle5);
            DAQmxStopTask(taskHandle6);
            DAQmxStopTask(taskHandle8);
            DAQmxClearTask(taskHandle);
            DAQmxClearTask(taskHandle2);
            DAQmxClearTask(taskHandle3);
            DAQmxClearTask(taskHandle4);
            DAQmxClearTask(taskHandle5);
            DAQmxClearTask(taskHandle6);
            DAQmxClearTask(taskHandle8);
        }

        if (commands.resetY==1)
        {
            uInt32      datay=0xffffffff;
            int32		writteny;

            // create tasks
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle2));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle3));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle4));
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle5));


            // create pulse generation through digital pulses
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle,commands.getyDrive(),"",DAQmx_Val_ChanForAllLines));

            // create digital output for direction
            DAQmxErrChk (DAQmxCreateDOChan(taskHandle2,commands.getyDirection(),"",DAQmx_Val_ChanForAllLines));

            // create digital input to read pulses from encoder
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle3,"Dev2/port0/line2","",DAQmx_Val_ChanForAllLines)); // encoder y

            // create digital inputs for limit switches
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle4,"Dev2/port0/line5","",DAQmx_Val_ChanForAllLines)); // fwd limit switches y
            DAQmxErrChk (DAQmxCreateDIChan(taskHandle5,"Dev2/port0/line4","",DAQmx_Val_ChanForAllLines)); // rwd limit switches y

            // setting direction
            cout << "Setting y_Motion Direction... " << endl;
            DAQmxErrChk (DAQmxStartTask(taskHandle2));
            DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle2,commands.getvoltony(),1,10.0,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));


            // start tasks to rev motors and monitor encoders + limit switches
            cout << '\n' << '\n';
            cout << "Revving Motor in y..." << '\n' << '\n';
            cout << "Encoder Reading..." << '\n' << '\n';
            DAQmxErrChk (DAQmxStartTask(taskHandle));
            DAQmxErrChk (DAQmxStartTask(taskHandle3)); // encoder y
            DAQmxErrChk (DAQmxStartTask(taskHandle4)); // fwd limit switch y
            DAQmxErrChk (DAQmxStartTask(taskHandle5)); // rwd limit switch y

            for (int i=0; i < commands.getSamplePerChanToY(); ++i)
            {
                    // reading limit switches
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle4,-1,-1,DAQmx_Val_GroupByChannel,&fwdLimity,1,&readfwdLimity,NULL)); // fwd limit switch y
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle5,-1,-1,DAQmx_Val_GroupByChannel,&rwdLimity,1,&readrwdLimity,NULL)); // rwd limit switch y

                    // reading limit switches
                    if (countery>40 || fwdLimity==0)
                    {
                         if (fwdLimity==0)
                        {
                            cout << "Foward y Limit switch reached" << '\n';
                            if (commands.voltony==1)
                            {
                                commands.yCount = commands.yCount+countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }

                            if (commands.voltony==2)
                            {
                                commands.yCount = commands.yCount-countery;
                                cout << "Recorded yCount: " << commands.yCount << '\n';
                                ofstream out ("y.txt");
                                out << commands.yCount;
                                out.close();
                            }
                            break;
                        }

                        if (rwdLimity==0)
                        {
                        cout << "Rearward y limit switch reached" << '\n';
                        commands.yCount=0;
                        cout << "Recorded yCount: " << commands.yCount << '\n';
                        ofstream out ("y.txt");
                        out << commands.yCount;
                        out.close();
                        break;
                        }

                    }

                    // generate pulses
                    datay=0xffffffff;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                    for (int i=0; i < 200000; ++i) // delay 0.08s
                    datay = 0;
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,-1,DAQmx_Val_GroupByChannel,&datay,&writteny,NULL));
                    for (int i=0; i < 200000; ++i); // delay

                    // reading x1 encoder data
                    DAQmxErrChk (DAQmxReadDigitalU32(taskHandle3,-1,-1,DAQmx_Val_GroupByChannel,&datacountery,1,&readcountery,NULL));
                    encoder_arrayy[i] = datacountery;
                    if(encoder_arrayy[i]==4 && encoder_arrayy[i-1]!=4)
                    {
                        countery++;
                    }

            }


            //terminate tasks
            DAQmxStopTask(taskHandle);
            DAQmxStopTask(taskHandle2);
            DAQmxStopTask(taskHandle3);
            DAQmxStopTask(taskHandle4);
            DAQmxStopTask(taskHandle5);
            DAQmxStopTask(taskHandle6);
            DAQmxStopTask(taskHandle8);
            DAQmxClearTask(taskHandle);
            DAQmxClearTask(taskHandle2);
            DAQmxClearTask(taskHandle3);
            DAQmxClearTask(taskHandle4);
            DAQmxClearTask(taskHandle5);
            DAQmxClearTask(taskHandle6);
            DAQmxClearTask(taskHandle8);
        }


    }

    //create terminations for all tasks!
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxStopTask(taskHandle2);
		DAQmxStopTask(taskHandle3);
		DAQmxStopTask(taskHandle4);
		DAQmxStopTask(taskHandle5);
		DAQmxStopTask(taskHandle6);
		DAQmxStopTask(taskHandle8);
		DAQmxClearTask(taskHandle);
		DAQmxClearTask(taskHandle2);
		DAQmxClearTask(taskHandle3);
		DAQmxClearTask(taskHandle4);
		DAQmxClearTask(taskHandle5);
		DAQmxClearTask(taskHandle6);
		DAQmxClearTask(taskHandle8);
	}
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle3);
		DAQmxClearTask(taskHandle3);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
//	return 0;
}

void Startup::ESP()
{
    // sets the bitset to the number of addresses used
    int32       error=0;
    TaskHandle	taskHandle=0;
    TaskHandle	taskHandle2=0;
    uInt32      data=0xffffffff;
    int32		written;

    //record data
    ofstream MyExcelFile;
    MyExcelFile.open("C:\\Users\\Hesham\\Documents\\Engineering\\Northumbria University\\PhD Research\\Data Aquisition\\ESPdata.csv");
    MyExcelFile << "Iteration, chan01, Time Stamp" << endl; // sets excel file headers

    // constant variables
    int address[6];
    string setAddressNum;
    string setAddressPort;
    string finalAddress;
    const char* cfinalAddress;

    //read configuration file
    readConfig commands;
    commands.setAllConfig();

    for (int i=0; i < commands.getIteration(); i = i+1)
    {
        cout << "iteration  " << i << endl;
        SYSTEMTIME st;
        GetSystemTime(&st);
        printf("System Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        for (int numofAddresses=0; numofAddresses<2; numofAddresses++)
        {
            bitset<6> bits(numofAddresses);
            //cout << bits << endl;

            // allocated the binary address to an array of integers
            for(int i=0; i<6; i++)
            {
                address[i] = bits[i];
                //cout << address[i] << endl;
            }

            // sends signal to the corresponding channels
            for(long j=0; j<6; j++)
            {
                DAQmxErrChk (DAQmxCreateTask("",&taskHandle)); // create task

                if (address[j]==1)
                {
                    stringstream convert;
                    convert << j;
                    setAddressNum = convert.str(); // convert to string
                    setAddressPort = "Dev1/port1/line"; // select address port
                    finalAddress = setAddressPort + setAddressNum;
                    cfinalAddress = finalAddress.c_str();
                    DAQmxErrChk (DAQmxCreateDOChan(taskHandle,cfinalAddress,"",DAQmx_Val_ChanForAllLines));
                    DAQmxErrChk (DAQmxStartTask(taskHandle));
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
                    DAQmxStopTask(taskHandle); //stops the task
                    DAQmxClearTask(taskHandle); //clears the task
                }

                else if (address[j]==0)
                {
                    stringstream convert;
                    convert << j;
                    setAddressNum = convert.str();
                    setAddressPort = "Dev1/port1/line";
                    finalAddress = setAddressPort + setAddressNum;
                    cfinalAddress = finalAddress.c_str();
                    DAQmxErrChk (DAQmxCreateDOChan(taskHandle,cfinalAddress,"",DAQmx_Val_ChanForAllLines));
                    DAQmxErrChk (DAQmxStartTask(taskHandle));
                    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,2,1,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
                    DAQmxStopTask(taskHandle); //stops the task
                    DAQmxClearTask(taskHandle); //clears the task
                }

            }

            /*********************************************/

            // reading the data of V0
            SYSTEMTIME st;
            DAQmxErrChk (DAQmxCreateTask("",&taskHandle2)); //creates a task
            DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle2,commands.getPhysChan(),"",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL)); // create channel(s) to measure RMS voltage
            DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle2,"",commands.getSamplingFreq(),DAQmx_Val_Rising,commands.getSamplingMode(),commands.getSamplePerChan())); // a digital edge produces each sample

            DAQmxErrChk (DAQmxStartTask(taskHandle2)); //starts the created task

            //printf("Logging samples continuously. Press Enter to interrupt\n");

            static int  totalRead=0;
            int32       read;//=0;
            double      *dataread;
            dataread = commands.getDataArray();

            DAQmxErrChk (DAQmxReadAnalogF64(taskHandle2,commands.getSamplePerChan(),10.0,DAQmx_Val_GroupByScanNumber,dataread,commands.getSampleRate(),&read,NULL));
            //printf("Acquiring samples: Samples Read %d\r",(int)(totalRead+=read));
            //cout << endl;
            //GetSystemTime(&st);
            //printf("System Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
            DAQmxStopTask(taskHandle2); //stops the task
            DAQmxClearTask(taskHandle2); //clears the task

            GetSystemTime(&st); // time stamp
            printf("System Time: %02d:%02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
            string strTime;
            char buffer [256];
            sprintf(buffer, "%02d:%02d:%02d:%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
            strTime = buffer;

            int columnNum = 0;

            cout << "checking   " << read*commands.getNumofChan() << endl;
            if (numofAddresses==0)
            {
                for (int j=0; j < (read*commands.getNumofChan()); ++j)
                {
                    MyExcelFile << j << "," << dataread[j] << endl;
                    columnNum = columnNum+commands.getNumofChan();
                }
            }

            else
            {
                for (int j=0; j < (read*commands.getNumofChan()); ++j)
                {
                    MyExcelFile << "," << "," << dataread[j] << endl;
                }
            }





           /* int arraySize = read*commands.getNumofChan()*commands.getIteration(); // specify array size for excel
            int col2 = 0;
            int col3 = 1;
            int col4 = 2;



                if(col2==j)
                {
                    MyExcelFile << i << "," << distance[j];
                    col2 = col2+commands.getNumofChan();
                }

                if(col3==j)
                {
                    MyExcelFile << "," << data[j];
                    col3 = col3+commands.getNumofChan();
                }

                if(col4==j)
                {
                    MyExcelFile << "," << data[j] << "," << strTime << endl;
                    col4 = col4+commands.getNumofChan();
                }*/


        }

    }



Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle); //stops the task
		DAQmxClearTask(taskHandle); //clears the task
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
}
