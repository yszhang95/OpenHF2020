#ifndef ROO_MODCB_SHAPE
#define ROO_MODCB_SHAPE


#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooAbsReal.h"
#include "RooArgSet.h"
  
class RooRealVar;
  
class RooModCBShape : public RooAbsPdf {
 public:
  RooModCBShape() {} ;
  RooModCBShape(const char *name, const char *title, RooAbsReal& _m,
                RooAbsReal& _m0, RooAbsReal& _sigma, RooAbsReal& _alphaL);
  
  RooModCBShape(const RooModCBShape& other, const char* name = 0);
  virtual TObject* clone(const char* newname) const { return new RooModCBShape(*this,newname); }
  
  inline virtual ~RooModCBShape() { }
  
  virtual Int_t getAnalyticalIntegral( RooArgSet& allVars,  RooArgSet& analVars, const char* rangeName=0 ) const;
  virtual Double_t analyticalIntegral( Int_t code, const char* rangeName=0 ) const;
  
  // Optimized accept/reject generator support
  virtual Int_t getMaxVal(const RooArgSet& vars) const ;
  virtual Double_t maxVal(Int_t code) const ;
  
 protected:
  
  Double_t ApproxErf(Double_t arg) const ;
  
  RooRealProxy m;
  RooRealProxy m0;
  RooRealProxy sigma;
  RooRealProxy alphaL;
  
  Double_t evaluate() const;
  
 private:

  ClassDef(RooModCBShape,1)
  
};
  
#endif
