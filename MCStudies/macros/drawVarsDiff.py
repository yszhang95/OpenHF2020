#!/usr/bin/env python
import ROOT as r

#inFile = r.TFile("hist.root")
#
#myvars = ("decayL3D", "decayL2D", "alpha3D", "alpha2D", "vtxProb", "decayL3DSig", "decayL2DSig")
#
#mytypes = ("gen", "signal", "unmatch")
#
##myhists = { "%s%s" % (myvar, mytype): inFile.Get("hlamc%s%s" % (myvar, mytype)) for mytype in mytypes for myvar in myvars  }
##del myhists['vtxProbgen']
#myhists = { "%s%s" % (myvar, mytype): inFile.Get("hlamc%s%s" % (myvar, mytype)) for mytype in mytypes if not mytype == 'gen' for myvar in myvars  }
#for myvar in myvars:
#  if (myvar != 'vtxProb') and (myvar != 'decayL3DSig') and (myvar != 'decayL2DSig'):
#    print myvar
#    myhists["%sgen" % myvar] = inFile.Get("hlamc%sgen" % myvar)

#for (k, v) in myhists.iteritemms(): # for python3, myhists.items()
  #print k
  #print v

#for ivar in myvars:
#  r.gStyle.SetOptStat(0)
#  c = r.TCanvas("c%s" % ivar, "", 550, 450)
#  c.SetLogy()
#  c.SetLeftMargin(0.15)
#  myhists['%ssignal' % ivar].Scale(1./myhists['%ssignal' % ivar].Integral(0, 100000))
#  myhists['%sunmatch' % ivar].Scale(1./myhists['%sunmatch' % ivar].Integral(0, 100000))
#  print myhists['%ssignal' % ivar].Integral(0, 10000)
#  print myhists['%ssignal' % ivar].GetMaximum()
#
#  myhists['%ssignal' % ivar].SetMarkerStyle(20)
#  myhists['%ssignal' % ivar].SetLineColor(r.kRed)
#  myhists['%ssignal' % ivar].GetYaxis().SetTitleOffset(1.3)
#  #myhists['%ssignal' % ivar].SetMaximum(0.3)
#
#  myhists['%sunmatch' % ivar].SetLineStyle(2)
#  myhists['%sunmatch' % ivar].SetLineColor(r.kBlue)
#
#  myhists['%ssignal' % ivar].Draw()
#  myhists['%sunmatch' % ivar].Draw("SAME")
#
#  if (ivar != 'vtxProb') and (ivar != 'decayL3DSig') and (ivar != 'decayL2DSig'):
#    print ivar
#    myhists['%sgen' % ivar].Scale(1./myhists['%sgen' % ivar].Integral(1, 100000));
#    #print myhists['%sgen' % ivar].Integral(1, 10000)
#    #print myhists['%sgen' % ivar].GetMaximum()
#    myhists['%sgen' % ivar].SetLineColor(r.kBlack)
#    myhists['%sgen' % ivar].Draw("SAME")
#
#  leg = r.TLegend(0.65, 0.75, 0.89, 0.89)
#  leg.AddEntry(myhists['%ssignal'%ivar], "signal", "p")
#  leg.AddEntry(myhists['%sunmatch'%ivar], "unmatch", "lp")
#  if (ivar != 'vtxProb') and (ivar != 'decayL3DSig') and (ivar != 'decayL2DSig'):
#    leg.AddEntry(myhists['%sgen' % ivar], "gen", "lp")
#  leg.Draw()
#  c.SaveAs('plots/%s.png' % c.GetName())

def drawVarsDiff(particle, var, hasGEN, jpt, myhists):
  r.gStyle.SetOptStat(0)
  c = r.TCanvas("c%s%s_pt%d" % (particle, var, jpt), "", 550, 450)
  c.SetLogy()
  c.SetLeftMargin(0.15)
  myhists['%ssignal' % var].Scale(1./myhists['%ssignal' % var].Integral(1, 100000))
  myhists['%sunmatch' % var].Scale(1./myhists['%sunmatch' % var].Integral(1, 100000))

  myhists['%ssignal' % var].SetMarkerStyle(20)
  myhists['%ssignal' % var].SetLineColor(r.kRed)
  myhists['%ssignal' % var].GetYaxis().SetTitleOffset(1.3)

  myhists['%sunmatch' % var].SetLineStyle(2)
  myhists['%sunmatch' % var].SetLineColor(r.kBlue)

  myhists['%ssignal' % var].SetMinimum(1e-5)
  myhists['%ssignal' % var].Draw("P")
  myhists['%sunmatch' % var].Draw("SAME")

  if hasGEN:
    myhists['%sgen' % var].Scale(1./myhists['%sgen' % var].Integral(1, 100000));
    myhists['%sgen' % var].SetLineColor(r.kBlack)
    myhists['%sgen' % var].Draw("SAME")

  leg = r.TLegend(0.65, 0.75, 0.89, 0.89)
  leg.AddEntry(myhists['%ssignal'%var], "signal", "lp")
  leg.AddEntry(myhists['%sunmatch'%var], "unmatch", "lp")
  if hasGEN:
    leg.AddEntry(myhists['%sgen' % var], "gen", "lp")
  leg.Draw()
  c.SaveAs('plots/%s.png' % c.GetName())
  return

def __main__():
  return 0


#inFile = r.TFile("hist20200720.root")
inFile = r.TFile("hist20200721.root")
#inFile = r.TFile("hist.root")

myvars = ("decayL3D", "decayL2D", "alpha3D", "alpha2D", "vtxProb", "decayL3DSig", "decayL2DSig", 'dcaToPV', 'pseudoDL', 'y')

mytypes = ("gen", "signal", "unmatch")

for ipt in range(9):
  lamchists = { "%s%s" % (myvar, mytype): inFile.Get("hlamc%s%s_pt%d" % (myvar, mytype, ipt)) for mytype in mytypes if not mytype == 'gen' for myvar in myvars  }
  kshists = { "%s%s" % (myvar, mytype): inFile.Get("hks%s%s_pt%d" % (myvar, mytype, ipt)) for mytype in mytypes if not mytype == 'gen' for myvar in myvars  }

  for myvar in myvars:
    if (myvar != 'vtxProb') and (myvar != 'decayL3DSig') and (myvar != 'decayL2DSig'):
      lamchists["%sgen" % myvar] = inFile.Get("hlamc%sgen_pt%d" % (myvar, ipt))
      #print lamchists["%sgen" % myvar]
      kshists["%sgen" % myvar] = inFile.Get("hks%sgen_pt%d" % (myvar, ipt))
      #print kshists["%sgen" % myvar]

  #for (k, v) in lamchists.iteritems(): # for python3, myhists.items()
    #print k
    #print v
  for var in myvars:
    hasGEN = False
    #if (var != 'vtxProb') and (var != 'decayL3DSig') and (var != 'decayL2DSig'):
    if (var == 'decayL3D') or (var == 'decayL2D') or (var == 'alpha3D') or (var == 'alpha2D'):
      hasGEN = True
    drawVarsDiff("lamc", var, hasGEN, ipt, lamchists)
    if (var != 'dcaToPV') and (var != 'pseudoDL') and (var != 'y'):
      drawVarsDiff("ks", var, hasGEN, ipt, kshists)

  kshists["%ssignal" % 'dca'] = inFile.Get("hks%ssignal_pt%d" % ('dca', ipt))
  kshists["%sunmatch" % 'dca'] = inFile.Get("hks%sunmatch_pt%d" % ('dca', ipt))
  #drawVarsDiff("ks", 'dca', False, ipt, kshists)

  #lamchists['dcaToPVsignal'] = inFile.Get("hlamcdcaToPVsignal_pt%d" % ipt)
  #lamchists['pseudoDLsignal'] = inFile.Get("hlamcpseudoDLsignal_pt%d" % ipt)
  #lamchists['dcaToPVunmatch'] = inFile.Get("hlamcdcaToPVunmatch_pt%d" % ipt)
  #lamchists['pseudoDLunmatch'] = inFile.Get("hlamcpseudoDLunmatch_pt%d" % ipt)

  #print lamchists['dcaToPVsignal']
  #print lamchists['pseudoDLsignal']
  #print lamchists['dcaToPVunmatch']
  #print lamchists['pseudoDLunmatch']

  #drawVarsDiff('lamc', 'dcaToPV', False, ipt, lamchists)
  #drawVarsDiff('lamc', 'pseudoDL', False, ipt, lamchists)

trackhists = {}
trackhists['zDCASig_pi_in_ks_lamc_signal'] = inFile.Get('hdz_pi_in_ks_lamc_signal')
trackhists['zDCASig_pi_in_ks_lamc_unmatch'] = inFile.Get('hdz_pi_in_ks_lamc_unmatch')
trackhists['xyDCASig_pi_in_ks_lamc_signal'] = inFile.Get('hdxy_pi_in_ks_lamc_signal')
trackhists['xyDCASig_pi_in_ks_lamc_unmatch'] = inFile.Get('hdxy_pi_in_ks_lamc_unmatch')

#for (k, v) in trackhists.iteritems(): # for python3, myhists.items()
#  print k
#  print v

drawVarsDiff('track','zDCASig_pi_in_ks_lamc_', False, 0, trackhists)
drawVarsDiff('track','xyDCASig_pi_in_ks_lamc_', False, 0, trackhists)
