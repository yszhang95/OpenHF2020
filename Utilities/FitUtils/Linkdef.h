#if defined(__CLING__) || defined(__ROOTCLING__)
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class FitParConfigs+;
#pragma link C++ function fitSignal;
#pragma link C++ function fitSwap;
#pragma link C++ function fitSignalAndSwap;
#pragma link C++ function fitCBShapeKK;
#pragma link C++ function fitCBShapePiPi;
#pragma link C++ function fitD0;

#pragma link C++ class MassFitter+;
#pragma link C++ class D0Fitter+;

#pragma link C++ function string_format;
#pragma link C++ class VarCuts+;

#endif
