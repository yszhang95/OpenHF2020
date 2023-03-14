"""
Used under CMSSW_8_0_35
"""
from __future__ import print_function
import ROOT
from DataFormats.FWLite import Events, Handle

def all_mom_ids(genPar):
  momids = []
  mom = genPar.mother(0)
  while mom:
      momids.append(mom.pdgId())
      mom = mom.mother(0)
  return momids

def all_dau_refs(genPar):
  nDau = genPar.numberOfDaughters()
  output = [genPar.daughter(idau) for idau in range(nDau)]
  for idau in range(nDau):
      output.extend(all_dau_refs(genPar.daughter(idau)))
  return output

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('python')
options.register ('outputRecord',
                  'record.txt',
                  VarParsing.multiplicity.singleton,
                  VarParsing.varType.string,
                  "Name of output text containing counting information")
options.parseArguments()

events = Events (options)
# create handle outside of loop
handle  = Handle ('std::vector<reco::GenParticle>')
# a label is just a tuple of strings that is initialized just
# like and edm::InputTag
label = ("genParticles")
ROOT.gROOT.SetBatch()        # don't pop up canvases

hLamC = ROOT.TH1D("hLamC", "", 10, -0.5, 9.5)
hKs = ROOT.TH1D("hKs", "", 10, -0.5, 9.5)

hLamC.GetXaxis().SetBinLabel(0+1, "All")
hLamC.GetXaxis().SetBinLabel(1+1, "Prompt")
hLamC.GetXaxis().SetBinLabel(2+1, "NonPrompt")

hKs.GetXaxis().SetBinLabel(0+1, "All K_{S}^{0}")
hKs.GetXaxis().SetBinLabel(1+1, "2 prong")
hKs.GetXaxis().SetBinLabel(2+1, "#pi^{+}#pi^{-}")
hKs.GetXaxis().SetBinLabel(3+1, "#pi^{0}#pi^{0}")

# loop over events
# print(events.size())
for event in events:
    # use getByLabel, just like in cmsRun
    event.getByLabel (label, handle)
    # get the product
    genParticles = handle.product()
    for genParticle in genParticles:
        if (abs(genParticle.pdgId()) == 310):
            hKs.Fill(0)
            if (genParticle.numberOfDaughters() == 2):
                hKs.Fill(1)
                if (abs(genParticle.daughter(0).pdgId()) == 211):
                    hKs.Fill(2)
                if (abs(genParticle.daughter(1).pdgId()) == 111):
                    hKs.Fill(3)
        if (abs(genParticle.pdgId()) == 4122):
            hLamC.Fill(0)
            mom_ids = all_mom_ids(genParticle)
            mom_id_inits = [i for i in map(lambda x: str(x)[0], mom_ids)]
            if '5' in mom_id_inits:
                hLamC.Fill(2)
            else:
                hLamC.Fill(1)

fout = ROOT.TFile("fout.root", "recreate")
fout.cd()
hKs.Write();
hLamC.Write();
