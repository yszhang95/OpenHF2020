#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include <memory>

#include <algorithm>
#include <climits>
#include <numeric>

#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TString.h"
#include "TSystem.h"

#include "TreeReader/ParticleTreeData.hxx"
#include "Ana/Common.h"
#include "Ana/TreeHelpers.h"

using std::cerr;
using std::cout;
using std::endl;

using std::vector;
using std::string;
using std::list;
using std::map;
using std::tuple;

using std::shared_ptr;

using PtEtaPhiM_t = ROOT::Math::PtEtaPhiMVector;

using MatchPairInfo =  tuple<size_t, size_t, double, double>;

int skimTree(const TString& inputList, const TString& treeDir,
               const TString& postfix, const TString& outDir,
               Particle particle, KineCut kins,
               Long64_t nentries=-1)
{
  TString basename(gSystem->BaseName(inputList));
  const auto firstPos = basename.Index(".list");
  basename.Replace(firstPos, 5, "_");
  basename += nentries > 0 ? Form("%s%lld_", treeDir.Data(), nentries) : (treeDir + "_AllEntries_");
  basename += postfix + ".root";
  TString outName;
  if (outDir == "") outName = basename;
  else outName = outDir + "/" + basename;
  TFile ofile(outName, "recreate");
  cout << "Created " << ofile.GetName() << endl;

  // new TTree
  ofile.mkdir(treeDir);
  ofile.cd(treeDir);
  TH1I hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0, 300);
  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  ntp.isMC = false;
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initNTuple();

  TFileCollection tf("tf", "", inputList);
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTreeData p(&t);

  if(nentries < 0) nentries = p.GetEntries();
  cout << "Tree " << treeDir << "/ParticleTree in " << inputList
    << " has " << nentries << " entries." << endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    p.GetEntry(ientry);

    // check pileup filter
    if (!p.evtSel().at(4)) continue;
    hNtrkoffline.Fill(p.Ntrkoffline());

    const auto recosize = p.cand_mass().size();

    const auto& pdgId = p.cand_pdgId();

    // loop over particles
    for (size_t ireco=0; ireco<recosize; ireco++) {
      // begin LambdaC
      if (pdgId[ireco] == std::abs(particle.id())) {
        const auto dauIdx = p.cand_dauIdx().at(ireco);
        // 0 for Ks, 1 for proton
        const auto gDauIdx = p.cand_dauIdx().at(dauIdx.at(0));

        // check it is not a Lambda/Anti-Lambda
        PtEtaPhiM_t gDau0P4 = getRecoDauP4(dauIdx.at(0), 0, p); // pi- from Ks
        PtEtaPhiM_t gDau1P4 = getRecoDauP4(dauIdx.at(0), 1, p); // pi+ from Ks
        if (gDau0P4.P() > gDau1P4.P()) {
          gDau0P4.SetM(0.938272081);
        } else {
          gDau1P4.SetM(0.938272081);
        }
        PtEtaPhiM_t total = gDau0P4 + gDau1P4;
        if ( std::abs(total.M() - 1.11568) < 0.02) { continue; }
        // dielectron cross check
        gDau0P4.SetM(0.000511);
        gDau1P4.SetM(0.000511);
        PtEtaPhiM_t ee = gDau0P4 + gDau1P4;
        if ( ee.M() <0.2 ) { continue; }

        // retrieve other information
        ntp.hltRecordLumi = p.hltRecordLumi().at(0); // 0 means high multiplicity 185
        ntp.retrieveTreeInfo(p, ireco);
        // fill this entry
        ntp.fillNTuple();
      } // end LambdaC
    } // end looping over particles
  } // end loop
  ofile.Write();
  return 0;
}
