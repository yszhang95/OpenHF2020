#!/usr/bin/env python
import ROOT
from DataFormats.FWLite import Events, Handle

events = Events("LambdaC_KsProton_pt0p9_y2p4_pPb.root")

handle = Handle('std::vector<reco::GenParticle>')

label = ("genParticles")

ROOT.gROOT.SetBatch()

for event in events:
  event.getByLabel(label, handle)
  genParticles = handle.product()
  print "Run:Lumi:Event:", event.eventAuxiliary().run(),\
    event.eventAuxiliary().luminosityBlock(), event.eventAuxiliary().event()
  print ""
  for p in genParticles:
    if abs(p.pdgId()) == 4122:
      print "LambdaC:", p.pdgId(), p.charge()
      dau0 = p.daughter(0)
      dau1 = p.daughter(1)
      if dau0.pdgId() != 310:
        print "[ERROR] Not Ks!"
      print "Its daughters:", p.daughter(0).pdgId(), p.daughter(1).pdgId()
      print "Its daughters' statuses", p.daughter(0).status(), p.daughter(1).status()
      if dau0.pdgId() == 310 :
        nGDau = p.daughter(0).numberOfDaughters()
        if nGDau > 0:
          for i in range(0, nGDau):
            print "Ks daughter", i, dau0.daughter(i).pdgId()
        else:
          print "[ERROR] Ks does not decay"
    if abs(p.pdgId()) == 310 and abs(p.mother(0).pdgId())!= 4122:
      print "Ks:", p.pdgId(), p.status()
      print "Ks is long-lived:", p.longLived()
      print "Ks mother:", p.mother(0).pdgId(), p.mother(0).status()
      nKsDau = p.numberOfDaughters()
      if nKsDau > 0:
        for i in range(0, nKsDau):
          print "Ks daughter", i, p.daughter(i).pdgId()
      else:
        print "[ERROR] Ks does not decay"
    if abs(p.pdgId()) == 311:
      print "K:", p.pdgId(), p.status()
      print "K is long-lived:", p.longLived()
      print "K mother:", p.mother(0).pdgId(), p.mother(0).status()
      nKDau = p.numberOfDaughters()
      if nKDau > 0:
        for i in range(0, nKDau):
          print "K daughter", i, p.daughter(i).pdgId()
      else:
        print "[ERROR] K does not decay"
    if abs(p.pdgId()) == 130 and abs(p.mother(0).pdgId())!= 4122:
      print "KL:", p.pdgId(), p.status()
      print "KL is long-lived:", p.longLived()
      print "KL mother:", p.mother(0).pdgId(), p.mother(0).status()
      nKLDau = p.numberOfDaughters()
      if nKLDau > 0:
        for i in range(0, nKLDau):
          print "KL daughter", i, p.daughter(i).pdgId()
      else:
        print "[ERROR] KL does not decay"
  print ""
