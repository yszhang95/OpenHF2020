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
config.JobType.psetName = 'pPb2016SkimAndNtuple_LamCKsP_Training_pT4to6_20210212.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.inputFiles = ['lumiData.csv', 'lumiDataHM.csv', 'lumiDataMB.csv']

config.section_("Data")
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 5
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
    "pPb" : {"PD": '/PAHighMultiplicity1/PARun2016C-PromptReco-v1/AOD',
      "lumiMask" : 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/HI/Cert_285479-285832_HI8TeV_PromptReco_pPb_Collisions16_JSON_NoL1T.txt',
      "runRange" : '285517-285539'
      },
    "Pbp" : {"PD": '/PAHighMultiplicity1/PARun2016C-PromptReco-v1/AOD',
      "lumiMask" : 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/HI/Cert_285952-286442_HI8TeV_PromptReco_Pbp_Collisions16_JSON_NoL1T.txt',
      "runRange" : '285956-286023'
      }
    }

for key, val in dataMap.items():
    config.General.requestName = 'PA8TeV_'+key+'_HighMult1_Run'+val["runRange"]+'_LamCKsP_pT4to6_y1p0_Training_20210212'
    config.Data.inputDataset = val["PD"]
    config.Data.lumiMask = val["lumiMask"]
    config.Data.runRange = val["runRange"]
    config.Data.outLFNDirBase = '/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/DATA'
    config.Data.outputDatasetTag = config.General.requestName
    print("Submitting CRAB job for: "+val["PD"])
    submit(config)
