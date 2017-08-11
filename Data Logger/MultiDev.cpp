/*#include <string.h>
#include <stdio.h>
#include <NIDAQmx.h>

static TaskHandle masterTaskHandle=0,slaveTaskHandle=0;


#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[]);

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData);

int main(void)
{
	int32       error=0;
	char        errBuff[2048]={'\0'};
	char	    str1[256],str2[256],trigName[256];
	float64     clkRate;
	// synchType indicates what device family the devices you are synching belong to:
	// 0 : E series
	// 1 : M series (PCI)
	// 2 : M series (PXI)
	// 3 : DSA Sample Clock Timebase
	// 4 : DSA Reference Clock
	uInt32 synchType=0;

 	/*********************************************/
	// DAQmx Configure Code
/*	/*********************************************/
/*	DAQmxErrChk (DAQmxCreateTask("",&masterTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(masterTaskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(masterTaskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	DAQmxErrChk (DAQmxCreateTask("",&slaveTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(slaveTaskHandle,"Dev10/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(slaveTaskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1000));
	switch( synchType ) {
		case 0: // E & S Series Sharing Master Timebase
			// Note:  PXI 6115 and 6120 (S Series) devices don't require sharing of master timebase,
			// because they auto-lock to Clock 10.  For those devices sharing a start trigger is adequate.
			// For the PCI-6154 S Series device use the M Series (PCI) synchronization type to synchronize
			// using the reference clock.
			DAQmxErrChk (DAQmxGetMasterTimebaseSrc(masterTaskHandle,str1,256));
			DAQmxErrChk (DAQmxGetMasterTimebaseRate(masterTaskHandle,&clkRate));
			DAQmxErrChk (DAQmxSetMasterTimebaseSrc(slaveTaskHandle,str1));
			DAQmxErrChk (DAQmxSetMasterTimebaseRate(slaveTaskHandle,clkRate));
			break;
		case 1: // M Series Sharing Reference Clock for PCI Devices
			DAQmxErrChk (DAQmxSetRefClkSrc(masterTaskHandle,"OnboardClock"));
			DAQmxErrChk (DAQmxGetRefClkSrc(masterTaskHandle,str1,256));
			DAQmxErrChk (DAQmxGetRefClkRate(masterTaskHandle,&clkRate));
			DAQmxErrChk (DAQmxSetRefClkSrc(slaveTaskHandle,str1));
			DAQmxErrChk (DAQmxSetRefClkRate(slaveTaskHandle,clkRate));
			break;
		case 2: // M Series Sharing Reference Clock for PXI Devices
			DAQmxErrChk (DAQmxSetRefClkSrc(masterTaskHandle,"PXI_Clk10"));
			DAQmxErrChk (DAQmxSetRefClkRate(masterTaskHandle,10000000.0));
			DAQmxErrChk (DAQmxSetRefClkSrc(slaveTaskHandle,"PXI_Clk10"));
			DAQmxErrChk (DAQmxSetRefClkRate(slaveTaskHandle,10000000.0));
			break;
		case 3: // DSA Sharing Sample Clock
			// Note:  If you are using PXI DSA Devices, the master device must reside in PXI Slot 2.
			DAQmxErrChk (GetTerminalNameWithDevPrefix(masterTaskHandle,"SampleClockTimebase",str1));
			DAQmxErrChk (GetTerminalNameWithDevPrefix(masterTaskHandle,"SyncPulse",str2));
			DAQmxErrChk (DAQmxSetSampClkTimebaseSrc(slaveTaskHandle,str1));
			DAQmxErrChk (DAQmxSetSyncPulseSrc(slaveTaskHandle,str2));
			break;
		case 4: // Reference clock 10 synchronization for DSA devices.
			// Note: Not all DSA devices support reference clock synchronization. Refer to your hardware
			// device manual for further information on whether this method of synchronization is supported
			// for your particular device
			DAQmxErrChk (DAQmxSetRefClkSrc(masterTaskHandle, "PXI_Clk10"));
			DAQmxErrChk (GetTerminalNameWithDevPrefix(masterTaskHandle,"SyncPulse",str1));
			DAQmxErrChk (DAQmxSetSyncPulseSrc(slaveTaskHandle, str1));
			DAQmxErrChk (DAQmxSetRefClkSrc(slaveTaskHandle, "PXI_Clk10"));
			break;
		default:
			break;
	}
	DAQmxErrChk (GetTerminalNameWithDevPrefix(masterTaskHandle,"ai/StartTrigger",trigName));
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(slaveTaskHandle,trigName,DAQmx_Val_Rising));

	DAQmxErrChk (DAQmxRegisterEveryNSamplesEvent(masterTaskHandle,DAQmx_Val_Acquired_Into_Buffer,1000,0,EveryNCallback,NULL));
	DAQmxErrChk (DAQmxRegisterDoneEvent(masterTaskHandle,0,DoneCallback,NULL));

	/*********************************************/
	// DAQmx Start Code
/*	/*********************************************/
	// The slave device is armed before the master so that the slave device does
	// not miss the trigger.
/*	DAQmxErrChk (DAQmxStartTask(slaveTaskHandle));
	DAQmxErrChk (DAQmxStartTask(masterTaskHandle));

	printf("Acquiring samples continuously. Press Enter to interrupt\n");
	printf("\nRead:\tMaster\tSlave\tTotal:\tMaster\tSlave\n");
	getchar();

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);

	/*********************************************/
	// DAQmx Stop Code
/*	/*********************************************/
/*	if( masterTaskHandle ) {
		DAQmxStopTask(masterTaskHandle);
		DAQmxClearTask(masterTaskHandle);
		masterTaskHandle = 0;
	}
	if( slaveTaskHandle ) {
		DAQmxStopTask(slaveTaskHandle);
		DAQmxClearTask(slaveTaskHandle);
		slaveTaskHandle = 0;
	}

	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit");
	getchar();
	return 0;
}

static int32 GetTerminalNameWithDevPrefix(TaskHandle taskHandle, const char terminalName[], char triggerName[])
{
	int32	error=0;
	char	device[256];
	int32	productCategory;
	uInt32	numDevices,i=1;

	DAQmxErrChk (DAQmxGetTaskNumDevices(taskHandle,&numDevices));
	while( i<=numDevices ) {
		DAQmxErrChk (DAQmxGetNthTaskDevice(taskHandle,i++,device,256));
		DAQmxErrChk (DAQmxGetDevProductCategory(device,&productCategory));
		if( productCategory!=DAQmx_Val_CSeriesModule && productCategory!=DAQmx_Val_SCXIModule ) {
			*triggerName++ = '/';
			strcat(strcat(strcpy(triggerName,device),"/"),terminalName);
			break;
		}
	}

Error:
	return error;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
	int32           error=0;
	char            errBuff[2048]={'\0'};
	static int32    masterTotal=0,slaveTotal=0;
	int32           masterRead,slaveRead;
	float64         masterData[1000],slaveData[1000];

	/*********************************************/
	// DAQmx Read Code
/*	/*********************************************/
/*	DAQmxErrChk (DAQmxReadAnalogF64(masterTaskHandle,1000,10.0,DAQmx_Val_GroupByChannel,masterData,1000,&masterRead,NULL));
	DAQmxErrChk (DAQmxReadAnalogF64(slaveTaskHandle,1000,10.0,DAQmx_Val_GroupByChannel,slaveData,1000,&slaveRead,NULL));

	if( masterRead>0 )
		masterTotal += masterRead;
	if( slaveRead>0 )
		slaveTotal += slaveRead;
	printf("\t%d\t%d\t\t%d\t%d\r",(int)masterRead,(int)slaveRead,(int)masterTotal,(int)slaveTotal);
	fflush(stdout);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		/*********************************************/
		// DAQmx Stop Code
/*		/*********************************************/
/*		if( masterTaskHandle ) {
			DAQmxStopTask(masterTaskHandle);
			DAQmxClearTask(masterTaskHandle);
		}
		if( slaveTaskHandle ) {
			DAQmxStopTask(slaveTaskHandle);
			DAQmxClearTask(slaveTaskHandle);
		}
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}

int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandle);
		if( slaveTaskHandle ) {
			DAQmxStopTask(slaveTaskHandle);
			DAQmxClearTask(slaveTaskHandle);
			slaveTaskHandle = 0;
		}
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}*/

