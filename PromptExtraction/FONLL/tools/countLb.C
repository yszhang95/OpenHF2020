#include <iostream>
#include <vector>
#include <memory>

#include "TFile.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TString.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"

#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyTreeReader.so)
#endif

/*
 * Uncomment the following if you want to compile
 */
/*
#include "Utilities/TreeReader/ParticleTree.hxx"
#include "Utilities/TreeReader/ParticleTreeMC.hxx"
#include "Utilities/Ana/Common.h"
#include "Utilities/Ana/TreeHelpers.h"
*/

using std::vector;
using std::abs;

// ymins = [-3.46, -2.46, -1.46, -0.46, 0.54, 1.54]
// ymaxs = [-2.46, -1.46, -0.46, 0.54, 1.54, 2.54]
// ptmins = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28]
//  ptmaxs = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 30]

template<typename Func, typename T>
int get_last_idx(const std::vector<T>& v, Func f)
{
  const auto it = std::find_if(v.rbegin(), v.rend(), f);
  if (it!=v.rend()) {
    int idx = v.size() - 1 - (it - v.rbegin());
    return idx;
  }
  return -1;
}

void produce2DPlots(const char* inputList)
{
  TFile ofile("EvtGen.root", "recreate");
  // const vector<double> LbPt = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10., 11., 12., 13., 14., 16., 18., 20., 25.};
  //const vector<double> LcPt = {3.0, 4.0, 5.0, 6.0, 8.0, 10.};
  // const vector<double> LcPt = {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.};
  // TH2D hLcVsLb("hLcVsLb", ";Lb pT;Lc pT", LbPt.size()-1, LbPt.data(), LcPt.size()-1, LcPt.data());
  // TH1D hLbPt("hLbPt", ";Lb pT", LbPt.size()-1, LbPt.data());
  // TH1D hLcPt("hLcPt", ";Lb pT", LcPt.size()-1, LcPt.data());

  // 2D
  const std::vector<double> pts_Lc = {2, 3, 4, 5, 6, 8, 10};
  std::vector<double> pts = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28};
  std::vector<double> yMin = {-3, -2, -1, 0, 1, 2};
  std::vector<double> yMax = {-2, -1, 0, 1, 2, 3};

  std::vector<TH1D*> hLb_all;
  std::vector<TH1D*> hB_all;
  std::vector<std::vector<TH1D* > > hLb_pass;
  std::vector<std::vector<TH1D* > > hB_pass;
  for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
    std::vector<TH1D*> hLbs;
    std::vector<TH1D*> hBs;
    for (int iy=0; iy<yMin.size(); ++iy) {
      std::string Lb_name = ::Form("hLb_pass_pt%d_y%d", ipt, iy);
      std::string Lb_title = ::Form("#Lambda_{c}^{+} p_{T} %.1f--%.1f #Lambda_{b}^{0} y %.1f to %.1f",
          pts_Lc.at(ipt), pts_Lc.at(ipt+1), yMin.at(iy), yMax.at(iy));
      TH1D* hLb = new TH1D(Lb_name.c_str(), Lb_title.c_str(), pts.size()-1, pts.data());
      hLbs.push_back(hLb);

      std::string B_name = ::Form("hB_pass_pt%d_y%d", ipt, iy);
      std::string B_title = ::Form("#Lambda_{c}^{+} p_{T} %.1f--%.1f B^{+/0} y %.1f to %.1f",
          pts_Lc.at(ipt), pts_Lc.at(ipt+1), yMin.at(iy), yMax.at(iy));
      TH1D* hB = new TH1D(B_name.c_str(), B_title.c_str(), pts.size()-1, pts.data());
      hBs.push_back(hB);
    }
    hLb_pass.push_back(hLbs);
    hB_pass.push_back(hBs);
  }
  for (int iy=0; iy<yMin.size(); ++iy) {
    std::string Lb_name = ::Form("hLb_all_y%d", iy);
    std::string Lb_title = ::Form("#Lambda_{b}^{0} y %.1f to %.1f",
        yMin.at(iy), yMax.at(iy));
    TH1D* hLb = new TH1D(Lb_name.c_str(), Lb_title.c_str(), pts.size()-1, pts.data());
    hLb_all.push_back(hLb);

    std::string B_name = ::Form("hB_all_y%d", iy);
    std::string B_title = ::Form("B^{+/0} y %.1f to %.1f",
        yMin.at(iy), yMax.at(iy));
    TH1D* hB = new TH1D(B_name.c_str(), B_title.c_str(), pts.size()-1, pts.data());
    hB_all.push_back(hB);
  }


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
  Long64_t nBu=0;
  Long64_t nBd=0;
  Long64_t nbquark=0;
  Long64_t nbdecay=0;
  Long64_t nother=0;
  Long64_t ntotal=0;

  Long64_t ncount = 0;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0) break;
    p.GetEntry(ientry);
    ++ntotal;
    // if (ncount++>1000) break;
    const auto& pdgId = p.gen_pdgId();
    for (size_t ipar=0; ipar<pdgId.size(); ++ipar) {
      // inclusive Lambda_c from b
      if (abs(pdgId.at(ipar)) == 4122) {

        Particle particle_copy(particle);
        if (!checkDecayChain(particle_copy, ipar, p)) continue;
        particle_copy.setTreeIdx(ipar);

        std::vector<int> momIds;
        std::vector<int> momIdxs;
        for (auto momIdxVec = p.gen_momIdx().at(ipar);
              !momIdxVec.empty(); ) {
          int momIdx = momIdxVec.at(0);
          momIdxs.push_back(momIdx);
          // pdgId = momId;
          int momId = p.gen_pdgId().at(momIdx);
          momIds.push_back(momId);
          // update it
          momIdxVec = p.gen_momIdx().at(momIdx);
        }
        // B meson
        auto Bmeson = [](int c) -> bool { return abs(c) < 550&& abs(c) > 500; };
        auto Bu = [](int c) -> bool { return abs(c) > 520 && abs(c) < 530; };
        auto Bd = [](int c) -> bool { return abs(c) > 510 && abs(c) < 520; };
        // Lambda_b
        auto Lambda_b = [](int c) -> bool { return abs(c) < 6000 && abs(c) > 5000; };
        // b quark
        auto bquark = [](int c) -> bool { return abs(c) == 5; };
        auto bdecay = [](int c) -> bool { return std::to_string(abs(c))[0] == '5'; };

        auto has_Lambda_b = std::find_if(momIds.begin(), momIds.end(), Lambda_b) != momIds.end();
        auto has_Bu = std::find_if(momIds.begin(), momIds.end(), Bu) != momIds.end();
        auto has_Bd = std::find_if(momIds.begin(), momIds.end(), Bd) != momIds.end();
        auto has_bquark = std::find_if(momIds.begin(), momIds.end(), bquark) != momIds.end();
        auto has_bdecay = std::find_if(momIds.begin(), momIds.end(), bdecay) != momIds.end();

        // last copy
        auto idx_Bmeson = get_last_idx(momIds, Bmeson);
        auto idx_Lambda_b = get_last_idx(momIds, Lambda_b);

          const auto y_lc = p.gen_y().at(ipar);
          const auto pt_lc = p.gen_pT().at(ipar);

        if (idx_Bmeson > 0) {
          auto momIdx = momIdxs.at(idx_Bmeson);
          const auto y = p.gen_y().at(momIdx);
          const auto pt = p.gen_pT().at(momIdx);
          // inclusive
          for (int iy=0; iy<yMin.size(); ++iy) {
            if (y>yMin.at(iy) && y<yMax.at(iy)) {
              // modify
              hB_all.at(iy)->Fill(pt);
              // Lambda_c pt and y
              if (abs(y_lc)<1) {
                for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
                  if (pt_lc < pts_Lc.at(ipt+1) && pt_lc > pts_Lc.at(ipt)) {
                    // modify
                    hB_pass.at(ipt).at(iy)->Fill(pt);
                  }
                }
              }
            }
          }
        }
        if (idx_Lambda_b > 0) {
          auto momIdx = momIdxs.at(idx_Lambda_b);
          const auto y = p.gen_y().at(momIdx);
          const auto pt = p.gen_pT().at(momIdx);
          // inclusive
          for (int iy=0; iy<yMin.size(); ++iy) {
            if (y>yMin.at(iy) && y<yMax.at(iy)) {
              // modify
              hLb_all.at(iy)->Fill(pt);
              // Lambda_c pt and y
              if (abs(y_lc)<1) {
                for (int ipt=0; ipt<pts_Lc.size()-1; ++ipt) {
                  if (pt_lc < pts_Lc.at(ipt+1) && pt_lc > pts_Lc.at(ipt)) {
                    // modify
                    hLb_pass.at(ipt).at(iy)->Fill(pt);
                  }
                }
              }
            }
          }
        }



        if (has_Lambda_b) { ++nlb; }
        if (has_Bu) { ++nBu; }
        if (has_Bd) { ++nBd; }
        if (has_bquark) { ++nbquark; }
        if (has_bdecay) { ++nbdecay; }

        // Lambda_b
        // Bu + Bd
        
        // if (abs(pdgId.at(momPdgIdIdx)) != 5122) continue;
        // auto parLbPt = p.gen_pT().at(momPdgIdIdx);
        // auto parLcPt = p.gen_pT().at(ipar);
        // if (abs(p.gen_y().at(ipar)) > 1.) continue;
        // hLcPt.Fill(parLcPt);
        // if (abs(p.gen_y().at(momPdgIdIdx)) > 1.) continue;
        // hLcVsLb.Fill(parLbPt, parLcPt);
        // hLbPt.Fill(parLbPt);
      }
    }
  }
  ofile.cd();
  ofile.Write();
  cout << "Total: " << ntotal << endl;
  cout << "Lambda_b: " << nlb << endl;
  cout << "Bu: " << nBu << endl;
  cout << "Bd: " << nBd << endl;
  cout << "bquark: " << nbquark<< endl;
  cout << "bdecay: " << nbdecay<< endl;
  cout << "Other: " << nother << endl;

  std::vector<int> a{1, 2, 3, 33, 4};
  auto has_3 = [](int i) -> bool { return std::to_string(i)[0] == '3'; };
  int idx = get_last_idx(a, has_3);
  std::cout << "last copy index" << idx << " And it is " << a[idx] << "\n";

  for (auto& v : hLb_all) delete v;
  for (auto& v : hB_all) delete v;
  for (auto& v : hLb_pass) {
    for (auto e : v) delete e;
  }
  for (auto& v : hB_pass) {
    for (auto e : v) delete e;
  }
}
