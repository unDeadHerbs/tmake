#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::vector;

vector<string> split(string str, string const dlim) {
  vector<string> ret;
  size_t pos;
  while ((pos = str.find(dlim)) != std::string::npos) {
    ret.push_back(str.substr(0, pos));
    str.erase(0, pos);
    str.erase(0, dlim.length());
  }
  ret.push_back(str);
  return ret;
}

bool starts_with(string const s, string const p) {
  if (p.size() > s.size())
    return false;
  for (size_t i = 0; i < p.size(); i++)
    if (p[i] != s[i])
      return false;
  return true;
}

struct TDig {
  string source, dest, native;
  vector<TDig> production;
  TDig(string t, vector<string> natives) {
    auto s = split(t, ".");
    if (s.size() == 1) {
      source = s[0];
      native = natives[0];
    } else if (s.size() == 2) {
      source = s[0];
      native = s[1];
    } else if (s.size() == 3) {
      source = s[0];
      dest = s[1];
      native = s[2];
    } else
      cout << "nuuuu" << endl;
  }
};

std::ostream &operator<<(std::ostream &o, TDig const &t) {
  if (t.dest.size())
    return o << t.source << "." << t.dest << "." << t.native;
  return o << t.source << "." << t.native;
}

int main(int argc, char **argv) {

  vector<string> args;
  for (int i = 1; i < argc; i++)
    args.push_back(argv[i]);

  vector<TDig> targets;
  vector<string> runnables;

  for (auto &arg : args)
    if (starts_with(arg, "-r=")) {
      arg.erase(0, 3);
      runnables = split(arg, ",");
    }
  if (!runnables.size())
    runnables = {"bin", "sh"};

  for (auto &arg : args)
    if (!starts_with(arg, "-r="))
      targets.push_back({arg, runnables});

  // debug tmp
  cout << "runnables: ";
  for (auto const &r : runnables)
    cout << r << ", ";
  cout << endl;

  cout << "targets: ";
  for (auto const &t : targets)
    cout << t << ", ";
  cout << endl;

  // get existings

  // find existing with correct (source, dest)

  // find transforms to get to target

  // find transforms for needed

  return 0;
}
