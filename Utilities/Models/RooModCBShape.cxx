 /** \class RooModCBShape
     \ingroup Roofit
    
     P.d.f implementing the Modified Crystal Ball line shape
 **/

#include "Riostream.h"

#include "RooModCBShape.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooMath.h"

ClassImp(RooModCBShape);
    
////////////////////////////////////////////////////////////////////////////////
    
Double_t RooModCBShape::ApproxErf(Double_t arg) const
{
  static const double& erflim = 5.0;
  if( arg >  erflim ) return  1.0;
  if( arg < -erflim ) return -1.0;
  return RooMath::erf(arg);
}
    
////////////////////////////////////////////////////////////////////////////////
    
RooModCBShape::RooModCBShape(const char *name, const char *title,
                             RooAbsReal& _m, RooAbsReal& _m0, RooAbsReal& _sigma,
                             RooAbsReal& _alphaL) :
  RooAbsPdf(name, title),
  m("m", "Dependent", this, _m),
  m0("m0", "M0", this, _m0),
  sigma("sigma", "Sigma", this, _sigma),
  alphaL("alphaL", "AlphaL", this, _alphaL)
{
}
    
////////////////////////////////////////////////////////////////////////////////
    
RooModCBShape::RooModCBShape(const RooModCBShape& other, const char* name) :
  RooAbsPdf(other, name), m("m", this, other.m), m0("m0", this, other.m0),
  sigma("sigma", this, other.sigma), alphaL("alphaL", this, other.alphaL)
{
}
    
////////////////////////////////////////////////////////////////////////////////
    
Double_t RooModCBShape::evaluate() const {
  const double t = (m-m0)/sigma;
  const double absAlphaL = fabs((Double_t)alphaL);
  if (t >= -absAlphaL) { return exp(-0.5*t*t); }
  else { return exp(0.5*absAlphaL*absAlphaL + absAlphaL*t); }
  return 0.0;
}

////////////////////////////////////////////////////////////////////////////////

Int_t RooModCBShape::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const
{
  if (matchArgs(allVars, analVars, m)) { return 1; }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

Double_t RooModCBShape::analyticalIntegral(Int_t code, const char* rangeName) const
{
  static const double& sqrtPiOver2 = 1.2533141373;
  static const double& sqrt2 = 1.4142135624;
  
  R__ASSERT(code==1);
  double result = 0.0;
  
  const double sig = fabs((Double_t)sigma);
  const double absAlphaL = fabs((Double_t)alphaL);
  
  double tmin = (m.min(rangeName)-m0)/sig;
  double tmax = (m.max(rangeName)-m0)/sig;
  
  if (tmin >= -absAlphaL) {
    result += sig*sqrtPiOver2*( ApproxErf(tmax/sqrt2) - ApproxErf(tmin/sqrt2) );
  }
  else if (tmax < -absAlphaL) {
    result += (sig/absAlphaL)*exp(0.5*absAlphaL*absAlphaL)*( exp(absAlphaL*tmax) - exp(absAlphaL*tmin) );
  }
  else {
    const double& term1 = sig*sqrtPiOver2*( ApproxErf(tmax/sqrt2) - ApproxErf(-absAlphaL/sqrt2) );
    const double& term2 = (sig/absAlphaL)*exp(0.5*absAlphaL*absAlphaL)*( exp(-absAlphaL*absAlphaL) - exp(absAlphaL*tmin) );
    result += term1 + term2;
  }
    
  return result;
}

////////////////////////////////////////////////////////////////////////////////
/// Advertise that we know the maximum of self for given (m0,alpha,n,sigma)
    
Int_t RooModCBShape::getMaxVal(const RooArgSet& vars) const
{
  RooArgSet dummy;
  if (matchArgs(vars,dummy,m)) { return 1 ; }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
    
Double_t RooModCBShape::maxVal(Int_t code) const
{
  R__ASSERT(code==1);
  // The maximum value for given (m0,alpha,sigma)
  return 1.0 ;
}
