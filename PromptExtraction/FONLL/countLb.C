#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
// R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/TreeReader/ParticleTreeMC.hxx"
#include "Utilities/Ana/Common.h"
#include "Utilities/Ana/TreeHelpers.h"
#include <vector>
#include <memory>

#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TString.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"

using std::vector;
using std::abs;
void produce2DPlots(const char* inputList)
{
  const vector<double> LbPt = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10., 11., 12., 13., 14., 16., 18., 20., 25.};
  //const vector<double> LcPt = {3.0, 4.0, 5.0, 6.0, 8.0, 10.};
  const vector<double> LcPt = {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.};
  TH2D hLcVsLb("hLcVsLb", ";Lb pT;Lc pT", LbPt.size()-1, LbPt.data(), LcPt.size()-1, LcPt.data());
  TH1D hLbPt("hLbPt", ";Lb pT", LbPt.size()-1, LbPt.data());
  TH1D hLcPt("hLcPt", ";Lb pT", LcPt.size()-1, LcPt.data());

  TFile ofile("EvtGen.root", "recreate");

  TString treeDir = "lambdacAna_mc";
  TFileCollection tf("tf", "", inputList);
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTreeMC p(&t);

  Long64_t nentries = -1;
  if(nentries < 0) nentries = p.GetEntriesFast();

  Particle particle(4122);
  Particle Ks(310);
  Ks.selfConj(true);
  Ks.longLived(true);
  particle.addDaughter(Ks);
  Particle proton(2212);
  particle.addDaughter(proton);

  Long64_t nlb=0;
  Long64_t nother=0;
  Long64_t ntotal=0;

  Long64_t ncount = 0;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0) break;
    p.GetEntry(ientry);
    ++ntotal;
    if (ncount>1000) break;
    const auto& pdgId = p.gen_pdgId();
    for (size_t ipar=0; ipar<pdgId.size(); ++ipar) {
      if (abs(pdgId.at(ipar)) == 4122) {
        auto momPdgIdIdx = p.gen_momIdx().at(ipar).at(0);
        if (abs(pdgId.at(momPdgIdIdx)) == 5122) ++nlb;
        if (abs(pdgId.at(momPdgIdIdx)) != 5122) {
          unsigned int mypdgid = abs(pdgId.at(momPdgIdIdx)) ;
          auto idstr = std::to_string( mypdgid );
          std::cout << idstr << "\n";
          if (idstr.at(0) == '5') {
            ++nother;
            //cout << "MomId: " << pdgId.at(momPdgIdIdx) << endl;
          }
        }
        if (abs(pdgId.at(momPdgIdIdx)) != 5122) continue;
        Particle particle_copy(particle);
        if (!checkDecayChain(particle_copy, ipar, p)) continue;
        particle_copy.setTreeIdx(ipar);
        auto parLbPt = p.gen_pT().at(momPdgIdIdx);
        auto parLcPt = p.gen_pT().at(ipar);
        if (abs(p.gen_y().at(ipar)) > 1.) continue;
        hLcPt.Fill(parLcPt);
        if (abs(p.gen_y().at(momPdgIdIdx)) > 1.) continue;
        hLcVsLb.Fill(parLbPt, parLcPt);
        hLbPt.Fill(parLbPt);
      }
    }
  }
  ofile.cd();
  hLcVsLb.Write();
  hLbPt.Write();
  hLcPt.Write();
  cout << "Total: " << ntotal << endl;
  cout << "Lambda_b: " << nlb << endl;
  cout << "Other: " << nother << endl;
}
