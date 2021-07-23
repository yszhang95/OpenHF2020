using namespace ROOT;
void eventid()
{
  int i=0;
  auto myindex = [&i] () {return (i++);};
  auto df = RDataFrame("lambdacAna_mc/ParticleNTuple", "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/MC_*lambdacAna_mc_AllEntries_LamCKsP_weighted.root");
  auto mydf = df.Define("eventID", myindex);
  mydf.Snapshot("lambdacAna_mc/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/LambdaC_KsProton_mc_weighted_eventID.root");
}

/*
void eventid()
{
  int i=0;
  auto myindex = [&i] () {return (i++);};
  auto df = RDataFrame("lambdacAna/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/data_*lambdacAna_AllEntries_LamCKsP_weighted.root");
  auto mydf = df.Filter("abs(cand_mass-2.29)>0.15 && abs(cand_mass-2.29)<0.20").Define("eventID", myindex);
  mydf.Snapshot("lambdacAna/ParticleNTuple", "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/LambdaC_KsProton_weighted_eventID.root");
}
*/
