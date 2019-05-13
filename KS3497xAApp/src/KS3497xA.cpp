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

#include <string.h>
#include <stdlib.h>

#include <asynPortDriver.h>
#include <asynOctetSyncIO.h>
#include <epicsThread.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "KS3497xA.h"

#define NUM_KS3497xA_PARAMS ((int)(&LAST_KS3497XA_PARAM - &FIRST_KS3497XA_PARAM + 1))

static const char *driverName = "KS3497xA";

static void pollerThreadC(void *pPvt)
{
    KS3497xA *pKS3497xA = (KS3497xA *)pPvt;
    pKS3497xA->pollerThread();
}

KS3497xA::KS3497xA(const char *portName, const char *devicePortName, int pollTime)
    : asynPortDriver(portName, 
        1, // maxAddr
        NUM_KS3497xA_PARAMS,
        asynInt32Mask | asynFloat64Mask | asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask | asynOctetMask | asynDrvUserMask,
        asynInt32Mask | asynFloat64Mask | asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask | asynOctetMask,
        ASYN_CANBLOCK,  // asynFlags
        1,  // autoconnect
        0,  // default priority
        0) // default stack size
{
    int status = asynSuccess;
    const char *functionName = "KS3497xA";

    if (pollTime == 0) pollTime = 1000;
    this->pollTime_ = pollTime_ / 1000.;
    
    // Connect to the low level asyn port
    status = pasynOctetSyncIO->connect(devicePortName, 0, &this->pasynUserKS, NULL);
    if (status != asynSuccess) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: error connecting to port %s\n",
            driverName, functionName, devicePortName);
        return;
    }

    /* Create parameters */
    createParam(KS3497xASerialNumberString,     asynParamOctet,     &KS3497xASerialNumber);
    createParam(KS3497xAManufacturerString,     asynParamOctet,     &KS3497xAManufacturer);
    createParam(KS3497xAModelString,     	    asynParamOctet,     &KS3497xAModel);
    createParam(KS3497xAIDNString,     	        asynParamOctet,     &KS3497xAIDN);
    createParam(KS3497xATriggerSourceString,    asynParamInt32,     &KS3497xATriggerSource);
    createParam(KS3497xANumDataPointsString,    asynParamInt32,     &KS3497xANumDataPoints);


    epicsThreadCreate("KS3497xA",
                epicsThreadPriorityMedium,
                epicsThreadGetStackSize(epicsThreadStackMedium),
                (EPICSTHREADFUNC)pollerThreadC,
                this);
}

void KS3497xA::pollerThread()
{
    int i;
    int status;

    while(1) {
        lock();
        // Read data from KS3497xA here
        read_metadata();
        unlock();
        epicsThreadSleep(pollTime_);
    }
}

void KS3497xA::read_metadata(void)
{
    size_t nWrite;
    asynStatus status;
    double timeout = 1.0;
    int eomReason;
    char *response;
    char *command;
    size_t response_length;
    static const char *functionName = "read_metadata";

    // Set up the command and response buffers
    response = (char *)malloc(MAX_RESPONSE_LENGTH * sizeof(char));
    command = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));

    // Read the model information
    sprintf(command, "*IDN?\n");

    status = pasynOctetSyncIO->writeRead(
        this->pasynUserKS,
        command,
        strlen(command),
        response,
        MAX_RESPONSE_LENGTH,
        timeout,
        &nWrite,
        &response_length,
        &eomReason);
     
    printf("read_metadata: response = %s\n", response);

    setStringParam(KS3497xAIDN, response);

    // Split up the response
    char *p_ch;
    p_ch = strtok(response, ",");
    if (p_ch != NULL)
        setStringParam(KS3497xAManufacturer, p_ch);
    p_ch = strtok(NULL, ",");
    if (p_ch != NULL)
        setStringParam(KS3497xAModel, p_ch);
    p_ch = strtok(NULL, ",");
    if (p_ch != NULL)
        setStringParam(KS3497xASerialNumber, p_ch);

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

asynStatus KS3497xA::readOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual, int *eomReason)
{
    asynStatus status = asynSuccess;
    return status;
}
