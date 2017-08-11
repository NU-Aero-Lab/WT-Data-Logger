#ifndef INTERFACE_H
#define INTERFACE_H
using namespace std;
#include <iostream>
#include <stdint.h>
#pragma once
#include <Startup.h>

class Interface
{
    friend class    Startup;
    public:
        int         getSampleRate();
        int         getSamplePerChan();
        int         getSamplingMode();
        int         getNumofChan();
        float       getSamplingFreq();
        double     *getDataArray();
        int         getIteration();
        const char *getPhysChan();
        void        setSampleInterval();
        void        setSampleRate();
        void        setSamplePerChan();
        void        setsamplingMode();
        void        setPhysChan();
        void        setNumofChan();
        void        setIteration();

        // traverse controls
        void        setxDrive();
        void        setxAutoDrive();
        void        setyDrive();
        void        setyAutoDrive();
        void        setzDrive();
        void        setzAutoDrive();
        const char *getxDrive();
        const char *getyDrive();
        const char *getzDrive();
        string      getxDrivectr();
        string      getyDrivectr();
        const char *getx1Direction();
        const char *getx2Direction();
        const char *getyDirection();
        const char *getzDirection();
        void        setSamplePerChanToX1();
        void        setSamplePerChanToX2();
        void        setSamplePerChanToY();
        void        setSamplePerChanToZ();
        double      getSamplePerChanToX1();
        double      getSamplePerChanToX2();
        double      getSamplePerChanToY();
        double      getSamplePerChanToZ();
        void        setx1();
        void        setx2();
        void        sety();
        void        setz();
        long        getvoltonx1();
        long        getvoltonx2();
        long        getvoltony();
        long        getvoltonz();
        void        resetPosX();
        void        resetPosY();
        int         getresetX();
        int         getresetY();
        void        setautoMotion();

        void        setupInfo(); // is this working or used??

    protected:

    private:
        int         sampleRate;
        int         samplePerChan;
        int         sampleMode;
        int         numofChan;
        int         Iteration;
        int         size;
        float       sampleInterval;
        string      physChan;
        const char* strPhysChan;

        // traverse controls
        string      xDrive;
        string      yDrive;
        string      zDrive;
        string      x1Direction;
        string      x2Direction;
        string      yDirection;
        string      zDirection;
        const char* strxDrive;
        const char* stryDrive;
        const char* strzDrive;
        const char* strx1Direction;
        const char* strx2Direction;
        const char* stryDirection;
        const char* strzDirection;
        double      numOfRevsx1;
        double      numOfRevsx2;
        double      numOfRevsy;
        double      numOfRevsz;
        long        x1=0;
        long        x1Count=0;
        long        x2=0;
        long        x2Count=0;
        long        y=0;
        long        yCount=0;
        long        z=0;
        long        zCount=0;
        long        x1_new=0;
        long        x2_new=0;
        long        y_new=0;
        long        z_new=0;
        string      linex1;
        string      linex2;
        string      liney;
        string      linez;
        long        voltonx1;
        long        voltonx2;
        long        voltony;
        long        voltonz;
        int         resetX;
        int         resetY;
        int rowA = 0;
        int colA = 0;
        string lineA;
        int counter;
        double arrayA[10][10] = {{0}};


};

#endif // SAMPLERATE_H
