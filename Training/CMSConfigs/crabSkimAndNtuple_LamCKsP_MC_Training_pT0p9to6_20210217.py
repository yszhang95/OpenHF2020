import CRABClient
from WMCore.Configuration import Configuration
config = Configuration()

from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from httplib import HTTPException

config.section_("General")
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True

config.section_("JobType")
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pPb2016SkimAndNtuple_LamCKsP_MC_Training_pT0p9to6_20210217.py'
config.JobType.allowUndistributedCMSSW = True

config.section_("Data")
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.ignoreLocality = True

config.section_("Site")
config.Site.storageSite = 'T2_CH_CERN'
config.Site.whitelist = ['T1_US_*','T2_US_*','T1_FR_*','T2_FR_*','T2_CH_CERN']

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print "Failed submitting task: %s" % (hte.headers)
    except ClientException as cle:
        print "Failed submitting task: %s" % (cle)

dataMap = {
    "pPb" : {"PD" : '/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/pPb816Summer16DR-pPbEmb_80X_mcRun2_pA_v4-v1/AODSIM'},
    "Pbp" : {"PD" : '/LambdaC-KsPr_LCpT-0p9_PbP-EmbEPOS_8p16_Pythia8/pPb816Summer16DR-PbPEmb_80X_mcRun2_pA_v4-v1/AODSIM'}
    }

for key, val in dataMap.items():
    config.General.requestName = 'PA8TeV_'+key+'_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217'
    config.Data.inputDataset = val["PD"]
    config.Data.outputDatasetTag = config.General.requestName
    config.Data.outLFNDirBase = '/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/'
    print("Submitting CRAB job for: "+val["PD"])
    submit(config)
