#!../../bin/linux-x86_64/Keysight34972A

## You may have to change Keysight34972A to something else
## everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/Keysight34972A.dbd"
Keysight34972A_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=wlewis")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=wlewis"
