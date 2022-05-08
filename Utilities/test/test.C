R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyTreeReader.so)
void test_newParticle()
{
  gROOT->SetBatch(kTRUE);
  std::string s = "HepMC::Version 3.0.0\n"
                  "HepMC::Asciiv3-START_EVENT_LISTING\n"
                  "W 0\n"
                  "E 0 2 5\n"
                  "U GEV MM\n"
                  "A 2 longLived 1\n"
                  "A 2 selfConj 1\n"
                  "P 1 0 4122 0.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 1.8650000000000000e+00 2\n"
                  "P 2 1 310 1.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 1.0000000000000000e+00 4.9500000000000000e-01 2\n"
                  "P 3 1 211 1.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 -1.0000000000000000e+00 1.4899999999999999e-01 1\n"
                  "P 4 2 -211 1.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 1.0000000000000000e+00 1.4899999999999999e-01 1\n"
                  "P 5 2 211 1.0000000000000000e+00 0.0000000000000000e+00 0.0000000000000000e+00 -1.0000000000000000e+00 1.4899999999999999e-01 1\n"
                  "HepMC::Asciiv3-END_EVENT_LISTING\n";
  std::cout << "\n" << s << "\n";
  
  auto p = Particle::newParticle(s);
  std::cout << "Particle " << p->id() << "\n"
    << "Particle isStable " << p->isStable() << "\n"
    << "Particle self conjugation " << p->selfConj() << "\n"
    << "Particle long lived " << p->longLived() << "\n\n";
  auto dau1 = p->daughter(0);
  std::cout << "Particle dau2 " << dau1->id() << "\n"
    << "Particle isStable " << dau1->isStable() << "\n"
    << "Particle self conjugation " << dau1->selfConj() << "\n"
    << "Particle long lived " << dau1->longLived() << "\n\n";
  auto dau2 = p->daughter(1);
  std::cout << "Particle dau2 " << dau2->id() << "\n"
    << "Particle isStable " << dau2->isStable() << "\n"
    << "Particle self conjugation " << dau2->selfConj() << "\n"
    << "Particle long lived " << dau2->longLived() << "\n\n";

  auto gdau1 = dau1->daughter(0);
  std::cout << "Particle dau2 " << gdau1->id() << "\n"
    << "Particle isStable " << gdau1->isStable() << "\n"
    << "Particle self conjugation " << gdau1->selfConj() << "\n"
    << "Particle long lived " << gdau1->longLived() << "\n\n";
  auto gdau2 = dau1->daughter(1);
  std::cout << "Particle dau2 " << gdau2->id() << "\n"
    << "Particle isStable " << gdau2->isStable() << "\n"
    << "Particle self conjugation " << gdau2->selfConj() << "\n"
    << "Particle long lived " << gdau2->longLived() << "\n\n";
}
