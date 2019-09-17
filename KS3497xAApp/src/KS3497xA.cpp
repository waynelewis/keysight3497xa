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
#include <algorithm>

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
        400, // maxAddr
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

    if (pollTime == 0) pollTime = 1000;
    this->pollTime_ = pollTime / 1000.;
    
    /* Create parameters */
    createParam(KS3497xASerialNumberString,         asynParamOctet,     &KS3497xASerialNumber);
    createParam(KS3497xAManufacturerString,         asynParamOctet,     &KS3497xAManufacturer);
    createParam(KS3497xAModelString,                asynParamOctet,     &KS3497xAModel);
    createParam(KS3497xAIDNString,                  asynParamOctet,     &KS3497xAIDN);
    createParam(KS3497xATriggerSourceString,        asynParamInt32,     &KS3497xATriggerSource);
    createParam(KS3497xACard1TypeString,            asynParamOctet,     &KS3497xACard1Type);
    createParam(KS3497xACard2TypeString,            asynParamOctet,     &KS3497xACard2Type);
    createParam(KS3497xACard3TypeString,            asynParamOctet,     &KS3497xACard3Type);
    createParam(KS3497xACardMonSelectString,        asynParamInt32,     &KS3497xACardMonSelect);
    createParam(KS3497xAMonOnOffString,             asynParamInt32,     &KS3497xAMonOnOff);
    createParam(KS3497xAMonValString,               asynParamFloat64,   &KS3497xAMonVal);
    createParam(KS3497xAInput101ValueString,        asynParamFloat64,   &KS3497xAInput101Value);
    createParam(KS3497xANumDataPointsString,        asynParamInt32,     &KS3497xANumDataPoints);
    createParam(KS3497xAInputTypeSelectString,      asynParamInt32,     &KS3497xAInputTypeSelect);
    createParam(KS3497xATCTypeSelectString,         asynParamInt32,     &KS3497xATCTypeSelect);
    createParam(KS3497xARTDTypeSelectString,        asynParamInt32,     &KS3497xARTDTypeSelect);
    createParam(KS3497xARTDRValueSelectString,      asynParamInt32,     &KS3497xARTDRValueSelect);
    createParam(KS3497xAInputTypeReadString,      	asynParamInt32,     &KS3497xAInputTypeRead);
    createParam(KS3497xATempTypeReadString,      	asynParamInt32,     &KS3497xATempTypeRead);
    createParam(KS3497xATCTypeReadString,         	asynParamInt32,     &KS3497xATCTypeRead);
    createParam(KS3497xAThermistorTypeSelectString, asynParamInt32,     &KS3497xAThermistorTypeSelect);
    createParam(KS3497xALastErrorMessageString,     asynParamOctet,     &KS3497xALastErrorMessage);
    createParam(KS3497xALastErrorCodeString,        asynParamInt32,     &KS3497xALastErrorCode);
    createParam(KS3497xAErrorMessageString,         asynParamOctet,     &KS3497xAErrorMessage);
    createParam(KS3497xAErrorCodeString,            asynParamInt32,     &KS3497xAErrorCode);

    // Create the parameters for the cards
    for (int i = 1; i < 4; i++) {
    createParam(i, KS3497xACardInput01_16SelectString, asynParamInt32,     &KS3497xACardInput01_16Select);
    }

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

    pasynOctetSyncIO->setInputEos(this->pasynUserKS, "\n", 1);
    pasynOctetSyncIO->setOutputEos(this->pasynUserKS, "\n" ,1);
            
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
        // Check for errors
        check_status();
        // Read data from KS3497xA here
        if (read_metadata_request == true)
            read_metadata();
        if (monitoring)
            read_data();
        unlock();
        epicsThreadSleep(pollTime_);
    }
}

asynStatus KS3497xA::check_status(void)
{
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_COMMAND_LENGTH];
    std::string response_buf;
    std::string substring;
    std::string separator = ",";
    int error_code;
    static const char *functionName = "check_status";

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
        "%s:%s: [%s]: entering\n",
        driverName, functionName, this->portName);

    sprintf(command, "SYST:ERR?");

    status = writeread_command(command, response);

    response_buf = response;
    substring = response_buf.substr(0, response_buf.find(separator));
    // TODO: Add validity checking here
    error_code = atoi(substring.c_str());

    // If the error code is non-zero, update the last error code
    // latching PVs
    if (error_code != NO_ERROR_CODE) {
        setStringParam(
                KS3497xALastErrorMessage, 
                response);

        setIntegerParam(
                KS3497xALastErrorCode, 
                error_code);
    }
        
    setStringParam(KS3497xAErrorMessage, response);
    setIntegerParam(KS3497xAErrorCode, error_code);
    callParamCallbacks();
    return status;
}

asynStatus KS3497xA::read_data(void)
{
    asynStatus status = asynSuccess;
    static const char *functionName = "read_data";

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
        "%s:%s: [%s]: entering\n",
        driverName, functionName, this->portName);

    status = read_monitor_data();

    return status;
}

asynStatus KS3497xA::read_metadata(void)
{
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_COMMAND_LENGTH];
    std::string response_buf;
    std::string substring;
    std::string separator = ",";
    static const char *functionName = "read_metadata";

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
        "%s:%s: [%s]: entering\n",
        driverName, functionName, this->portName);

    // Read the model information
    sprintf(command, "*IDN?");

    status = writeread_command(command, response);

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
    sprintf(command, "SYST:CTYPE? 100");

    status = writeread_command(command, response);

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
    sprintf(command, "SYST:CTYPE? 200");

    status = writeread_command(command, response);
     
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
    sprintf(command, "SYST:CTYPE? 300");

    status = writeread_command(command, response);
     
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

    if (status != asynSuccess)
        asynPrint(
            this->pasynUserSelf, 
            ASYN_TRACE_ERROR,
            "%s:%s: error reading metadata\n",
            driverName, functionName);
            
    callParamCallbacks();

    return status;
}

asynStatus KS3497xA::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    asynStatus status = asynSuccess;
    int function = pasynUser->reason;
    int addr;
    const char* functionName = "writeInt32";

    pasynManager->getAddr(pasynUser, &addr);

    asynPrint(pasynUser, ASYN_TRACE_FLOW,
        "%s:%s: [%s]: function=%d value=%d, card=%d\n",
        driverName, functionName, this->portName, function, value, addr);

    if (function == KS3497xACardInput01_16Select) {
        select_inputs(addr, value);
    }
    else if (function == KS3497xACardMonSelect) {
        status = select_monitor(addr, value);
    }
    else if (function == KS3497xAMonOnOff) {
        status = start_stop_monitor(value);
    }
    else if (function == KS3497xAInputTypeSelect
            || function == KS3497xATCTypeSelect
            || function == KS3497xARTDTypeSelect
            || function == KS3497xARTDRValueSelect
            || function == KS3497xAThermistorTypeSelect) {

		// Save the value passed in
		setIntegerParam(function, value);
		
        status = set_input_type(addr);
		if (status != asynSuccess)
			return (status);

		get_input_type(addr);
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

void KS3497xA::select_inputs(int card, int flags) {
    int i;
    int flag;
    
    // Populate the local channel selection info
    for (i = 0; i < 16; i++) {
        // Mask out each flag in turn
        flag = flags & (1 << i);
        card_input_active[card][i] = flag;
    }
}

asynStatus KS3497xA::select_monitor(int card, int channel) {
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];

    sprintf(command, "ROUT:MON (@%d%02d)", card, channel);

    status = write_command(command);
    return status;
}

asynStatus KS3497xA::start_stop_monitor(int value) {
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];

    sprintf(command, "ROUT:MON:STATE %s\n", (value==0?"OFF":"ON"));

    monitoring = value;

    status = write_command(command);
    return status;
}

asynStatus KS3497xA::set_input_type(int channel) {
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];
    int temperature_type;

	std::cout << "set_input_type: channel = " << channel << std::endl;

    getIntegerParam(KS3497xAInputTypeSelect, &temperature_type);

	std::cout << "set_input_type: temperature type = " << temperature_type << std::endl;

    switch(temperature_type) {
        case KS3497xA::input_type_tc :
            int tc_type;
            getIntegerParam(KS3497xATCTypeSelect, &tc_type);

			std::cout << "set_input_type: tc_type = " << tc_type << std::endl;

            sprintf(command, "CONF:TEMP TC,%s,(@%d)\n", 
                    KS3497xA::TC_TYPES[tc_type].c_str(),
                    channel);
			std::cout << "set_input_type: command = " << command << std::endl;
            status = write_command(command);
            break;

        case KS3497xA::input_type_rtd :
            int rtd_type;
            int rtd_resistance;
            getIntegerParam(KS3497xARTDTypeSelect, &rtd_type);
            getIntegerParam(KS3497xARTDRValueSelect, &rtd_resistance);

			std::cout << "set_input_type: rtd_type = " << rtd_type << std::endl;
			std::cout << "set_input_type: rtd_resistance = " << rtd_resistance << std::endl;

            // TODO: Add some sanity checking on values we've received

            sprintf(command, "CONF:TEMP RTD,%d,(@%d)\n",
                    KS3497xA::RTD_TYPES[rtd_type],
                    channel);
			std::cout << "set_input_type: command = " << command << std::endl;
            status = write_command(command);

            if (status != asynSuccess)
                break;

            std::fill(command, command+MAX_COMMAND_LENGTH, '\0');

            sprintf(command, "SENS:TEMP:TRAN:FRTD:RES %d,(@%d)\n", 
                    rtd_resistance,
                    channel);
            break;

        case KS3497xA::input_type_thermistor :
            int thermistor_type;
            getIntegerParam(KS3497xAThermistorTypeSelect, &thermistor_type);

            sprintf(command, "CONF:TEMP THER,%d,(@%d)\n",
                    KS3497xA::THERMISTOR_TYPES[thermistor_type],
                    channel);
            status = write_command(command);
            break;
    }

    return status;
}

asynStatus KS3497xA::get_input_type(int channel) {
    asynStatus status = asynSuccess;
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_COMMAND_LENGTH];
	std::string response_buf;
	std::string input_type;
	std::string temperature_input_type;
	std::string temperature_conf_string;
	std::string tc_type;
	std::string quote = "\"";
	std::string separator = " ";
	int index;

	std::vector<std::string>::iterator it;

	sprintf(command, "CONF? (@%d)\n", channel);
            status = writeread_command(command, response);

	response_buf = response;
	std::cout << "get_input_type:: response = " << response << std::endl;

	// Remove the leading "
    input_type = response_buf.substr(
			response_buf.find(quote) + 1, 
			response_buf.find(separator) - 1);

	// Get all data after the first space
	temperature_conf_string = response_buf.substr(response_buf.find(separator) + 1);

	std::cout << "get_input_type: input_type = " << input_type << std::endl;
	std::cout << "get_input_type: temperature_conf_string = " << temperature_conf_string << std::endl;

	std::cout << "get_input_type: len(input_type) = " << input_type.length() << std::endl;
	std::cout << "get_input_type: len(TEMP) = " << KS3497xA::INPUT_TYPE_STRINGS[1].length() << std::endl;

	it = std::find(
			KS3497xA::INPUT_TYPE_STRINGS.begin(), 
			KS3497xA::INPUT_TYPE_STRINGS.end(), 
			input_type);

	if (it != KS3497xA::INPUT_TYPE_STRINGS.end()) {
		index = std::distance(
				KS3497xA::INPUT_TYPE_STRINGS.begin(),
				it);

		std::cout << "get_input_type: index = " << index << std::endl;

		setIntegerParam(KS3497xAInputTypeRead, index);
		status = asynSuccess;
	}
	else {
		std::cout << "get_input_type: didn't find match for " << input_type << std::endl;
		status = asynError;
	}

	std::cout 
		<< "get_input_type: KS3497xA::INPUT_TYPE_STRINGS[0] = " 
		<< KS3497xA::INPUT_TYPE_STRINGS[0] 
		<< std::endl;

	std::cout 
		<< "get_input_type: KS3497xA::INPUT_TYPE_STRINGS[1] = " 
		<< KS3497xA::INPUT_TYPE_STRINGS[1] 
		<< std::endl;

	std::cout 
		<< "get_input_type: comparison = " 
		<< input_type.compare(KS3497xA::INPUT_TYPE_STRINGS[1]) 
		<< std::endl;
	
	if (input_type.compare(KS3497xA::INPUT_TYPE_STRINGS[1]) == 0) {
		std::string comma = ",";
		temperature_input_type = temperature_conf_string.substr(
				0, temperature_conf_string.find(comma));

		std::cout << "get_input_type: temperature_input_type = " << temperature_input_type << std::endl;

		if (temperature_input_type.compare(KS3497xA::TEMP_TYPE_STRINGS[0]) == 0) {
			tc_type = temperature_conf_string.substr(
					temperature_conf_string.find(comma) + 1).substr(
					0, temperature_conf_string.find(comma) - 1);

			std::cout << "get_input_type: tc_type = " << tc_type << std::endl;

			it = std::find(
					KS3497xA::TC_TYPES.begin(), 
					KS3497xA::TC_TYPES.end(), 
					tc_type);

			if (it != KS3497xA::TC_TYPES.end()) {
				index = std::distance(
						KS3497xA::TC_TYPES.begin(),
						it);

				std::cout << "get_input_type: index = " << index << std::endl;
				setIntegerParam(KS3497xATCTypeRead, index);
				status = asynSuccess;
			}
			else {
				std::cout << "get_input_type: didn't find match for " << tc_type << std::endl;
				status = asynError;
			}
		}
	}
	return status;
}

	asynStatus KS3497xA::read_monitor_data(void) {
		asynStatus status = asynSuccess;
		double value;
		char command[MAX_COMMAND_LENGTH];
		char response[MAX_COMMAND_LENGTH];
		const char *functionName = "read_monitor_data";

		sprintf(command, "ROUT:MON:DATA?\n");

		status = writeread_command(command, response);

		value = atof(response);

		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
				"%s:%s: response = %s, value = %f\n",
				driverName, functionName, response, value);

		setDoubleParam(KS3497xAMonVal, value);
		callParamCallbacks();

		return status;

	}

	asynStatus KS3497xA::write_command(const char *command) {
		asynStatus status = asynSuccess;
		double timeout = 1.0;
		size_t nWrite;
		const char *functionName = "write_command";

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
			return(asynError);
		}
		return status;
	}

	asynStatus KS3497xA::writeread_command(const char *command, char *response) {
		asynStatus status = asynSuccess;
		double timeout = 1.0;
		size_t nWrite;
		size_t response_length;
		int eomReason;
		const char *functionName = "writeread_command";

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

		if (status != asynSuccess) {
			asynPrint(
					this->pasynUserSelf,
					ASYN_TRACE_ERROR,
					"%s:%s comms status = %d, command = %s, response length = %lu, eomReason = %d\n", 
					driverName, 
					functionName,
					status,
					command,
					response_length,
					eomReason);
			this->comms_status = status;
			return(asynError);
		}

		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
				"%s:%s: command = %s\n",
				driverName, functionName, command);

		asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
				"%s:%s: response = %s\n",
				driverName, functionName, response);

		return status;
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
