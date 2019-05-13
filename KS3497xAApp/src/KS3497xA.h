#ifndef DRVKEYSIGHT3497XA_H
#define DRVKEYSIGHT3497XA_H

#include <iocsh.h>
#include <epicsExport.h>
#include <asynPortDriver.h>

#define KS3497XASerialNumberString      "SerialNumber"      // asynOctet
#define KS3497XATriggerSourceString     "TriggerSource"     // asynInt32    
#define KS3497XANumDataPointsString     "NumDataPoints"     // asynInt32    

class KS3497xA : public asynPortDriver {

public:
    KS3497xA(const char *portName);
    
    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet (asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);

protected:
    int P_serialNumber;
    #define FIRST_KS3497XA_PARAM P_serialNumber;
    int P_triggerSource;
    int P_numDataPoints;
    #define LAST_KS3497XA_PARAM P_numDataPoints;
    
};

#endif
