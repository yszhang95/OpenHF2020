#!/usr/bin/env python
import ROOT as r

def projectTo1D(h2D, xory, start, end):
  if h2D == None:
    return None
  bounds = {}
  h1D = None
  s = '%1.2f_%1.2f' % (start, end)
  ss = '_' + s.replace('.', 'p')
  if xory == 'x':
    bounds['start'] = h2D.GetXaxis().FindBin(start)
    bounds['end']   = h2D.GetXaxis().FindBin(end  )
    h1D = h2D.ProjectionX('hx'+h2D.GetName()+ss, bounds['start'], bounds['end'])
    print bounds['start'], bounds['end']
  if xory == 'y':
    bounds['start'] = h2D.GetYaxis().FindBin(start)
    bounds['end']   = h2D.GetYaxis().FindBin(end  )
    h1D = h2D.ProjectionY('hy'+h2D.GetName()+ss, bounds['start'], bounds['end'])
    print bounds['start'], bounds['end']

  return h1D

r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

f = r.TFile.Open('PA8TeV_pPb_LamC_MC_20210302_effHists.root')
types = ['matchLc', 'peakLc', 'sideLc']
particles = ['pip', 'pim', 'proton']
variables = ['dPt', 'chi2']
dRCut = [0.03, 0.05, 0.1]
h2Ds = {}
h1Ds = {}
for t in types:
  for p in particles:
    for v in variables:
      h2Ds[t+'_'+p+'_'+v+'vsdR'] = f.Get('h'+t+'_'+p+'_'+v+'vsdR')
      for c in dRCut:
        print c
        temp = projectTo1D(h2Ds[t+'_'+p+'_'+v+'vsdR'], 'y', 0., c)
        h1Ds[temp.GetName()] = temp
    tempX = projectTo1D(h2Ds[t+'_'+p+'_'+'dPt'+'vsdR'], 'x', 0., 1.)
    tempX.SetName(tempX.GetName().replace('_0p00_1p00','').replace('dPtvs',''))
    print tempX.GetName()
    h1Ds[tempX.GetName()] = tempX

# chi2 distro
#drawComp('sideLc_proton_chi2_dRComp', h1Ds['hyhsideLc_proton_chi2vsdR_0p00_0p10'], h1Ds['hyhsideLc_proton_chi2vsdR_0p00_0p03'])
for k, v in h1Ds.items():
  v.Sumw2()
  v.Scale(1./v.Integral(0, 100000000)/v.GetBinWidth(1))

# chi2, pt distro, three dRs
for t in types:
  for p in particles:
    for v in variables:
      c = r.TCanvas("distro_"+t+'_'+p+'_'+v, "", 500, 400)
      colors = [r.kRed, r.kBlue, r.kGreen]
      hframeName = 'hyh' + t + '_' + p + '_' + v + 'vsdR_0p00_0p03'
      h1Ds[hframeName].SetMarkerStyle(20)
      h1Ds[hframeName].SetTitle(h1Ds[hframeName].GetTitle() + ', ' + p)
      h1Ds[hframeName].Draw()
      leg = r.TLegend(0.6, 0.65, 0.89, 0.89)
      for cut, col in zip(dRCut, colors):
        s = '%1.2f_%1.2f' % (0., cut)
        ss = '_' + s.replace('.', 'p')
        hname = 'hyh' + t + '_' + p + '_' + v + 'vsdR' + ss
        h1Ds[hname].SetLineColor(col)
        h1Ds[hname].SetMarkerColor(col)
        h1Ds[hname].Draw("ESAME")
        leg.AddEntry(h1Ds[hname], "dR<%.2f" % cut, "l")
      leg.Draw()
      c.Print('plots/' + c.GetName() + '.png')

# peak, side comp, three dRs
for cut in dRCut:
  s = '%1.2f_%1.2f' % (0., cut)
  ss = '_' + s.replace('.', 'p')
  for p in particles:
    for v in variables:
      c = r.TCanvas("distro_"+p+'_'+v+'dR'+ss, "", 500, 400)
      colors = [r.kRed, r.kBlue]
      hframeName = 'hyhpeakLc_' + p + '_' + v + 'vsdR' + ss
      h1Ds[hframeName].SetMarkerStyle(20)
      h1Ds[hframeName].SetTitle(h1Ds[hframeName].GetTitle() + ', dR<%.2f' % cut + ', ' +p)
      h1Ds[hframeName].Draw()
      leg = r.TLegend(0.6, 0.65, 0.89, 0.89)
      newtypes = list(types)
      del newtypes[0]
      for t, col in zip(newtypes, colors):
        hname = 'hyh' + t + '_' + p + '_' + v + 'vsdR' + ss
        h1Ds[hname].SetLineColor(col)
        h1Ds[hname].SetMarkerColor(col)
        h1Ds[hname].Draw("ESAME")
        leg.AddEntry(h1Ds[hname], t, "l")
      leg.Draw()
      c.Print('plots/' + c.GetName() + '.png')
# dR under dPtRel = 0.5
h1DdRs = {}
for p in particles:
  c = r.TCanvas("distro_"+p+'_dR', "", 500, 400)
  colors = [r.kRed, r.kBlue]
  hframeName = 'hxhpeakLc_' + p + '_' + 'dR'
  h1Ds[hframeName].SetMarkerStyle(20)
  h1Ds[hframeName].SetTitle(h1Ds[hframeName].GetTitle() + ', ' +p)
  h1Ds[hframeName].Draw()
  leg = r.TLegend(0.6, 0.65, 0.89, 0.89)
  newtypes = list(types)
  del newtypes[0]
  for t, col in zip(newtypes, colors):
    hname = 'hxh' + t + '_' + p + '_' + 'dR'
    h1Ds[hname].SetLineColor(col)
    h1Ds[hname].SetMarkerColor(col)
    h1Ds[hname].Draw("ESAME")
    leg.AddEntry(h1Ds[hname], t, "l")
  leg.Draw()
  c.Print('plots/' + c.GetName() + '.png')
