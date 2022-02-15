#include <fstream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "TString.h"

#ifndef __READ_CONFIG__
#define __READ_CONFIG__

/**
   Helper class (ParConfigs), configuration in format of
   mean, 1.865, 1.7, 2.1
   sigma, 0.01, 0.001, 0.1
 */

/**
   Helper class:
   1. strings for plotting
   2. path to output
   3. path to input
   4. strings for cut

   Make use perl regular expression:
   Each category is marked as:
   [BEG name]
   # comment
   ...
   [END name]
   A space/tab (or more) must be inserted between BEG/END and name.
   Name can only contain a-zA-Z_0-9 because regular expression \w
   is used. An example is:
   [BEG INPUT]
   # This is an example of input category
   # the format is sample : type : file : tree/hist name
   KK : /home/yousen/MCKK.root : D0NTuple
   [END INPUT]
   The regular expressions are:
   - \[[Bb][Ee][Gg]\w*?\s*([(\w)]{1,})\s*\]
   - \[[Ee][Nn][Dd]\s*([(\w)]{1,})\s*\]

 */

class FitParConfigs
{
public:
  class ParConfigs
  {
  public:
    friend class FitParConfigs;
    explicit ParConfigs() {}
    bool empty() const { return _data.empty(); }
    bool hasInit(const std::string&) const;
    double getInit(const std::string&) const;
    double getMin(const std::string&) const;
    double getMax(const std::string&) const;
    void setInit(const std::string n, const double _init);
    void setMin(const std::string n, const double _min);
    void setMax(const std::string n, const double _max);
  private:
    void initialize(const std::vector<std::string>&);
    std::map<std::string,
             std::tuple<bool, double, double, double > > _data;
  };

  class InputConfigs
  {
  public:
    friend class FitParConfigs;
    explicit InputConfigs() {}
    bool empty() const { return _paths.empty(); }
    std::vector<std::string> getPaths(const std::string&) const;
    std::string getType(const std::string&) const;
    std::string getName(const std::string&) const;
  private:
    void initialize(const std::vector<std::string>&);
    std::map<std::string, std::vector<std::string>> _paths;
    std::map<std::string, std::set<std::string>> _types;
    std::map<std::string, std::set<std::string>> _names;

  };

  class OutputConfigs
  {
  public:
    friend class FitParConfigs;
    explicit OutputConfigs() {}
    bool empty() const { return _paths.empty(); }
    std::string getPath(const std::string& category)const
    { return _paths.at(category); }
    std::string getName(const std::string& category) const
    { return _types.at(category); }
  private:
    void initialize(const std::vector<std::string>&);
    std::map<std::string, std::string> _paths;
    std::map<std::string, std::string> _types;
  };

  /**
     [BEG CutConfigs]
     MLP6to8MBNp2N
     pT, 0.0, 2.0
     [END CutConfigs]

     I only support two types of configurations:
     1. MVA name, min, max, MVA
     2. variable names, min, max, int/float

     If the min or max is not available, place INF.  INF is defined by
     std::numeric_limits<float>::infinity().  I make use of
     std::stof. This can be used to compare with general float numbers.
   */
  class CutConfigs
  {
  public:
    friend class FitParConfigs;
    explicit CutConfigs() {}
    bool empty() const
    {
      return _data_float_min.empty() && _data_int_min.empty()
        && _data_float_max.empty() && _data_float_max.empty();
    }
    std::string getMvaName() const { return _mvaName; }
    int getIntMin(std::string) const;
    int getIntMax(std::string) const;
    float getFloatMin(std::string) const;
    float getFloatMax(std::string) const;
    bool getBool(std::string) const;
  private:
    void initialize(const std::vector<std::string>&);
    std::string _mvaName;
    std::map<std::string, int> _data_int_min;
    std::map<std::string, int> _data_int_max;
    std::map<std::string, float> _data_float_min;
    std::map<std::string, float> _data_float_max;
    std::map<std::string, bool> _data_bool;
  };

  explicit FitParConfigs(const char*);
  explicit FitParConfigs(const TString&);
  explicit FitParConfigs(const std::string&);
  bool empty() const { return _data.empty(); };
  void dump();
  const FitParConfigs::ParConfigs& getParConfigs() const
  { return _parConfigs; }
  const FitParConfigs::InputConfigs& getInputConfigs() const
  { return _inputConfigs; }
  const FitParConfigs::OutputConfigs& getOutputConfigs() const
  { return _outputConfigs; }
  const FitParConfigs::CutConfigs& getCutConfigs() const
  { return _cutConfigs; }

private:
  void initialize(std::ifstream&);
  std::map<std::string, std::vector<std::string > > _data;
  ParConfigs _parConfigs;
  InputConfigs _inputConfigs;
  OutputConfigs _outputConfigs;
  CutConfigs _cutConfigs;
};

#endif
