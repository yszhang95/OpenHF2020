#include <fstream>

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooCurve.h"
#include "RooFitResult.h"
#include "TCanvas.h"

#include "Utilities/FitUtils/ReadConfig.h"

#ifndef __FIT_UTILS__
#define __FIT_UTILS__

/**
   General descriptions for the coding:
   All will require at least three parameters
   1. RooRealVar mass
   2. Dataset, either binned or unbinned
   3. parameter set

   Several parts are required:
   - Model setup. Parameters are
   initialized using attributes in parameter setup.
   - Fit. Try to fix parameters and repeat and tune the fit with
   parameters updated.
   - Draw the plot.
   - Update parameter set. Update the input parameter set with the
   new parameters after fit.
*/


/**
   A workaround for VLine bug.

   Please check the post https://root-forum.cern.ch/t/unexpected-behavior-of-drawoption-f/44851
   and the PR https://github.com/root-project/root/pull/8198

   The function assume the x for points in RooCurve are in increasing
   order. And then, two more points will be placed at the largest x
   and then smallest x. These points then will construct a polygon
   in the filling area.
 */

void setVLines(RooCurve* curve);

void setCanvas(TCanvas& c);

void setKinematics(TVirtualPad* pad,
                   std::map<std::string, std::string> strs={},
                   const double x=0.25, const double y=0.7);

void setCMS(TVirtualPad* pad);

void setCollision(TVirtualPad* pad);

void setCollisionAndLumi(TVirtualPad* pad);

void fitGausCB(RooRealVar& mass, RooAbsData& ds,
               FitParConfigs::ParConfigs& par,
               std::map<std::string, std::string> strs);

void fitSignal(RooRealVar& mass, RooAbsData& ds,
               FitParConfigs::ParConfigs& par,
               std::map<std::string, std::string> strs);

void fitSwap(RooRealVar& mass, RooAbsData& ds,
             FitParConfigs::ParConfigs& par,
             std::map<std::string, std::string> strs);

void fitSignalAndSwap(RooRealVar& mass, RooAbsData& ds,
                      FitParConfigs::ParConfigs& par,
                      std::map<std::string, std::string> strs);


void fitCBShapeKK(RooRealVar& mass, RooAbsData& ds,
                  FitParConfigs::ParConfigs& par,
                  std::map<std::string, std::string> strs);

void fitCBShapePiPi(RooRealVar& mass, RooAbsData& ds,
                    FitParConfigs::ParConfigs& par,
                    std::map<std::string, std::string> strs);

RooFitResult fitD0(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
           FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
           std::map<std::string, std::string> strs);

RooFitResult fitLamC(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs);

RooFitResult fitLamCGausCB(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs);

RooFitResult fitSideband(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs);

class MassFitter
{
public:
  explicit MassFitter(const FitParConfigs::ParConfigs& par): _par(par) {}
  RooFitResult fitDoubGaus(RooRealVar& mass, RooAbsData& ds,
                           std::map<std::string, std::string> strs);
protected:
  FitParConfigs::ParConfigs _par;

};

class D0Fitter : MassFitter
{
public:
  explicit D0Fitter(const FitParConfigs::ParConfigs& par): MassFitter(par) {}
  RooFitResult fitD0(RooRealVar& mass, RooWorkspace& myws,
                       const FitOptsHF& fitOpts, std::map<std::string, std::string> strs);
  RooFitResult fitData(RooRealVar& mass, RooAbsData& ds, RooWorkspace& myws,
                       const FitOptsHF& fitOpts, std::map<std::string, std::string> strs);

  RooFitResult fitSwap(RooRealVar& mass, RooAbsData& ds, std::map<std::string, std::string> strs);
  RooFitResult fitDoubGausAndSwap(RooRealVar& mass, RooAbsData& ds, std::map<std::string, std::string> strs);
  RooFitResult fitCBShape(RooRealVar& mass, RooAbsData& ds, std::map<std::string, std::string> strs);
};
#endif
