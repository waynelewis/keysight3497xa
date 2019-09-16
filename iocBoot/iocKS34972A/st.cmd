#!../../bin/linux-x86_64/KS3497xAApp

## You may have to change KS3497xA to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/KS3497xAApp/Db")
epicsEnvSet("DEV_IP_ADDRESS", "192.168.1.41")
epicsEnvSet("DEV_ASYN_PORT", "KS3497XA")
epicsEnvSet("DEV_SUP_ASYN_PORT", "MUX1")

epicsEnvSet("PREFIX", "MMT:")
epicsEnvSet("DEVICE", "MUX1:")

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/KS3497xAApp.dbd"
KS3497xAApp_registerRecordDeviceDriver pdbbase

## Establish the connection to the 34972A
drvAsynIPPortConfigure("$(DEV_ASYN_PORT)",  "$(DEV_IP_ADDRESS):5025", 0, 0, 0)

## Initialize the driver support code
initKS3497xA("$(DEV_SUP_ASYN_PORT)", "$(DEV_ASYN_PORT)", 1000)

## Load record instances
dbLoadRecords("db/KS3497xA.db","P=$(PREFIX),R=$(DEVICE),PORT=$(DEV_SUP_ASYN_PORT)")
dbLoadRecords("db/KS3497xA_channels.db","P=$(PREFIX),R=$(DEVICE),PORT=$(DEV_SUP_ASYN_PORT)")
dbLoadRecords("db/asynRecord.db","P=$(PREFIX),R=$(DEVICE)Asyn,PORT=$(DEV_ASYN_PORT),OMAX=80,IMAX=80,ADDR=0")

#asynSetTraceMask("$(DEV_ASYN_PORT)", -1, 0xFF)
#asynSetTraceMask("$(DEV_SUP_ASYN_PORT)", -1, 0x11)
asynSetTraceMask("$(DEV_SUP_ASYN_PORT)", -1, 0x01)
#asynSetTraceIOMask("$(DEV_ASYN_PORT)", -1, 0xFF)

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=wlewis"
