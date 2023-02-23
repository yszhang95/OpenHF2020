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

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
'/store/hidata/PARun2016C/PAHighMultiplicity1/AOD/PromptReco-v1/000/285/505/00000/006F1E14-85AF-E611-9F9E-02163E014508.root'
)
)

# =============== Other Statements =====================
# process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(2000))
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.GlobalTag.globaltag = '80X_dataRun2_v19'

# =============== Import Sequences =====================
#Trigger Selection
### Comment out for the timing being assuming running on secondary dataset with trigger bit selected already
# Add trigger selection
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltFilter.andOr = cms.bool(True)
process.hltFilter.throw = cms.bool(False)
process.hltFilter.HLTPaths = [
    'HLT_PAFullTracks_Multiplicity120_v*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity150_v*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity185_part*', # High multiplicity
    'HLT_PAFullTracks_Multiplicity250_v*', # High multiplicity
    'HLT_PAL1MinimumBiasHF_OR_SinglePixelTrack_part*', # Minimum bias
    ]

# Add PbPb collision event selection
process.load('VertexCompositeAnalysis.VertexCompositeProducer.collisionEventSelection_cff')
#process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping * process.olvFilter_pPb8TeV_dz1p0)
#remove the default dz1p0 filter
process.colEvtSel = cms.Sequence(process.hfCoincFilter * process.primaryVertexFilterPA * process.NoScraping)

# Define the event selection sequence
process.eventFilter_HM = cms.Sequence(
    process.hltFilter *
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
# No efficiency loss from MC studies if I use > 0.0001
# process.generalD0CandidatesNew.vtxChi2ProbCut = cms.double(0)

process.d0rereco_step = cms.Path( process.eventFilter_HM * process.generalD0CandidatesNew )

# produce D0 trees
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0selector_cff")
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.d0analyzer_tree_cff")
process.load("VertexCompositeAnalysis.VertexCompositeAnalyzer.eventinfotree_cff")

process.TFileService = cms.Service("TFileService",
    fileName =
    cms.string('d0ana_tree.root')
    )

# set up selectors
process.d0selectorNoCut = process.d0selector.clone()
process.d0selectorNoCut.VertexCompositeCollection = cms.untracked.InputTag("generalD0CandidatesNew:D0")
process.d0selectorNoCut.multMin = cms.untracked.double(0)
process.d0selectorNoCut.multMax = cms.untracked.double(100000)

process.d0selectorMid = process.d0selectorNoCut.clone()
process.d0selectorMid.candpTMin = cms.untracked.double(1.9)
process.d0selectorMid.candYMax = cms.untracked.double(1.2)
process.d0selectorMid.candYMin = cms.untracked.double(-1.2)

process.d0selectorMidAngle3D = process.d0selectorMid.clone()
process.d0selectorMidDlsig3D = process.d0selectorMid.clone()

process.d0selectorMidDlsig3D.cand3DDecayLengthSigMin = cms.untracked.double(2.0)
process.d0selectorMidAngle3D.cand3DPointingAngleMax = cms.untracked.double(0.25)

process.d0anaNoCut  = process.d0ana.clone()
process.d0anaNoCut.useAnyMVA = cms.bool(False)
process.d0anaNoCut.multMin = cms.untracked.double(0)
process.d0anaNoCut.multMax = cms.untracked.double(100000)

process.d0anaCutDLSig = process.d0anaNoCut.clone()
process.d0anaCutDLSig.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMidDlsig3D:D0")

process.d0anaCutAlpha = process.d0anaNoCut.clone()
process.d0anaCutAlpha.VertexCompositeCollection = cms.untracked.InputTag("d0selectorMidAngle3D:D0")

process.d0ana_seq_dlsig = cms.Sequence(process.eventFilter_HM * process.d0selectorMidDlsig3D* process.d0anaCutDLSig)
process.d0ana_seq_angle = cms.Sequence(process.eventFilter_HM * process.d0selectorMidAngle3D* process.d0anaCutAlpha)

# eventinfoana must be in EndPath, and process.eventinfoana.selectEvents must be the name of eventFilter_HM Path
process.eventinfoana.selectEvents = cms.untracked.string('eventFilter_HM_step')
process.eventinfoana.triggerPathNames = cms.untracked.vstring(
    'HLT_PAFullTracks_Multiplicity120_v', # High multiplicity
    'HLT_PAFullTracks_Multiplicity150_v', # High multiplicity
    'HLT_PAFullTracks_Multiplicity185_part', # High multiplicity
    'HLT_PAFullTracks_Multiplicity250_v', # High multiplicity
    'HLT_PAL1MinimumBiasHF_OR_SinglePixelTrack_part', # Minimum bias
    )
process.eventinfoana.triggerFilterNames = cms.untracked.vstring()
process.pevt = cms.EndPath(process.eventinfoana)

process.p_dlsig = cms.Path(process.d0ana_seq_dlsig)
process.p_angle = cms.Path(process.d0ana_seq_angle)

# Define the process schedule
process.schedule = cms.Schedule(
    process.eventFilter_HM_step,
    process.d0rereco_step,
    process.p_dlsig,
    process.p_angle,
    process.pevt,
)

# Add the event selection filters
process.Flag_colEvtSel = cms.Path(process.eventFilter_HM * process.colEvtSel)
process.Flag_hfCoincFilter = cms.Path(process.eventFilter_HM * process.hfCoincFilter)
process.Flag_primaryVertexFilterPA = cms.Path(process.eventFilter_HM * process.primaryVertexFilterPA)
process.Flag_NoScraping = cms.Path(process.eventFilter_HM * process.NoScraping)
process.Flag_pileupVertexFilterCut = cms.Path(process.eventFilter_HM * process.olvFilter_pPb8TeV_dz1p0)
process.Flag_pileupVertexFilterCutGplus = cms.Path(process.eventFilter_HM * process.pileUpFilter_pPb8TeV_Gplus)
# follow the exactly same config of process.eventinfoana.eventFilterNames
eventFilterPaths = [ process.Flag_colEvtSel , process.Flag_hfCoincFilter , process.Flag_primaryVertexFilterPA , process.Flag_NoScraping , process.Flag_pileupVertexFilterCut , process.Flag_pileupVertexFilterCutGplus ]
for P in eventFilterPaths:
    process.schedule.insert(0, P)
