#!../../bin/linux-x86_64/KS3497xAApp

## You may have to change KS3497xA to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet("STREAM_PROTOCOL_PATH", "$(TOP)/KS3497xAApp/Db")
epicsEnvSet("DEV_IP_ADDRESS", "192.168.1.41")
epicsEnvSet("DEV_ASYN_PORT", "KS3497XA")
epicsEnvSet("DEV_SUP_ASYN_PORT", "MUX1")

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/KS3497xAApp.dbd"
KS3497xAApp_registerRecordDeviceDriver pdbbase

## Establish the connection to the 34972A
drvAsynIPPortConfigure("$(DEV_ASYN_PORT)",  "$(DEV_IP_ADDRESS):5025", 0, 0, 0)

## Initialize the driver support code
initKS3497xA("$(DEV_SUP_ASYN_PORT)", "$(DEV_ASYN_PORT)", 1000)

## Load record instances
dbLoadRecords("db/KS3497xA.db","P=MMT:,R=MUX1:,PORT=$(DEV_SUP_ASYN_PORT)")

#asynSetTraceMask("$(DEV_ASYN_PORT)", -1, 0xFF)
#asynSetTraceIOMask("$(DEV_ASYN_PORT)", -1, 0xFF)

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=wlewis"
