from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

import CRABClient
from CRABClient.UserUtilities import config, getUsername

config = config()

# do it later
# config.General.requestName = 'data_MB1_pPb'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.scriptExe = 'drop_files.sh'
config.JobType.psetName = 'dummy.py'
config.JobType.inputFiles = ['FrameworkJobReport.xml', 'dropEmptyFiles.C']
config.JobType.maxJobRuntimeMin = 120

config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
# config.Data.unitsPerJob = 100
# Must be commented out
# config.Data.totalUnits= 1
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/yousen/LamC_pPb2016'
config.JobType.outputFiles = ['bad_input_files.list', 'good_input_files.list']
# do it later
# config.Data.userInputFiles = [ f.strip() for f in open("lists/dataMB1_pPb.list").readlines() ]
# config.Data.outputDatasetTag = 'condor_dataMB_LamC'

# config.Site.whitelist = ['T2_BR_SPRACE']
config.Site.storageSite = 'T2_CH_CERN'

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print ("Failed submitting task: %s" % (hte.headers))
    except ClientException as cle:
        print ("Failed submitting task: %s" % (cle))

#############################################################################################
## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
#############################################################################################

dataMap = {}

import argparse
import os
parser = argparse.ArgumentParser(description='Submit a single job to target site to test accessibility of files.')
parser.add_argument('--inputFile', dest='inputFile', help='input file', type=str, default=None)
args = parser.parse_args()
if args.inputFile:
    basename = os.path.basename(args.inputFile)
    basename = basename.rsplit(".", 1)[ 0 ]
    dataMap[basename] = {}
    dataMap[basename]["files"] = [ l.strip() for l in open(args.inputFile).readlines() ]
else:
    for i in range(1, 7):
# pPb
        dataMap["dataHM{}_pPb".format(i)] = {
                "files" : [ l.strip() for l in open("lists/dataHM{}_pPb.list".format(i)).readlines() ]
                }
# Pbp
        dataMap["dataHM{}_Pbp".format(i)] = {
                "files" : [ l.strip() for l in open("lists/dataHM{}_Pbp.list".format(i)).readlines() ]
                }
## Submit the PA PDs
for key, val in dataMap.items():
    config.General.requestName = "{}_select_files_v5".format(key)
    config.Data.outputDatasetTag = "condor_LambdaC_{}".format(key)
    config.Data.userInputFiles = val["files"]
    config.Data.unitsPerJob = len(val["files"])
    sitename = "T2_BR_SPRACE" if key.find("HM") < 0 else "T2_US_MIT"
    print("The target site for execution is {}".format(sitename))
    config.Site.whitelist = [ sitename ]

    if sitename == "T2_US_MIT":
        config.section_("Debug")
        config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
        print("Disabled OVERFLOW mechanism")

    print("Submitting CRAB job for: {}".format(key))
    submit(config)
