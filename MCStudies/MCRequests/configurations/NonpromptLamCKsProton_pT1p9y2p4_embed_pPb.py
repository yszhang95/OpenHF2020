import FWCore.ParameterSet.Config as cms
from Configuration.Generator.Pythia8CommonSettings_cfi import *
from Configuration.Generator.Pythia8CUEP8M1Settings_cfi import *
from GeneratorInterface.EvtGenInterface.EvtGenSetting_cff import *

generator = cms.EDFilter("Pythia8GeneratorFilter",
    pythiaPylistVerbosity = cms.untracked.int32(0),
    pythiaHepMCVerbosity = cms.untracked.bool(False),
    comEnergy = cms.double(8160.0),
    maxEventsToPrint = cms.untracked.int32(0),
    ExternalDecays = cms.PSet(
        EvtGen130 = cms.untracked.PSet(
            decay_table = cms.string('GeneratorInterface/EvtGenInterface/data/DECAY_2014_NOLONGLIFE.DEC'),
            operates_on_particles = cms.vint32(),
            particle_property_file = cms.FileInPath('GeneratorInterface/EvtGenInterface/data/evt_2014.pdl'),
            convertPythiaCodes = cms.untracked.bool(False),
            user_decay_embedded= cms.vstring(
"""
Alias myLambdaC      Lambda_c+
Alias myanti-LambdaC anti-Lambda_c-
ChargeConj myanti-LambdaC myLambdaC
Decay myLambdaC
1.000   K_S0 p+  PHSP;
Enddecay
CDecay myanti-LambdaC
End
"""
            ),
            list_forced_decays = cms.vstring('myLambdaC', 'myanti-LambdaC')
        ),
        parameterSets = cms.vstring('EvtGen130')
    ),
    PythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CUEP8M1SettingsBlock,
        processParameters = cms.vstring(
            'HardQCD:all = on',
            'PhaseSpace:pTHatMin = 5.0'
        ),
        parameterSets = cms.vstring(
            'pythia8CommonSettings',
            'pythia8CUEP8M1Settings',
            'processParameters',
        )
    )
)

generator.PythiaParameters.processParameters.extend(EvtGenExtraParticles)

partonfilter = cms.EDFilter("PythiaFilter",
        ParticleID = cms.untracked.int32(5) # 4 for c and 5 for b quark
          )

LambdaCDaufilter = cms.EDFilter("PythiaMomDauFilter",
    ParticleID = cms.untracked.int32(4122),
    MomMinPt = cms.untracked.double(1.9),
    MomMinEta = cms.untracked.double(-2.5),
    MomMaxEta = cms.untracked.double(2.5),
    DaughterIDs = cms.untracked.vint32(310, 2212),
    NumberDaughters = cms.untracked.int32(2),
    NumberDescendants = cms.untracked.int32(0),
    BetaBoost = cms.untracked.double(0.434),
)

ProductionFilterSequence = cms.Sequence(generator*partonfilter*LambdaCDaufilter)
