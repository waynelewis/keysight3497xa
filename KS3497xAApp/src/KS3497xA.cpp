// File: drvKS3497xA.cpp
// Author: Wayne Lewis
// Date: 2019-05-13
//
// Description:
// Device driver for KS 3497xA multiplexing DMM
//
// Revisions:
// 2019-05-13   WL  Original version
//
// Copyright:
// Lawrence Berkeley National Laboratory
//
// Licence:
// TBD

#include "KS3497xA.h"

#define NUM_KS3497xA_PARAMS ((int)(&LAST_KS3497XA_PARAM - &FIRST_KS3497XA_PARAM + 1))

KS3497xA::KS3497xA(const char *portName)
    : asynPortDriver(portName, 
        1, // maxAddr
        NUM_KS3497xA_PARAMS,
        asynInt32Mask | asynFloat64Mask | asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask | asynOctetMask | asynDrvUserMask,
        asynInt32Mask | asynFloat64Mask | asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask | asynOctetMask,
        ASYN_CANBLOCK,  // asynFlags
        1, // autoconnect
        0, // default priority
        0) // default stack size
{
    int status = asynSuccess;
    const char *functionName = "KS3497xA";

    /* Create parameters */
    createParam(KS3497xASerialNumberString,     asynParamOctet,     &KS3497xASerialNumber);
    createParam(KS3497xATriggerSourceString,    asynParamInt32,     &KS3497xATriggerSource);
    createParam(KS3497xANumDataPointsString,    asynParamInt32,     &KS3497xANumDataPoints);

}

asynStatus KS3497xA::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    asynStatus status = asynSuccess;
    return status;
}

asynStatus KS3497xA::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    asynStatus status = asynSuccess;
    return status;
}

asynStatus KS3497xA::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
    asynStatus status = asynSuccess;
    return status;
}

asynStatus KS3497xA::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    asynStatus status = asynSuccess;
    return status;
}

asynStatus KS3497xA::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual)
{
    asynStatus status = asynSuccess;
    return status;
}
