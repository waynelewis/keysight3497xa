#ifndef DRVKEYSIGHT3497XA_H
#define DRVKEYSIGHT3497XA_H

#include <iocsh.h>
#include <epicsExport.h>
#include <asynPortDriver.h>

#define KS3497xASerialNumberString          "SerialNumber"
#define KS3497xAManufacturerString          "Manufacturer"
#define KS3497xAModelString                 "Model"
#define KS3497xAIDNString                   "IDN"
#define KS3497xATriggerSourceString         "TriggerSource"
#define KS3497xACard1TypeString             "Card1Type"
#define KS3497xACard2TypeString             "Card2Type"
#define KS3497xACard3TypeString             "Card3Type"
#define KS3497xACardInput01_16SelectString  "CardInput01_16Select"
#define KS3497xACardMonSelectString         "CardMonSelect"
#define KS3497xACardMonOnOffString          "CardMonOnOff"
#define KS3497xAInput101ValueString         "Input101Value"
#define KS3497xANumDataPointsString         "NumDataPoints"

#define MAX_RESPONSE_LENGTH             255
#define MAX_COMMAND_LENGTH              255
#define MAX_CARDS                       3
#define MAX_INPUTS                      40

class KS3497xA : public asynPortDriver {

public:
    KS3497xA(const char *portName, const char *devicePortName, int pollTime);
    
    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    //virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    //virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    //virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
    virtual void pollerThread(void);

protected:
    int KS3497xASerialNumber;
    #define FIRST_KS3497XA_PARAM KS3497xASerialNumber
    int KS3497xAManufacturer;
    int KS3497xAModel;
    int KS3497xAIDN;
    int KS3497xATriggerSource;
    int KS3497xACard1Type;
    int KS3497xACard2Type;
    int KS3497xACard3Type;
    int KS3497xACardInput01_16Select;
    int KS3497xACardMonSelect;
    int KS3497xACardMonOnOff;
    int KS3497xAInput101Value;
    int KS3497xANumDataPoints;
    #define LAST_KS3497XA_PARAM KS3497xANumDataPoints

private:
    double pollTime_;
    bool read_metadata_request;
    asynUser *pasynUserKS;

    bool card_input_active[MAX_CARDS][MAX_INPUTS];

    void read_metadata(void);
    void read_data(void);
    void select_inputs(int card, int offset, int flags);
    void select_monitor(int card, int channel);
    void start_stop_monitor(int value);
    void send_command(const char *command);

};

#endif
