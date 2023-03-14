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
config.Data.publication = False
config.Data.outLFNDirBase = '/store/user/yousen/LamC_pPb2016'
config.JobType.outputFiles = ['bad_input_files.list', 'good_input_files.list']
# do it later
# config.Data.userInputFiles = [ f.strip() for f in open("lists/dataMB1_pPb.list").readlines() ]
# config.Data.outputDatasetTag = 'condor_dataMB_LamC'

# config.Site.whitelist = ['T2_BR_SPRACE']
config.Site.storageSite = 'T2_CH_CERN'

## Submit the PA PDs
key = "dataHM1_pPb"
val = {}
val["files"] = [ l.strip() for l in open("lists/dataHM1_pPb.list").readlines() ]
config.General.requestName = "{}_select_files_v2".format(key)
config.Data.outputDatasetTag = "condor_LambdaC_pPb_{}".format(key)
config.Data.userInputFiles = val["files"]
config.Data.unitsPerJob = len(val["files"])
sitename = "T2_BR_SPRACE" if key.find("HM") < 0 else "T2_US_MIT"
config.Site.whitelist = [ sitename ]

print("The target site for execution is {}".format(sitename))
print("Submitting CRAB job for: {}".format(key))
