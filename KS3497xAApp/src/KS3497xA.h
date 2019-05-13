#ifndef DRVKEYSIGHT3497XA_H
#define DRVKEYSIGHT3497XA_H

#include <iocsh.h>
#include <epicsExport.h>
#include <asynPortDriver.h>

#define KS3497xASerialNumberString      "SerialNumber"      // asynOctet
#define KS3497xATriggerSourceString     "TriggerSource"     // asynInt32    
#define KS3497xANumDataPointsString     "NumDataPoints"     // asynInt32    

class KS3497xA : public asynPortDriver {

public:
    KS3497xA(const char *portName);
    
    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);

protected:
    int KS3497xASerialNumber;
    #define FIRST_KS3497XA_PARAM KS3497xASerialNumber
    int KS3497xATriggerSource;
    int KS3497xANumDataPoints;
    #define LAST_KS3497XA_PARAM KS3497xANumDataPoints
};

#endif
