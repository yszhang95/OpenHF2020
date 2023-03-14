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
config.JobType.scriptExe = 'run.sh'
config.JobType.scriptArgs = []
config.JobType.psetName = 'dummy.py'
config.JobType.inputFiles = ['FrameworkJobReport.xml', 'OpenHF2020.tar.gz', 'skimNTuple.C']
# do it later
# config.JobType.maxJobRuntimeMin = 120

config.Data.inputDBS = 'phys03'
config.Data.splitting = 'FileBased'
# do it later
# config.Data.unitsPerJob = 50
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/yousen/LamC_pPb2016'
config.JobType.outputFiles = ['output.root']
# do it later
# config.Data.userInputFiles = [ f.strip() for f in open("lists/dataMB1_pPb.list").readlines() ]

# config.Site.whitelist = ['T2_US_MIT']
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
import json
parser = argparse.ArgumentParser(description='Submit a single job to target site to test accessibility of files.')
# parser.add_argument('--maxRuntimeMin', dest='maxRuntimeMin', help='max runtime in minutes', type=int, default=60)
# parser.add_argument('--unitsPerJob', dest='unitsPerJob', help='# of input root files per job', type=int, default=50)
# parser.add_argument('--ptmin', dest='ptmin', help='Minimum of pT, as precise as 1E-1', type=float, default=2.9)
args = parser.parse_args()

dataMap["dataHM1_pPb"] = {}
with open("dataHM1_pPb_bad_list.txt") as f:
    dataMap["dataHM1_pPb"]["files"] = json.load(f)
    print(dataMap["dataHM1_pPb"]["files"])

## Submit the PA PDs
for key, val in dataMap.items():
    config.General.requestName = "{}_SkimNoMVA_v3".format(key)
    config.Data.outputDatasetTag = 'condor_SkimNoMVA_LamC_{}'.format(key)
    config.Data.userInputFiles = val["files"]

    # https://twiki.cern.ch/twiki/bin/view/CMSPublic/CRAB3AdvancedTutorial#Exercise_4_user_script
    # pT Min
    additional_args = []
    # additional_args.append("PtMin={:.1f}".format(args.ptmin))
    additional_args.append("PtMin={:.1f}".format(2.9))
    # boost
    boost = "pPb" if key.find("pPb") > 0 else "Pbp"
    boost = "Boost={}".format(boost)
    additional_args.append(boost)
    config.JobType.scriptArgs = additional_args

    print("Additional parameters for scriptExe are : {}".format(additional_args))

    # max runtime
    config.JobType.maxJobRuntimeMin = 120
    # units per job
    config.Data.unitsPerJob = len(val["files"])

    sitename = "T2_US_MIT"
    config.Site.whitelist = [ sitename ]
    if sitename == "T2_US_MIT":
        config.section_("Debug")
        config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
        print("Disabled OVERFLOW mechanism")

    print("Submitting CRAB job for: {}".format(key))
    submit(config)
