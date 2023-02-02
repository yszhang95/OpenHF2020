using namespace ROOT;
void skimNTuple(std::string inputfile)
{
    gROOT->SetBatch(kTRUE);
    std::cout << "Starting skimNTuple.C\n";
    // histograms
    std::cout << "Reading histograms\n";
    TFile infile(inputfile.c_str());
    auto h1 = infile.Get<TH1D>("lambdacAna/hNtrkoffline");
    auto h2 = infile.Get<TH1D>("lambdacAna/hNtrkofflineDz1p0");

    TFile ofile("output.root", "recreate");
    ofile.cd();
    ofile.mkdir("lambdacAna");
    ofile.cd("lambdacAna");
    h1->Write();
    h2->Write();
    // close instances
    infile.Close();
    ofile.Close();
    std::cout << "Saved histograms\n";

    // skim tree
    std::cout << "Reading trees\n";
    ROOT::RDF::RSnapshotOptions opts;
    opts.fMode = "update";

    RDataFrame df("lambdacAna/ParticleNTuple", inputfile);
    auto dfSkim = df.Filter("cand_dau0_decayLength3D/cand_dau0_decayLengthError3D>5")
        .Filter("cos(cand_dau0_angle3D) > 0.999").Filter("cand_dau0_dca < 0.5")
        .Filter("filterBit_4 > 0.5");
    dfSkim.Snapshot("lambdacAna/ParticleNTuple", "output.root",
            {"cand_mass", "cand_pT", "cand_y", "cand_phi",
            "cand_massDau0", "cand_etaDau1", "cand_phiDau1", "cand_pTDau1",
            "trk_dau1_dEdxRes", "cand_angle3D", "cand_Ntrkoffline", "trigBit_2", "trigBit_4"},
            opts);
    std::cout << "Saved trees\n";
    std::cout << "Finished skimNTuple.C\n";
}
