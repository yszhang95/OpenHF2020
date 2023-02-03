import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras
process = cms.Process("ANASKIM", eras.Run2_2016_pA)

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.MessageLogger.cerr.FwkReport.reportEvery = 200

process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring(
'root://cmsxrootd.fnal.gov///store/user/anstahll/RiceHIN/MC/Embedded/Embedded_PromptD0_KK_8160GeV_pythia8_RECO_20190906/D0ToKK/Embedded_PromptD0_KK_8160GeV_pythia8_RECO_20190906/190906_020043/0000/step3_Embedded_PromptD0_KK_8160GeV_pythia8_RECO_20190906_8.root',
)
)

# =============== Other Statements =====================
#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.GlobalTag.globaltag = '80X_mcRun2_pA_v4'

# =============== Import Sequences =====================
#Trigger Selection
### Comment out for the timing being assuming running on secondary dataset with trigger bit selected already
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltHM = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
#process.hltHM.HLTPaths = ['HLT_PAFullTracks_Multiplicity280_v*']
process.hltHM.HLTPaths = ['HLT_PAFullTracks_Multiplicity280_v*']
process.hltHM.andOr = cms.bool(True)
process.hltHM.throw = cms.bool(False)

process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping)

process.eventFilter_HM = cms.Sequence(
#    process.hltHM *
    process.colEvtSel
)

process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

#process.dEdx_step = cms.Path( process.eventFilter_HM * process.produceEnergyLoss )

########## D0 candidate rereco ###############################################################
process.load("VertexCompositeAnalysis.VertexCompositeProducer.generalD0Candidates_cff")
process.generalD0CandidatesNew = process.generalD0Candidates.clone()
process.generalD0CandidatesNewWrongSign = process.generalD0Candidates.clone(isWrongSign = cms.bool(True))

process.d0rereco_step = cms.Path( process.eventFilter_HM * process.generalD0CandidatesNew )
process.d0rereco_wrongsign_step = cms.Path( process.eventFilter_HM * process.generalD0CandidatesNewWrongSign )

###############################################################################################

process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0selector_cff")
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0analyzer_tree_cff")
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0analyzer_ntp_cff")

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('d0ana_mc_tree.root')
)

process.d0ana_mc.useAnyMVA = cms.bool(True)
process.d0ana_mc.PID_dau1 = cms.untracked.int32(321)
process.d0ana_mc.PID_dau2 = cms.untracked.int32(321)
process.d0ana_mc.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMC:D0")
process.d0ana_mc.MVACollection = cms.InputTag("d0selectorMC:MVAValuesNewD0")
process.d0ana_mc_wrongsign = process.d0ana_mc.clone()
process.d0ana_mc_wrongsign.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMCWS:D0")
process.d0ana_mc_wrongsign.MVACollection = cms.InputTag("d0selectorMCWS:MVAValuesNewD0")

process.d0selectorMCBDTPreCut.GBRForestFileName = cms.string('GBRForestfile_BDT_PromptD0InpPb_default_HLT185_WS_Pt1p5MassPeak_v2.root')
process.d0selectorMC = process.d0selectorMCBDTPreCut.clone()
process.d0selectorMCWS = process.d0selectorMC.clone(
  VertexCompositeCollection = cms.untracked.InputTag("generalD0CandidatesNewWrongSign:D0"),
  MVACollection = cms.InputTag("generalD0CandidatesNewWrongSign:MVAValues")
)

process.d0selectorMCNewReduced = process.d0selectorMC.clone()
process.d0selectorMCNewReducedWS = process.d0selectorMCWS.clone()
process.d0selectorMCNewReduced.GBRForestFileName = cms.string('GBRForestfile_BDT_PromptD0InpPb_default_HLT185_WS_Pt1p5MassPeak_NoPtErrNHitDLAngle2D_v3.root')
process.d0selectorMCNewReducedWS.GBRForestFileName = cms.string('GBRForestfile_BDT_PromptD0InpPb_default_HLT185_WS_Pt1p5MassPeak_NoPtErrNHitDLAngle2D_v3.root')

process.d0ana_mc_newreduced = process.d0ana_mc.clone()
process.d0ana_mc_newreduced.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMCNewReduced:D0")
process.d0ana_mc_newreduced.MVACollection = cms.InputTag("d0selectorMCNewReduced:MVAValuesNewD0")

process.d0ana_mc_newreduced_wrongsign = process.d0ana_mc_wrongsign.clone()
process.d0ana_mc_newreduced_wrongsign.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMCNewReduced:D0")
process.d0ana_mc_newreduced_wrongsign.MVACollection = cms.InputTag("d0selectorMCNewReduced:MVAValuesNewD0")

process.d0ana_seq2 = cms.Sequence(process.d0selectorMCNewReduced * process.d0ana_mc_newreduced)

process.d0ana_wrongsign_seq2 = cms.Sequence(process.d0selectorMCNewReducedWS * process.d0ana_mc_newreduced_wrongsign)

process.p1 = cms.Path(process.eventFilter_HM * process.d0ana_seq2)
process.p2 = cms.Path(process.eventFilter_HM * process.d0ana_wrongsign_seq2)

process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.d0rereco_step,
    process.d0rereco_wrongsign_step,
    process.p1,
    process.p2,
)
