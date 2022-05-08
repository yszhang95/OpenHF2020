#if defined(__CLING__)
#endif
#ifndef MyGenNTuple_H
#include "Utilities/Ana/MyGenNTuple.h"
#endif

#ifndef ParticleTreeMC_hxx
#include "Utilities/TreeReader/ParticleTreeMC.hxx"
#endif

#ifndef ParticleTreeData_hxx
#include "Utilities/TreeReader/ParticleTreeData.hxx"
#endif

#ifndef HelpClass_H
#include "Utilities/Ana/Common.h"
#endif

#ifndef TreeHelpers_H
#include "Utilities/Ana/TreeHelpers.h"
#endif

#include <vector>
#include <memory>

#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TString.h"
#include "TSystem.h"

using std::cerr;
using std::cout;
using std::endl;

using std::vector;
using std::string;

struct Config
{
public:
  Config() : Config(-1) {}
  Config(const Long64_t n):
    _nentries(n) {}
  Config(const Long64_t n,
         const TString inputList,
         const TString treeDir):
    _nentries(n), _inputList(inputList), _treeDir(treeDir)
  {}
  TString GetInputList() const { return _inputList; }
  void SetInputList(const TString s) { _inputList = s; }
  TString GetTreeDir() const { return _treeDir; }
  void SetTreeDir(const TString s) { _treeDir = s; }
  TString GetPostfix() const { return _postfix; }
  void SetPostfix(const TString s) { _postfix = s; }
  TString GetOutDir() const { return _outDir; }
  void SetOutDir(const TString s) { _outDir = s; }
  Long64_t GetNEntries() const { return _nentries; }
  void SetNEntries(Long64_t n) { _nentries = n; }
  bool flipEta() const { return _flipEta; }
  void SetFlipEta(const bool flip) { _flipEta = flip; }
  void SetKeptBranchNames(const std::vector<TString> names)
  {
    _keptBranchNames = names;
  }
  std::vector<TString> GetKeptBranchNames() const
  { return _keptBranchNames; }

private:
  std::vector<TString> _keptBranchNames;
  TString _inputList;
  TString _treeDir;
  TString _postfix;
  TString _outDir;
  Long64_t _nentries;
  bool     _flipEta;
};

int saveGenTuple(const Config& conf, Particle particle)
{
  const auto inputList = conf.GetInputList();
  const auto treeDir = conf.GetTreeDir();
  const auto postfix = conf.GetPostfix();
  const auto outDir = conf.GetOutDir();
  auto nentries = conf.GetNEntries();
  const auto flipEta = conf.flipEta();
  const auto keptBranches = conf.GetKeptBranchNames();

  TString basename(gSystem->BaseName(inputList));
  auto firstPos = basename.Index(".list");
  if (firstPos>0) {
    basename.Replace(firstPos, 5, "_");
  }
  if (basename[basename.Length()-1] != '_')
    basename.Append('_');
  firstPos = basename.Index(".");
  while (firstPos>0) {
    basename.Replace(firstPos, 1, "_");
    firstPos = basename.Index("__");
    while (firstPos>0) {
      basename.Replace(firstPos, 2, "_");
      firstPos = basename.Index("__");
    }
    firstPos = basename.Index(".");
  }

  basename += nentries > 0
    ? Form("%s%lld_", treeDir.Data(), nentries) :
    (treeDir + "_AllEntries");
  if (postfix.Length()) basename += "_";
  basename += postfix + ".root";

  TString outName;
  if (outDir == "") outName = basename;
  else outName = outDir + "/" + basename;
  if (flipEta) {
    auto index = outName.Index(".root");
    if (index >0) {
      outName.Replace(index, 5, "_etaFlipped.root");
    }
  }

    TFile ofile(outName, "recreate");
  cout << "Created " << ofile.GetName() << endl;

  // new TTree
  ofile.mkdir(treeDir);
  ofile.cd(treeDir);

  TFileCollection tf("tf", "", inputList);
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTreeMC p(&t);

  ofile.cd(treeDir);
  TTree tt("genNTuple", "genNTuple");
  MyGenNTuple ntp(&tt);
  ntp.flipEta = flipEta;
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initGenBranches();
  if (!keptBranches.empty()) ntp.pruneNTuple(keptBranches);
  if(nentries < 0) nentries = p.GetEntriesFast();


  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0) break;
    p.GetEntry(ientry);
    const auto& pdgId = p.gen_pdgId();
    for (size_t ipar=0; ipar<pdgId.size(); ++ipar) {
      if (abs(pdgId.at(ipar)) == 4122) {
        Particle particle_copy(particle);
        if (!checkDecayChain(particle_copy, ipar, p)) continue;
        particle_copy.setTreeIdx(ipar);
        ntp.retrieveGenInfo(p, &particle_copy);
        ntp.fillNTuple();
      }
    }
  }
  cout << "Done" << endl;
  ofile.cd(treeDir);
  ntp.t->Write();
  return 0;
}
