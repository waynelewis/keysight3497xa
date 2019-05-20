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

#include <string>
#include <iostream>

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
        4, // maxAddr
        (int)NUM_KS3497xA_PARAMS,
        asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask | asynInt32ArrayMask | asynFloat64ArrayMask | asynOctetMask | asynDrvUserMask,
        asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask | asynInt32ArrayMask | asynFloat64ArrayMask | asynOctetMask,
        //ASYN_CANBLOCK,  // asynFlags
        0, // asynFlags
        1,  // autoconnect
        0,  // default priority
        0) // default stack size
{
    int status = asynSuccess;
    const char *functionName = "KS3497xA";

    std::cout << "number of parameters = " << NUM_KS3497xA_PARAMS << std::endl;
    std::cout << "KS3497xASerialNumber = " << KS3497xASerialNumber << std::endl;
    std::cout << "KS3497xANumDataPoints = " << KS3497xANumDataPoints << std::endl;

    if (pollTime == 0) pollTime = 1000;
    this->pollTime_ = pollTime / 1000.;
    
    /* Create parameters */
    createParam(KS3497xASerialNumberString,         asynParamOctet,     &KS3497xASerialNumber);
    createParam(KS3497xAManufacturerString,         asynParamOctet,     &KS3497xAManufacturer);
    createParam(KS3497xAModelString,     	        asynParamOctet,     &KS3497xAModel);
    createParam(KS3497xAIDNString,     	            asynParamOctet,     &KS3497xAIDN);
    createParam(KS3497xATriggerSourceString,        asynParamInt32,     &KS3497xATriggerSource);
    createParam(KS3497xACard1TypeString,            asynParamOctet,     &KS3497xACard1Type);
    createParam(KS3497xACard2TypeString,            asynParamOctet,     &KS3497xACard2Type);
    createParam(KS3497xACard3TypeString,            asynParamOctet,     &KS3497xACard3Type);
    createParam(KS3497xACardInput01_16SelectString, asynParamInt32,     &KS3497xACardInput01_16Select);
    createParam(KS3497xACardMonSelectString,        asynParamInt32,     &KS3497xACardMonSelect);
    createParam(KS3497xACardMonOnOffString,         asynParamInt32,     &KS3497xACardMonOnOff);
    createParam(KS3497xAInput101ValueString,        asynParamFloat64,   &KS3497xAInput101Value);
    createParam(KS3497xANumDataPointsString,        asynParamInt32,     &KS3497xANumDataPoints);

    std::cout << "KS3497xASerialNumber = " << KS3497xASerialNumber << std::endl;
    std::cout << "KS3497xANumDataPoints = " << KS3497xANumDataPoints << std::endl;

    epicsThreadCreate("KS3497xA",
                epicsThreadPriorityMedium,
                epicsThreadGetStackSize(epicsThreadStackMedium),
                (EPICSTHREADFUNC)pollerThreadC,
                this);
    setIntegerParam(KS3497xANumDataPoints, 0);

    // Connect to the low level asyn port
    status = pasynOctetSyncIO->connect(devicePortName, 0, &this->pasynUserKS, NULL);
    if (status != asynSuccess) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: error connecting to port %s\n",
            driverName, functionName, devicePortName);
        return;
    }

    // Request an initial read of metadata
    read_metadata_request = true;

    // Initialize the channel status
    // TODO: Read channel status from device on startup
    for (int i = 0; i < MAX_CARDS; i++)
        for (int j = 0; j < MAX_INPUTS; j++)
            card_input_active[i][j] = false;
}

void KS3497xA::pollerThread()
{
    //int i;
    //int status;

    while(1) {
        lock();
        // Read data from KS3497xA here
        if (read_metadata_request == true)
            read_metadata();
        read_data();
        unlock();
        epicsThreadSleep(pollTime_);
    }
}

void KS3497xA::read_data(void)
{
    static const char *functionName = "read_data";
}

void KS3497xA::read_metadata(void)
{
    size_t nWrite;
    asynStatus status;
    double timeout = 1.0;
    int eomReason;
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_COMMAND_LENGTH];
    std::string response_buf;
    std::string substring;
    std::string separator = ",";
    size_t response_length;
    static const char *functionName = "read_metadata";

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
     
    setStringParam(KS3497xAIDN, response);

    // Split up the response to get the individual parts
    response_buf = response;
    
    substring = response_buf.substr(0, response_buf.find(separator));
    setStringParam(
        KS3497xAManufacturer, 
        substring.c_str());
    response_buf.erase(
            0, 
            response_buf.find(separator) + separator.length());

    substring = response_buf.substr(0, response_buf.find(separator));
    setStringParam(
        KS3497xAModel, 
        substring.c_str());
    response_buf.erase(
            0, 
            response_buf.find(separator) + separator.length());

    substring = response_buf.substr(0, response_buf.find(separator));
    setStringParam(
        KS3497xASerialNumber, 
        substring.c_str());

    // Get the card type for slot 1
    sprintf(command, "SYST:CTYPE? 100\n");

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
     
    response_buf = response;

    response_buf.erase(
            0, 
            response_buf.find(separator) + separator.length());

    substring = response_buf.substr(0, response_buf.find(separator));
    if (substring == "0")
        substring = "None";
    setStringParam(
        KS3497xACard1Type, 
        substring.c_str());

    // Get the card type for slot 2
    sprintf(command, "SYST:CTYPE? 200\n");

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
     
    // Split up the response
    response_buf = response;
    
    response_buf.erase(
            0, 
            response_buf.find(separator) + separator.length());
    
    substring = response_buf.substr(0, response_buf.find(separator));
    if (substring == "0")
        substring = "None";
    setStringParam(
        KS3497xACard2Type, 
        substring.c_str());
    response_buf.erase(
        0, 
        response_buf.find(separator) + separator.length());

    // Get the card type for slot 3
    sprintf(command, "SYST:CTYPE? 300\n");

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
     
    // Split up the response
    response_buf = response;
    
    response_buf.erase(
            0, 
            response_buf.find(separator) + separator.length());

    substring = response_buf.substr(0, response_buf.find(separator));
    if (substring == "0")
        substring = "None";
    setStringParam(
        KS3497xACard3Type, 
        substring.c_str());

    // Disable future metadata reads
    read_metadata_request = false;

}

asynStatus KS3497xA::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    int card;
    int offset;
    const char* functionName = "writeInt32";

    pasynManager->getAddr(pasynUser, &card);

    asynPrint(pasynUser, ASYN_TRACE_ERROR,
        "%s:%s: [%s]: function=%d value=%d, card=%d\n",
        driverName, functionName, this->portName, function, value, card);

    if (function == KS3497xACardMonSelect) {
        select_monitor(card, value);
    }
    else if (function == KS3497xACardMonOnOff) {
        start_stop_monitor(value);
    }
    else {
        asynPrint(this->pasynUserSelf,ASYN_TRACE_ERROR,"%s:%s got illegal function %d\n", driverName, functionName,  function);
        return(asynError);
    }

    return status;
}
/*

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
*/

void KS3497xA::select_inputs(int card, int offset, int flags) {
    int i;
    int flag;

    
    // Populate the local channel selection info
    for (i == 0; i < 16; i++) {
        // Mask out each flag in turn
        flag = flags & (1 << i);
        card_input_active[card][i + offset] = flag;
    }
}

void KS3497xA::select_monitor(int card, int channel) {
    int i;
    int flag;

    char command[MAX_COMMAND_LENGTH];

    sprintf(command, "ROUT:MON (@%d%02d)\n", card, channel);

    send_command(command);
}

void KS3497xA::start_stop_monitor(int value) {
    int i;
    int flag;
    int status;

    char command[MAX_COMMAND_LENGTH];

    sprintf(command, "ROUT:MON:STATE %s\n", (value==0?"OFF":"ON"));

    send_command(command);
}

void KS3497xA::send_command(const char *command) {

    int status;
    double timeout = 1.0;
    size_t nWrite;
    const char *functionName = "send_command";

    status = pasynOctetSyncIO->write(
        this->pasynUserKS,
        command,
        strlen(command),
        timeout,
        &nWrite);

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
            "%s:%s: command = %s\n",
            driverName, functionName, command);
    
    if (status != asynSuccess) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: error sending command %s\n",
            driverName, functionName, command);
        return;
    }
}

extern "C" int initKS3497xA(const char *portName, const char *devicePortName, int pollTime)
{
    new KS3497xA(portName, devicePortName, pollTime);
    return (asynSuccess);
}

static const iocshArg initArg0 = {"Port Nmae", iocshArgString};
static const iocshArg initArg1 = {"Device Port Nmae", iocshArgString};
static const iocshArg initArg2 = {"Poll time (ms)", iocshArgInt};
static const iocshArg *const initArgs[] = {&initArg0, 
                                                 &initArg1,
                                                 &initArg2
                                                };
static const iocshFuncDef initFuncDef = {"initKS3497xA", 3, initArgs};

static void initCallFunc(const iocshArgBuf *args)
{
    initKS3497xA(args[0].sval, args[1].sval, args[2].ival);
}

void KS3497xARegister(void) 
{
    iocshRegister(&initFuncDef, initCallFunc);
}

extern "C" {
    epicsExportRegistrar(KS3497xARegister);
}
