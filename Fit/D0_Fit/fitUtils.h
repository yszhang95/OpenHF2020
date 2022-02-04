#include <fstream>

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "ReadConfig.h"
#include "RooCurve.h"
#include "TCanvas.h"


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

void setCMS(TVirtualPad* pad);

void setCollision(TVirtualPad* pad);

void setCollisionAndLumi(TVirtualPad* pad);

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

void fitD0(RooRealVar& mass, RooAbsData& ds,
           FitParConfigs::ParConfigs& par,
           std::map<std::string, std::string> strs);

#endif
