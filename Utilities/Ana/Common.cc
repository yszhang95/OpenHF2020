#ifndef HelpClass_H
#include "Utilities/Ana/Common.h"
#include <cmath>
#include <algorithm>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#endif
#ifdef HelpClass_H
Particle::Particle(const Particle& p):
  _id(p._id), _selfConj(p._selfConj), _longLived(p._longLived),
  _flip(0), _treeIdx(USHRT_MAX)
{
  for (const auto& d : p._daus) {
    _daus.push_back(std::make_shared<Particle>(*d));
  }
}

Particle::ParticlePtr Particle::daughter(size_t i)
{
  if (_daus.size()) {
    return _daus[i];
  }
  else {
    std::cerr << "No daughter found with pdgId " << _id << std::endl;
    return nullptr;
  }
}

Particle::ParticlePtr Particle::daughter(size_t i) const
{
  if (_daus.size()) {
    return _daus.at(i);
  }
  else {
    std::cerr << "No daughter found with pdgId " << _id << std::endl;
    return nullptr;
  }
}

void Particle::flipFlavor()
{
  if(!_selfConj) {
    _id = -_id;
    for(auto& d : _daus) d->flipFlavor();
  }
  _flip++;
}

bool passKinematic (const float pT, const float eta, const float y,
                    const float mass, const KineCut& cut)
{
  if (pT < cut.pTMin || pT > cut.pTMax
      || std::abs(eta) < cut.absEtaMin
      || std::abs(eta) > cut.absEtaMax
      || std::abs(y) < cut.absYMin
      || std::abs(y) > cut.absYMax
      || std::abs(mass-cut.massPDG) < cut.absMassLw
      || std::abs(mass-cut.massPDG) > cut.absMassUp
      ) return false;
  return true;
}

DeDxSelection::DeDxSelection(const float turn1, const float turn2,
                       const float lower, const float upper,
                       std::initializer_list<float> in):
  _turn1(turn1), _turn2(turn2), _lower(lower), _upper(upper)
{
  _pars.assign(in.begin(), in.end());
}

DeDxSelection::DeDxSelection(const float turn1, const float turn2,
                       const float lower, const float upper):
  DeDxSelection(turn1, turn2, lower, upper,
  {3.27790e+01, -2.27040e+00, 5.71871e-01, 2.29089e+01, -2.48301e+00}) {}

DeDxSelection::DeDxSelection(const float turn1, const float turn2):
  DeDxSelection(turn1, turn2, 0.7, 1.3) {}

DeDxSelection::DeDxSelection():
  DeDxSelection(0.7, 200) {}

bool DeDxSelection::operator() (const float p, const float dedx)
{
  if (p<_turn1) {
    const float mean = (_pars[0] * exp(_pars[1]*p)+_pars[2]);
    bool pass = dedx < _upper * mean && dedx > _lower * mean;
    return pass;
  }
  else if (p<_turn2) {
    const float mean =
      _pars[3]*exp(_pars[4]*p) + _pars[0]*exp(_pars[1]*_turn1)
      + _pars[2] - _pars[3]*exp(_pars[4]*_turn1);
    bool pass = dedx < _upper * mean && dedx > _lower * mean;
    return pass;
  }
  return true;
}

double DeDxSelection::getMean(const float p)
{
  if (p<_turn1) {
    return _pars[0] * exp(_pars[1]*p)+_pars[2];
  }
  else {
    return
      _pars[3]*exp(_pars[4]*p) + _pars[0]*exp(_pars[1]*_turn1)
      + _pars[2] - _pars[3]*exp(_pars[4]*_turn1);
  }
  return 0;
}

EfficiencyTable<TGraph>::EfficiencyTable(TGraph* g) : n(0)
{
  if (auto p = dynamic_cast<TGraphAsymmErrors*>(g)) {
    std::cout << "The input type of efficiency table"
      " is TGraphasymmErrors" << std::endl;
    n = p->GetN();
    std::copy(p->GetX(), p->GetX()+n, back_inserter(x));
    std::copy(p->GetEXhigh(), p->GetEXhigh()+n,
              back_inserter(xErrHigh));
    std::copy(p->GetEXlow(), p->GetEXlow()+n,
              back_inserter(xErrLow));
    std::copy(p->GetY(), p->GetY()+n, back_inserter(y));
    std::copy(p->GetEYhigh(), p->GetEYhigh()+n,
              back_inserter(yErrHigh));
    std::copy(p->GetEYlow(), p->GetEYlow()+n,
              back_inserter(yErrLow));

  }
  else if (auto p = dynamic_cast<TGraphErrors*>(g)) {
    std::cout << "The input type of efficiency table"
      " is TGraphErrors" << std::endl;
    n = p->GetN();
    std::copy(p->GetX(), p->GetX()+n, back_inserter(x));
    std::copy(p->GetEX(), p->GetEX()+n,
              back_inserter(xErrHigh));
    xErrLow = xErrHigh;
    std::copy(p->GetY(), p->GetY()+n, back_inserter(y));
    std::copy(p->GetEY(), p->GetEY()+n,
              back_inserter(yErrHigh));
    yErrLow = yErrHigh;
  }
}

double EfficiencyTable<TGraph>::getEfficiency(double _x)
{
  for (int i=0; i != n; ++i) {
    const auto upper = x.at(i) + xErrHigh.at(i);
    const auto lower = x.at(i) - xErrLow.at(i);
    if (_x < upper && _x >= lower)
      return y.at(i);
  }
  throw
    std::runtime_error("Cannot find the input in efficiency table");
  return 0;
}

double EfficiencyTable<TGraph>::getEfficiency(int _x)
{
  for (int i=0; i != n; ++i) {
    // [ ) for integers
    const auto upper = x.at(i) + xErrHigh.at(i) - 0.5;
    const auto lower = x.at(i) - xErrLow.at(i) - 0.5;
    if (_x < upper && _x >= lower)
      return y.at(i);
  }
  throw
    std::runtime_error("Cannot find the input in efficiency table");
  return 0;
}

double EfficiencyTable<TGraph>::getWeight(double _x)
{
  const auto eff = getEfficiency(_x);
  if (eff > 1E-6) return 1./eff;
  return 0;
}

double EfficiencyTable<TGraph>::getWeight(int _x)
{
  const auto eff = getEfficiency(_x);
  if (eff > 1E-6) return 1./eff;
  return 0;
}

#endif
