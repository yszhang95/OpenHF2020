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
                #user_decay_file =  cms.vstring('GeneratorInterface/EvtGenInterface/data/LambdaB_JPsipK.dec'),













        list_forced_decays = cms.vstring(),
        ),
        parameterSets = cms.vstring('EvtGen130')
    ),
    PythiaParameters = cms.PSet(
        pythia8CommonSettingsBlock,
        pythia8CUEP8M1SettingsBlock,
        processParameters = cms.vstring(
                                            '443:onMode = off',
                                            '443:onIfAny = -13 13',
                                            'SoftQCD:nonDiffractive = on',
                                            'PTFilter:filter = on', # this turn on the filter
                                            'PTFilter:quarkToFilter = 5', # PDG id of q quark (can be any other)
                                            'PTFilter:scaleToFilter = 1.0'),
        parameterSets = cms.vstring(
            'pythia8CommonSettings',
            'pythia8CUEP8M1Settings',
            'processParameters',
        )
    )
)

generator.PythiaParameters.processParameters.extend(EvtGenExtraParticles)

# Filters

bfilter = cms.EDFilter(
        "PythiaFilter",
        MaxEta = cms.untracked.double(2.54),
        MinEta = cms.untracked.double(-3.46),
        ParticleID = cms.untracked.int32(511)
        )

ProductionFilterSequence = cms.Sequence(generator*bfilter)
