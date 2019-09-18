#ifndef DRVKEYSIGHT3497XA_H
#define DRVKEYSIGHT3497XA_H

#include <vector>
#include <iocsh.h>
#include <epicsExport.h>
#include <asynPortDriver.h>

#define KS3497xASerialNumberString          "SerialNumber"
#define KS3497xAManufacturerString          "Manufacturer"
#define KS3497xAModelString                 "Model"
#define KS3497xAIDNString                   "IDN"
#define KS3497xATriggerSourceString         "TriggerSource"
#define KS3497xAScanIntervalString          "ScanInterval"
#define KS3497xAScanCountString             "ScanCount"
#define KS3497xAScanContinuousString          "ScanContinuous"
#define KS3497xAScanStartString             "ScanStart"
#define KS3497xAScanAbortString             "ScanAbort"
#define KS3497xACard1TypeString             "Card1Type"
#define KS3497xACard2TypeString             "Card2Type"
#define KS3497xACard3TypeString             "Card3Type"
#define KS3497xACardInput01_16SelectString  "CardInput01_16Select"
#define KS3497xACardMonSelectString         "CardMonSelect"
#define KS3497xAMonOnOffString              "MonOnOff"
#define KS3497xAMonValString                "MonVal"
#define KS3497xANumDataPointsString         "NumDataPoints"
#define KS3497xAInputTypeSelectString       "InputTypeSelect"
#define KS3497xATCTypeSelectString          "TCTypeSelect"
#define KS3497xARTDTypeSelectString         "RTDTypeSelect"
#define KS3497xARTDRValueSelectString       "RTDRValue"
#define KS3497xAThermistorTypeSelectString  "ThermistorTypeSelect"
#define KS3497xAInputTypeReadString         "InputTypeRead"
#define KS3497xATempTypeReadString          "TempTypeRead"
#define KS3497xATCTypeReadString            "TCTypeRead"
#define KS3497xAScanSelectString            "ScanSelect"
#define KS3497xALastErrorMessageString      "LastErrorMessage"
#define KS3497xALastErrorCodeString         "LastErrorCode"
#define KS3497xAErrorMessageString          "ErrorMessage"
#define KS3497xAErrorCodeString             "ErrorCode"

#define MAX_RESPONSE_LENGTH             255
#define MAX_COMMAND_LENGTH              255
#define MAX_CARDS                       3
#define MAX_INPUTS                      40

#define NO_ERROR_CODE                   0

#define SCAN_START                      1
#define SCAN_ABORT                      1

class KS3497xA : public asynPortDriver {

public:
    KS3497xA(const char *portName, const char *devicePortName, int pollTime);
    
    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    //virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
    virtual void pollerThread(void);

protected:
    int KS3497xASerialNumber;
    #define FIRST_KS3497XA_PARAM KS3497xASerialNumber
    int KS3497xAManufacturer;
    int KS3497xAModel;
    int KS3497xAIDN;
    int KS3497xATriggerSource;
    int KS3497xAScanInterval;
    int KS3497xAScanCount;
    int KS3497xAScanContinuous;
    int KS3497xAScanStart;
    int KS3497xAScanAbort;
    int KS3497xACard1Type;
    int KS3497xACard2Type;
    int KS3497xACard3Type;
    int KS3497xACardInput01_16Select;
    int KS3497xACardMonSelect;
    int KS3497xAMonOnOff;
    int KS3497xAMonVal;
    int KS3497xANumDataPoints;
    int KS3497xAInputTypeSelect;
    int KS3497xATCTypeSelect;
    int KS3497xARTDTypeSelect;
    int KS3497xARTDRValueSelect;
    int KS3497xAThermistorTypeSelect;
    int KS3497xAInputTypeRead;
    int KS3497xATempTypeRead;
    int KS3497xATCTypeRead;
    int KS3497xAScanSelect;
    int KS3497xALastErrorMessage;
    int KS3497xALastErrorCode;
    int KS3497xAErrorMessage;
    int KS3497xAErrorCode;
    #define LAST_KS3497XA_PARAM KS3497xAErrorCode

private:
    double pollTime_;
    bool read_metadata_request;
    asynUser *pasynUserKS;
    asynStatus comms_status;

    bool card_input_active[MAX_CARDS][MAX_INPUTS];
    bool monitoring;
    bool scanning;
    bool scan_complete;

    asynStatus check_status(void);
    asynStatus read_metadata(void);
    asynStatus read_data(void);
    asynStatus read_scan_status(void);
    asynStatus read_scan_data(void);
    asynStatus read_monitor_data(void);
    asynStatus set_scan_interval(void);
    asynStatus set_scan_count(void);
    asynStatus set_trigger_source(epicsInt32 source);
    asynStatus scan_start(void);
    asynStatus scan_abort(void);
    void select_inputs(int card, int flags);
    asynStatus set_input_type(asynUser *pasynUser, int addr);
    asynStatus get_input_type(asynUser *pasynUser, int addr);
    asynStatus update_scan_list(void);
    asynStatus select_monitor(int card, int channel);
    asynStatus start_stop_monitor(int value);
    asynStatus write_command(const char *command);
    asynStatus writeread_command(const char *command, char *response);

    enum input_types {input_type_tc, input_type_rtd, input_type_thermistor};
    std::vector<std::string> INPUT_TYPE_STRINGS = {"VOLT", "TEMP"};
    std::vector<std::string> TEMP_TYPE_STRINGS = {"TC", "RTD", "THER"};
    std::vector<std::string> TC_TYPES = {
        "B", "E", "J", "K", "N", "R", "S", "T"};
    std::vector<std::string> TRIGGER_TYPES = {
        "BUS", "IMM", "EXT", "ALAR1", "ALAR1", "ALAR1", "ALAR1", "TIM"};

    const int RTD_TYPES[2] = {85, 91};
    const int THERMISTOR_TYPES[3] = {2200, 5000, 10000};



};

#endif
