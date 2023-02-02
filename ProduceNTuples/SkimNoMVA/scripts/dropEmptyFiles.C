std::vector<std::string> getFileNames(const std::string& files)
{
    std::ifstream infile(files.c_str());

    std::vector<std::string> names;
    std::string temp;
    while (std::getline(infile, temp)) {
        if (temp.empty()) continue;
        TString n(temp.c_str());
        n = n.Strip(TString::kBoth);
        if (n[0] == '#') continue;
        names.push_back(temp);
    }
    return names;
}
void dropEmptyFiles(const std::string& files)
{
    TStopwatch sw;
    sw.Start();
    std::string base = gSystem->BaseName(files.c_str());
    if (!gSystem->OpenDirectory("good")) { gSystem->mkdir("good"); }
    if (!gSystem->OpenDirectory("bad")) { gSystem->mkdir("bad"); }
    std::ofstream fgood("good/" + base);
    std::ofstream fbad("bad/" + base);
    auto names = getFileNames(files);

    std::cout << "To process " << names.size() << " files\n";

    for (const auto& n : names) {
        TString nn(n.c_str());
        nn = nn.Strip(TString::kBoth);
        std::cout << "Checking " << nn << "\n";
        TFile* f = TFile::Open(nn.Data());
        // correct way for TFile::Open.
        if (!f) {
            fbad << nn << "\n";
            std::cout << "Bad file\n";
            continue;
        }
        // valid only TFile f("name.root");
        // if (!f.IsZombie()) { std::cout << "Bad file\n"; continue; }
        // do not use IsOpen. It is for checking whether the file is closed.
        auto t = f->Get<TTree>("lambdacAna/ParticleTree");
        if (t) {
            // std::cout << "Good tree\n";
            fgood << nn << "\n";

        } else {
            // std::cout << "Non-existing tree\n";
            fbad << nn << "\n";
        }
        delete f;
        // std::cout << "Finished\n";
    }
    sw.Stop();
    sw.Print();
}
