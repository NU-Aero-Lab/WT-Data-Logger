#ifndef READCONFIG_H
#define READCONFIG_H
using namespace std;
#include <iostream>
#include <stdint.h>
#pragma once
#include <Startup.h>

class readConfig
{
     public:

        int         getSampleRate();
        int         getSamplePerChan();
        int         getSamplingMode();
        int         getNumofChan();
        float       getSamplingFreq();
        double     *getDataArray();
        const char *getPhysChan();
        const char *getPhysChan2();
        int         getIteration();
        void        setAllConfig();

        void        setupInfo();

    protected:

    private:
        int         sampleRate;
        int         samplePerChan;
        int         sampleMode;
        int         numofChan;
        int         Iteration;
        float       sampleInterval;
        int         size;
        string      physChan;
        const char* strPhysChan;
};

#endif // READCONFIG_H
