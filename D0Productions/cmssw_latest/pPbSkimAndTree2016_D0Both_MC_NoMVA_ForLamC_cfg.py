import FWCore.ParameterSet.Config as cms
process = cms.Process("ANASKIM")

process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.MessageLogger.cerr.FwkReport.reportEvery = 200

# The line below always has to be included to make VarParsing work
from FWCore.ParameterSet.VarParsing import VarParsing

# In teh line below 'analysis' is an instance of VarParsing object
options = VarParsing ('analysis')

if not options.inputFiles:
    options.inputFiles = [ '/store/himc/pPb816Summer16DR/SoftQCD_D0ToKPi_PtGT1p2-pPb-Embed_8p16TeV_TuneCUETP8M1_Pythia8_evtgen/AODSIM/80X_mcRun2_pA_v4-v1/2820000/0073076D-F489-ED11-8B66-B02628DEBEB0.root' ]

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles)
)

# =============== Other Statements =====================
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(2000))
# process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))
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

# Add PbPb collision event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
#process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping * process.olvFilter_pPb8TeV_dz1p0)
#remove the default dz1p0 filter
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping)

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.colEvtSel
)
process.eventFilter_HM_step = cms.Path( process.eventFilter_HM )

########## D0 candidate rereco ###############################################################
process.load("VertexCompositeAnalysis.VertexCompositeProducer.generalD0Candidates_cff")
process.generalD0CandidatesNew = process.generalD0Candidates.clone()
# process.generalD0CandidatesNew.trkPtSumCut = cms.double(1.6)
process.generalD0CandidatesNew.trkPtSumCut = cms.double(0)
# process.generalD0CandidatesNew.trkEtaDiffCut = cms.double(1.0)
process.generalD0CandidatesNew.trkEtaDiffCut = cms.double(100000.0)
process.generalD0CandidatesNew.tkNhitsCut = cms.int32(11)
process.generalD0CandidatesNew.tkPtErrCut = cms.double(0.1)
process.generalD0CandidatesNew.tkPtCut = cms.double(0.7)
# process.generalD0CandidatesNew.alphaCut = cms.double(1.0)
# process.generalD0CandidatesNew.alpha2DCut = cms.double(1.0)
process.generalD0CandidatesNew.dPtCut = cms.double(1.9)
# new
process.generalD0CandidatesNew.mPiKCutMin = cms.double(1.69)
process.generalD0CandidatesNew.vtxChi2ProbCut = cms.double(0)

process.d0rereco_step = cms.Path( process.eventFilter_HM * process.generalD0CandidatesNew )

# produce D0 trees
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0selector_cff")
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0analyzer_ntp_cff")

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('d0ana_mc_tree.root')
    )

# set up selectorMCs
process.d0selectorMCNoCut = process.d0selectorMC.clone()
process.d0selectorMCNoCut.VertexCompositeCollection = cms.untracked.InputTag("generalD0CandidatesNew:D0")
process.d0selectorMCNoCut.multMin = cms.untracked.double(0)
process.d0selectorMCNoCut.multMax = cms.untracked.double(100000)
process.d0selectorMCNoCut.selectGenMatch = cms.untracked.bool(True)
process.d0selectorMCNoCut.DCAValCollection = cms.InputTag("generalD0CandidatesNew:DCAValuesD0")
process.d0selectorMCNoCut.DCAErrCollection = cms.InputTag("generalD0CandidatesNew:DCAErrorsD0")

process.d0anaMCNoCut  = process.d0ana_mc.clone()
process.d0anaMCNoCut.useAnyMVA = cms.bool(False)
process.d0anaMCNoCut.multMin = cms.untracked.double(0)
process.d0anaMCNoCut.multMax = cms.untracked.double(100000)
process.d0anaMCNoCut.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMCNoCut:D0")
process.d0anaMCNoCut.DCAValCollection = cms.InputTag("d0selectorMCNoCut:DCAValuesNewD0")
process.d0anaMCNoCut.DCAErrCollection = cms.InputTag("d0selectorMCNoCut:DCAErrorsNewD0")

process.d0ana_seq = cms.Sequence(process.eventFilter_HM * process.d0selectorMCNoCut * process.d0anaMCNoCut)


process.p = cms.Path(process.d0ana_seq)

# Add the Conversion tree

# Define the process schedule
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.d0rereco_step,
    process.p,
)
