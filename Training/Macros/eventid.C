using namespace ROOT;
/*
void eventid()
{
  int i=0;
  auto myindex = [&i] () {return (i++);};
  auto df = RDataFrame("lambdacAna_mc/ParticleNTuple", "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/MC_*lambdacAna_mc_AllEntries_LamCKsP_weighted.root");
  auto mydf = df.Define("eventID", myindex);
  mydf.Snapshot("lambdacAna_mc/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/LambdaC_KsProton_mc_weighted_eventID.root");
}
*/

/*
void eventid()
{
  int i=0;
  auto myindex = [&i] () {return (i++);};
  auto df = RDataFrame("lambdacAna/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/data_*lambdacAna_AllEntries_LamCKsP_weighted.root");
  //auto mydf = df.Filter("abs(cand_mass-2.29)<0.027 && abs(cand_y)<1.0").Define("eventID", myindex);
  auto mydf = df.Filter("abs(cand_mass-2.29)>0.025 && abs(cand_mass-2.29)<0.051 && abs(cand_y)<1.0").Define("eventID", myindex);
  mydf.Snapshot("lambdacAna/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/LambdaC_KsProton_weighted_eventID.root");
}
*/

void eventid(bool isMC, TString files, TString output)
{
  TString treeDir = isMC ? "lambdacAna_mc/ParticleNTuple" : "lambdacAna/ParticleNTuple";
  int i=0;
  auto myindex = [&i] () {return (i++);};
  auto df = RDataFrame(treeDir.Data(), Form("root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/%s", files.Data()));
  TString filter = "abs(cand_y)<1.0";
  if (!isMC) 
    filter += " && abs(cand_mass-2.29)>0.1 && abs(cand_mass-2.29)<0.14";
    //filter += " && abs(cand_mass-2.29)>0.06 && abs(cand_mass-2.29)<0.1";
    //filter += " && abs(cand_mass-2.29)>0.025 && abs(cand_mass-2.29)<0.051";
  auto mydf = df.Filter(filter.Data()).Define("eventID", myindex);
  mydf.Snapshot(treeDir.Data(), Form("root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/%s", output.Data()));
}
