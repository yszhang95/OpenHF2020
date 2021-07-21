#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include <memory>

#include <algorithm>
#include <climits>
#include <numeric>

#include <fstream>

#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
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
using std::map;

int skimTree(const TString& inputList, const TString& treeDir,
               const TString& postfix, const TString& outDir,
               Long64_t nentries=-1)
{
  TString basename(gSystem->BaseName(inputList));
  const auto firstPos = basename.Index(".list");
  basename.Replace(firstPos, 5, "_");
  basename += nentries > 0 ? Form("%s%lld_", treeDir.Data(), nentries) : (treeDir + "_AllEntries_");
  basename += postfix + "_lumi.txt";
  TString outName;
  if (outDir == "") outName = basename;
  else outName = outDir + "/" + basename;
  std::ofstream ofile(outName.Data());
  cout << "Created " << outName.Data() << endl;

  // lumi buffer
  std::map<UInt_t, float> lumis;
  float accLumi = 0;

  // reader
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

    if (!lumis.count(p.LSNb())) accLumi += p.hltRecordLumi().at(0); // hlt high multiplicity

    // get lumi;
    ofile << p.EventNb() << "," << p.LSNb() << "," << p.RunNb() << "," << p.hltRecordLumi().at(0) << endl;
  } // end loop
  ofile.close();
  cout << "Accumulated luminosity is " << accLumi << " ub^-1" << endl;
  return 0;
}
