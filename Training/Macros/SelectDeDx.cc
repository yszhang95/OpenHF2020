#include <algorithm>
#include <initializer_list>
struct SelectDeDx
{
  public:
    SelectDeDx(const float turn1_, const float turn2_, std::initializer_list<float> in_):
      turn1(turn1_), turn2(turn2_)
  {
    upper = 1.3;
    lower = 0.7;
    std::copy(in_.begin(), in_.end(), pars);
  }
    SelectDeDx(const float turn1_, const float turn2_):
      SelectDeDx(turn1_, turn2_,
          {3.27790e+01, -2.27040e+00, 5.71871e-01, 2.29089e+01, -2.48301e+00}) {}
    SelectDeDx():
      SelectDeDx(0.7, 2.2) {}
    bool operator() (const float p, const float dedx)
    {
      if (p<turn1) {
        const float mean = (pars[0]*exp(pars[1]*p)+pars[2]);
        bool pass = dedx < upper * mean && dedx > lower * mean;
        return pass;
      }
      else if (p<turn2) {
        const float mean =
          pars[3]*exp(pars[4]*p)+pars[0]*exp(pars[1]*turn1)+pars[2]-pars[3]*exp(pars[4]*turn1);
        bool pass = dedx < upper * mean && dedx > lower * mean;
        return pass;
      }
      return true;
    }
  private:
    float turn1;
    float turn2;
    float upper;
    float lower;
    float pars[5];
};

SelectDeDx selectDeDx;

bool passDeDx (const float p, const float dedx)
{
  const float turn1 = 0.7;
  const float turn2 = 2.2;
  const float pars[] = {3.27790e+01, -2.27040e+00, 5.71871e-01, 2.29089e+01, -2.48301e+00};
  if (p<turn1) {
    const float mean = (pars[0]*exp(pars[1]*p)+pars[2]);
    bool pass = dedx < 1.3 * mean && dedx > 0.7 * mean;
    return pass;
  }
  else if (p<turn2) {
    const float mean =
      pars[3]*exp(pars[4]*p)+pars[0]*exp(pars[1]*turn1)+pars[2]-pars[3]*exp(pars[4]*turn1);
    bool pass = dedx < 1.3 * mean && dedx > 0.7 * mean;
    return pass;
  }
  return true;
}
