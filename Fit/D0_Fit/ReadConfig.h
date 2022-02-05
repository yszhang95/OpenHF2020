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
   Helper class, configuration in format of
   mean, 1.865, 1.7, 2.1
   sigma, 0.01, 0.001, 0.1
 */
class ParConfigs {
public:
  explicit ParConfigs(const char*);
  ParConfigs(const TString&);
  ParConfigs(const std::string&);
  bool hasInit(const std::string&) const;
  double getInit(const std::string&) const;
  double getMin(const std::string&) const;
  double getMax(const std::string&) const;
  // const  operator[] (unsigned int i) const;
  void setInit(const std::string n, const double _init);
  void setMin(const std::string n, const double _min);
  void setMax(const std::string n, const double _max);
private:
  void initialize(std::ifstream&);
  std::map<std::string,
           std::tuple<bool, double, double, double > > _data;

};

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

private:
  void initialize(std::ifstream&);
  std::map<std::string, std::vector<std::string > > _data;
  ParConfigs _parConfigs;
  InputConfigs _inputConfigs;
  OutputConfigs _outputConfigs;
};

#endif
