#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
namespace fs = std::filesystem;
using std::cout;
using std::endl;
using std::string;
using std::string_literals::operator""s;
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
  TDig(string t) {
    auto s = split(t, ".");
    if (s.size() == 1) {
      source = s[0];
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
  TDig(TDig tool, TDig from) {
    if (from.native != tool.source) {
      cout << "can't make that TDig" << endl;
    } else {
      source = from.source;
      dest = from.dest;
      native = tool.dest;
      production = {tool, from};
    }
  }
};

bool operator==(TDig const &lhs, TDig const &rhs) {
  return lhs.source == rhs.source && lhs.dest == rhs.dest &&
         lhs.native == rhs.native;
}

std::ostream &operator<<(std::ostream &o, TDig const &t) {
  if (t.dest.size())
    return o << t.source << "." << t.dest << "." << t.native;
  return o << t.source << "." << t.native;
}

std::ostream &operator<<(std::ostream &o, vector<TDig> const &vt) {
  for (auto &t : vt)
    o << t << " ";
  return o;
}

bool trivial(vector<TDig> &transforms, vector<TDig> const &tools,
             vector<TDig> const &targets) {
  size_t seen = 0;
  while (!std::all_of(targets.begin(), targets.end(), [&](TDig t) {
    return std::any_of(transforms.begin(), transforms.end(),
                       [t](TDig r) { return r == t; });
  }) && transforms.size() != seen) {
    seen = transforms.size();
    for (auto &t : transforms)
      for (auto &b : tools)
        if (t.native == b.source)
          transforms.push_back({b, t});
  }
  return std::all_of(targets.begin(), targets.end(), [&](TDig t) {
    return std::any_of(transforms.begin(), transforms.end(),
                       [t](TDig r) { return r == t; });
  });
}

bool brute_force(vector<TDig> &transforms, vector<string> const &runnables,
                 vector<TDig> const &targets) {
  size_t seen = 0;
  while (!std::all_of(targets.begin(), targets.end(), [&](TDig t) {
    return std::any_of(transforms.begin(), transforms.end(),
                       [t](TDig r) { return r == t; });
  }) && transforms.size() != seen) {
    seen = transforms.size();
  brk:
    for (auto &t : transforms)
      for (auto &b : transforms)
        if (std::any_of(runnables.begin(), runnables.end(),
                        [&](string r) { return b.native == r; }))
          if (t.native == b.source)
            if (!std::any_of(transforms.begin(), transforms.end(),
                             [&](TDig t2) { return t2 == TDig(b, t); })) {
              transforms.push_back({b, t});
              goto brk;
            }
  }
  return std::all_of(targets.begin(), targets.end(), [&](TDig t) {
    return std::any_of(transforms.begin(), transforms.end(),
                       [t](TDig r) { return r == t; });
  });
}

void usage(char const *argv0) {
  cout << "Usage: " << argv0 << "e [results] [-r=runnable1,runnable2]" << endl
       << "Results in the form [from.to.in], [name.in], [name]" << endl
       << "files of the type [name] are interpreted as [name.bin]" << endl
       << "Runnable defaults to bin, sh" << endl
       << endl
       << "Example:" << endl
       << "$ ls" << endl
       << "null.hello.cpp cpp.bin.bin" << endl
       << "$ tmake null.hello.bin" << endl
       << "cpp.bin.bin null.hello.cpp" << endl
       << "" << endl
       << "Example:" << endl
       << "$ ls" << endl
       << "cpp.bin.bin bf.cpp.cpp null.hello.bf" << endl
       << "$ tmake null.hello.bin" << endl
       << "cpp.bin.bin bf.cpp.cpp" << endl
       << "bf.cpp.bin null.hello.bf" << endl
       << "cpp.bin.bin null.hello.cpp" << endl
       << endl
       << "Example:" << endl
       << "$ ls" << endl
       << "tmake.cpp cpp.bin.sh" << endl
       << "$ tmake tmake" << endl
       << "cpp.bin.sh tmake.cpp" << endl
       << "" << endl
       << "Specs:" << endl
       << "Will accept folders as input. Will accept multiple runnables."
       << endl
       << endl
       << "Special rule for \"make a.b.runnable\"." << endl
       << "Special type \"null\" means \"no input\" or \"no output" << endl;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    usage(argv[0]);
    exit(0);
  }
  vector<string> args;
  for (int i = 1; i < argc; i++)
    args.push_back(argv[i]);

  vector<TDig> targets;
  vector<string> runnables;
  bool verbose = false;

  for (auto &arg : args)
    if (starts_with(arg, "-")) {
      if (starts_with(arg, "-r=")) {
        arg.erase(0, 3);
        runnables = split(arg, ",");
      }
      if (arg == "-v"s) {
        verbose = true;
      }
      if (arg == "-h"s) {
        usage(argv[0]);
        exit(0);
      } else {
        std::cerr << "Unknown option \"" << arg << "\"" << endl;
        exit(-1);
      }
    }
  if (!runnables.size())
    runnables = {"bin", "sh"};

  for (auto &arg : args)
    if (!starts_with(arg, "-"))
      targets.push_back({arg, runnables});

  if (verbose) {
    cout << "# runnables: ";
    for (auto const &r : runnables)
      cout << r << ", ";
    cout << endl;
    cout << "# targets: ";
    for (auto const &t : targets)
      cout << t << ", ";
    cout << endl;
  }

  vector<TDig> sources, tools;
  // get existings
  for (auto &p : fs::directory_iterator("."))
    if (!starts_with(p.path().filename(), ".")) {
      TDig t = {p.path().filename()};
      if (t.native != "" && *t.native.rbegin() != '~')
        if (std::find(runnables.begin(), runnables.end(), t.native) !=
            runnables.end()) {
          if (t.dest != "")
            tools.push_back(t);
        } else
          sources.push_back(t);
    }
  if (verbose) {
    cout << "# sources: ";
    for (auto const &s : sources)
      cout << s << ", ";
    cout << endl;
    cout << "# bins: ";
    for (auto const &t : tools)
      cout << t << ", ";
    cout << endl;
  }

  // find existing with correct (source, dest)
  vector<TDig> source; // convert to std::views when that exists
  for (auto &s : sources)
    if (std::any_of(targets.begin(), targets.end(),
                    [&](TDig t) { return t.source == s.source; }))
      source.push_back(s);
  if (verbose) {
    cout << "# useful sources: ";
    for (auto &s : source)
      cout << s << ", ";
    cout << endl;
  }

  // find transforms to get to target
  vector<TDig> transforms = source;
  if (verbose)
    cout << "# Trying trival search" << endl;
  bool success = trivial(transforms, tools, targets);

  if (success) {
    if (verbose)
      cout << "# Build Path:" << endl;
    for (auto &t : transforms) {
      if (!t.production.size())
        continue; // ignore source files
      cout << "./" << t.production;
      if (verbose)
        cout << "# " << t;
      cout << endl;
    }
  } else {
    if (verbose)
      cout << "# Trival search failed, brute force time" << endl;
    for (auto &t : tools)
      transforms.push_back(t);
    for (auto &s : sources)
      transforms.push_back(s);
    success = brute_force(transforms, runnables, targets);

    if (success) {
      if (verbose)
        cout << "# Finding Build Path:" << endl << "# Finding goals" << endl;
      vector<TDig> build_path_looking;
      for (auto &tf : transforms)
        if (std::any_of(targets.begin(), targets.end(),
                        [&](TDig t) { return t == tf; }) &&
            !std::any_of(build_path_looking.begin(), build_path_looking.end(),
                         [&](TDig t) { return t == tf; }))
          build_path_looking.push_back(tf);
      if (verbose) {
        cout << "# Found Goals:" << endl;
        for (auto &bp : build_path_looking)
          cout << "# " << bp.production << " -> " << bp << endl;
        cout << "# Finding paths" << endl;
      }
      vector<TDig> build_path;
      while (build_path_looking.size()) {
        auto bp = build_path_looking.front();
        if (!std::any_of(build_path.begin(), build_path.end(),
                         [&](TDig bp2) { return bp2 == bp; }))
          build_path.push_back(bp);
        build_path_looking.erase(build_path_looking.begin());
        if (bp.production.size()) {
          if (!std::any_of(build_path.begin(), build_path.end(),
                           [&](TDig bp2) { return bp2 == bp.production[0]; }))
            build_path_looking.push_back(bp.production[0]);
          if (!std::any_of(build_path.begin(), build_path.end(),
                           [&](TDig bp2) { return bp2 == bp.production[1]; }))
            build_path_looking.push_back(bp.production[1]);
        }
      }
      std::reverse(build_path.begin(), build_path.end());
      if (verbose)
        cout << "# Finished building path" << endl;
      if (verbose)
        cout << "# Build Path:" << endl;
      for (auto &t : build_path) {
        if (!t.production.size())
          continue; // ignore source files
        cout << "./" << t.production;
        if (verbose)
          cout << "# " << t;
        cout << endl;
      }
      if (true /*delete temporaries*/) {
        for (auto &t : build_path) {
          if (!t.production.size())
            continue; // ignore source files
          if (!std::any_of(targets.begin(), targets.end(),
                           [&](TDig tg) { return t == tg; }))
            cout << "rm " << t << endl;
        }
      }
    } else {
      if (verbose)
        cout << "# Brute Force failed" << endl;
      std::cerr << "No Build Path Found" << endl;
      exit(1);
    }
  }

  return 0;
}
