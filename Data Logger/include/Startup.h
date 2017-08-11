#ifndef STARTUP_H
#define STARTUP_H
#include <Interface.h>
#include <stdio.h>
#include <stdlib.h>

class Startup
{
    friend class    Interface;
    public:
        void        config();
        void        file();
        void        distance();
        void        multiDev();
        void        digitalOut();
        void        ESP();


    protected:

    private:
        char        errBuff[2048]={'\0'};
        char        incomingData[256] = "";			// don't forget to pre-allocate memory
        int         dataLength = 255;
        int         readResult = 0;
        char        test[8];

        //stepper motors
        unsigned long	    fwdLimitx1;
        signed long		    readfwdLimitx1;
        unsigned long	    rwdLimitx1;
        signed long		    readrwdLimitx1;
        unsigned long	    fwdLimitx2;
        signed long		    readfwdLimitx2;
        unsigned long	    rwdLimitx2;
        signed long		    readrwdLimitx2;
        unsigned long	    fwdLimity;
        signed long		    readfwdLimity;
        unsigned long	    rwdLimity;
        signed long		    readrwdLimity;

        unsigned long		datacounterx1;
        unsigned long		datacounterx2;
        unsigned long		datacountery;
        double              counterx1=0;
        double              countery=0;
        double              new_counterx1=0;
        double              new_countery=0;
        double              counterx1Diff;
        double              counteryDiff;
        int                 encoder_arrayx1[20000];
        int                 encoder_arrayy[20000];
        signed long 		readcounterx1;
        signed long 		readcountery;

        double              counterx2=0;
        double              new_counterx2=0;
        double              counterx2Diff;
        int                 encoder_arrayx2[20000];
        signed long 		readcounterx2;
};

#endif // STARTUP_H
