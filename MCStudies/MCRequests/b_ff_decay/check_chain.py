#!/usr/env/bin python
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

# loop over events
# print(events.size())
iev=0
nlb=0
nlb2lc=0
nlc=0
nb=0
nbu=0
nbu2lc=0
nbd=0
nbd2lc=0
sameEvent = 0
for event in events:
    #if iev > 1000:
    #    break
    iev += 1
    # use getByLabel, just like in cmsRun
    event.getByLabel (label, handle)
    # get the product
    genParticles = handle.product()
    isSameEvent = 0
    for genParticle in genParticles:
        pdgid = abs(genParticle.pdgId())
        if pdgid == 5122:
            nlb += 1
            # ndaus = genParticle.numberOfDaughters()
            # print([genParticle.daughter(i).pdgId()  for i in range(ndaus)])
            # print(all_dau_refs(genParticle))
            dau_refs = all_dau_refs(genParticle)
            dau_ids = [abs(d.pdgId()) for d in dau_refs]
            if 4122 in dau_ids:
              nlb2lc += 1
        if pdgid == 4122:
            nlc += 1
        if pdgid == 521:
            nbu += 1
            isSameEvent += 1
            dau_refs = all_dau_refs(genParticle)
            dau_ids = [abs(d.pdgId()) for d in dau_refs]
            if 4122 in dau_ids:
              nbu2lc += 1
        if pdgid == 511:
            nbd += 1
            isSameEvent += 1
            dau_refs = all_dau_refs(genParticle)
            dau_ids = [abs(d.pdgId()) for d in dau_refs]
            if 4122 in dau_ids:
              nbd2lc += 1
        #if pdgid == 4122:
            #print(all_mom_ids(genParticle))
    if isSameEvent==2:
        sameEvent += 1
    # print(genParticles.size())

#print("Number of b", nb)
print("Number of Lambda_b0", nlb)
print("Number of B+", nbu)
print("Number of B0", nbd)
print("Number of Lambda_b0 to Lambda_c+", nlb2lc)
print("Number of B+ to Lambda_c+", nbu2lc)
print("Number of B0 to Lambda_c+", nbd2lc)
print("Number of B+B0", sameEvent)
print("Number of Lambda_c+", nlc)
print("Total events:", iev)

with open(options.outputRecord, 'w') as fout:
  fout.write(
"""#Lb0,#Bu,#Bd,#Lb02Lc+,#Bu2Lc+,#Bd2Lc+,#Lc+,#events
{nlb},{nbu},{nbd},{nlb2lc},{nbu2lc},{nbd2lc},{nlc},{nevt}"""
    .format(nlb=nlb, nbu=nbu, nbd=nbd, nlb2lc=nlb2lc, nbu2lc=nbu2lc, nbd2lc=nbd2lc, nlc=nlc, nevt=iev))
  fout.close()
