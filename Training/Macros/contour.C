void contour()
{
  using namespace ROOT;
  const auto PathToPPb = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/MCNtuples/MC_pPb_lambdacAna_mc_AllEntries_LamCKsP.root";
  const auto PathToPbP = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/MCNtuples/MC_Pbp_lambdacAna_mc_AllEntries_LamCKsP.root";
  auto dfPPb = RDataFrame("lambdacAna_mc/ParticleNTuple", PathToPPb);
  auto dfPbP = RDataFrame("lambdacAna_mc/ParticleNTuple", PathToPbP);
  const auto PathToPPbData = "/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/DataNTuples/dataHM_pPb_0000_lambdacAna_AllEntries_LamCKsP.root";
  auto dfPPbData = RDataFrame("lambdacAna/ParticleNTuple", PathToPPbData);
  const double mass_PDG = 2.2865;
  const double peak = 0.03;
  const double side_up = 0.11;
  const double side_lw = 0.05;
  auto inPeak = [peak, mass_PDG] (float mass) { return abs(mass - mass_PDG) < peak; };
  auto inSideBand = [side_up, side_lw, mass_PDG] (float mass) { return abs(mass-mass_PDG) < side_up && abs(mass-mass_PDG) > side_lw; };
  auto dfPPbPeak = dfPPb.Filter(inPeak, {"cand_mass"});
  auto dfPbPPeak = dfPbP.Filter(inPeak, {"cand_mass"});
  auto dfPPbDataSide = dfPPbData.Filter(inSideBand, {"cand_mass"});
  auto dfPPbMid = dfPPbPeak.Filter("abs(cand_y)<1");
  auto dfPbPMid = dfPbPPeak.Filter("abs(cand_y)<1");
  auto dfPPbDataMid = dfPPbDataSide.Filter("abs(cand_y)<1");
  const double pTMin = 3.0;
  const double pTMax = 4.0;
  auto pTFilter = [=] (float pT) { return pT > pTMin && pT < pTMax; };
  auto dfPPbMidPt3to4 = dfPPb.Filter(pTFilter, {"cand_pT"});
  auto dfPbPMidPt3to4 = dfPbP.Filter(pTFilter, {"cand_pT"});
  auto dfPPbDataMidPt3to4 = dfPPbData.Filter(pTFilter, {"cand_pT"});
  auto hDeDxVsP_PPb = dfPPbMidPt3to4.Define("cand_dau1_p", [](float pT, float eta) { return pT * cosh(eta); }, {"cand_pTDau1", "cand_etaDau1"})
        .Histo2D({"hDeDxVsP_PPb_MC", ";Proton p;dE/dx", 100, 0, 5, 100, 0.5, 15}, "cand_dau1_p", "trk_dau1_dEdx_dedxHarmonic2");
  auto hDeDxVsP_PPb_Data = dfPPbDataMidPt3to4.Define("cand_dau1_p", [](float pT, float eta) { return pT * cosh(eta); }, {"cand_pTDau1", "cand_etaDau1"})
        .Histo2D({"hDeDxVsP_PPb_Data", ";Proton p;dE/dx", 100, 0, 5, 100, 0.5, 15}, "cand_dau1_p", "trk_dau1_dEdx_dedxHarmonic2");
  TCanvas c("c", "", 1200, 450);
  c.Divide(2, 1);
c.Draw();
c.cd(1);
hDeDxVsP_PPb->Draw("COLZ");
c.cd(2);
hDeDxVsP_PPb_Data->Draw("COLZ");

TF1 f1("f1", "[0]*exp([1]*x)*exp([1]*x)+[2]", 0.3, 1.5);

f1.SetParameter(0, 2.6);
f1.SetParameter(1, -1);
//f1.SetParameter(1, 1);
f1.SetParameter(2, 5);
c.cd(1);
//f1.Draw("SAME");
auto hprof = hDeDxVsP_PPb->ProfileX("s");
hprof->Draw("SAME");
TGraphErrors g(hprof);
hprof->Fit(&f1, "R", "", 0.5, 1.5);
hprof->Fit(&f1, "E");
TH1D h("h", "", 100, 0, 5);
h.SetFillColor(kBlack);
(TVirtualFitter::GetFitter())->GetConfidenceIntervals(&h);
h.Draw("SAME");
f1.Draw("SAME");

TF1 f2("f1", "[0]*exp([1]*x)*exp([1]*x)+[2]", 0.3, 1.5);
f2.SetParameter(0, 0.4*f1.GetParameter(0));
f2.SetParameter(1, f1.GetParameter(1));
f2.SetParameter(2, f1.GetParameter(2));
f2.Draw("SAME");
double mycontour[] = {5};
TH2D* hsignal = new TH2D(*hDeDxVsP_PPb);
hsignal->SetContour(1, mycontour);
hsignal->Draw("CONT Z LIST");
c.Update();

   TObjArray *conts = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
   //gROOT->GetListOfSpecials()->Print();
   TList* contLevel = NULL;
   TGraph* curv     = NULL;
   TGraph* gc       = NULL;

   Int_t nGraphs    = 0;
   Int_t TotalConts = 0;
 
   if (conts == NULL){
      printf("*** No Contours Were Extracted!\n");
      TotalConts = 0;
   } else {
      TotalConts = conts->GetSize();
   }
cout << TotalConts << endl;
   for(int i = 0; i < TotalConts; i++){
      contLevel = (TList*)conts->At(i);
      printf("Contour %d has %d Graphs\n", i, contLevel->GetSize());
      nGraphs += contLevel->GetSize();
   }

   Double_t xval0, yval0, zval0;
      char val[20];
   for(int i = 0; i < TotalConts; i++){
      contLevel = (TList*)conts->At(i);
zval0 = mycontour[i];
      printf("Z-Level Passed in as:  Z = %f\n", zval0);
      // Get first graph from list on curves on this level
      curv = (TGraph*)contLevel->First();
      for(int j = 0; j < contLevel->GetSize(); j++){
         curv->GetPoint(0, xval0, yval0);
         if (zval0<0) curv->SetLineColor(kRed);
         if (zval0>0) curv->SetLineColor(kBlue);
         nGraphs ++;
         printf("\tGraph: %d  -- %d Elements\n", nGraphs,curv->GetN());
 
         // Draw clones of the graphs to avoid deletions in case the 1st
         // pad is redrawn.
         gc = (TGraph*)curv->Clone();
         gc->Draw("C");
 
         sprintf(val,"%g",zval0);
         curv = (TGraph*)contLevel->After(curv); // Get Next graph
      }
   }

c.cd(2);
f1.Draw("SAME");
c.Draw();
}
