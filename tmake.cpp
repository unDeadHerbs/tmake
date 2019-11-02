/**
 * @file
 * @author  Murray Fordyce <Murray.Fordyce@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANT-ABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * A tool for deciding the build order of chained compilers and cross compilers.
 */

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

/**
 * Split a string by a delimiter.
 *
 * @param str The string to be split.
 * @param dlim The delimiter to split the string by.
 * @return A vector of the sub-strings.
 */
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

/**
 * Check if a string starts with a value.
 *
 * Temporary until
 * `std::basic_string::starts_with` is supported.
 *
 * @param str The string to check in.
 * @param pat The pattern to check against.
 * @return If the string starts with the pattern.
 */
bool starts_with(string const str, string const pat) {
	if (pat.size() > str.size()) return false;
	for (size_t i = 0; i < pat.size(); i++)
		if (pat[i] != str[i]) return false;
	return true;
}

/**
 * A class to represent a T Diagram.
 */
struct TDiagram {
	/**
	 * The type or format of the source material.
	 */
	string source;
	/**
	 * The type or format of the destination material.
	 */
	string destination;
	/**
	 * The type or format that this item is in.
	 */
	string native;
	/**
	 * The T Diagrams used to create this one (it they exist).
	 */
	vector<TDiagram> production;
	// TODO: make this an `optional<pair<>>` for a more accurate representations
	// of the idea..
	/**
	 * Parse a string of a T Diagram.
	 *
	 * This can fail.
	 *
	 * @param t The string to parse.
	 */
	TDiagram(string t) {
		auto s = split(t, ".");
		if (s.size() == 1) {
			source = s[0];
		} else if (s.size() == 2) {
			source = s[0];
			native = s[1];
		} else if (s.size() == 3) {
			source = s[0];
			destination = s[1];
			native = s[2];
		} else
			cout << "nuuuu" << endl;
		// TODO: Find a better way to throw.
	}
	/**
	 * Parse a string of a T Diagram. With optional default  type.
	 *
	 * @param t The string to be parsed.
	 * @param natives The set of types that are evaluable on the current system.
	 */
	TDiagram(string t, vector<string> natives) {
		auto s = split(t, ".");
		if (s.size() == 1) {
			source = s[0];
			native = natives[0];
		} else if (s.size() == 2) {
			source = s[0];
			native = s[1];
		} else if (s.size() == 3) {
			source = s[0];
			destination = s[1];
			native = s[2];
		} else
			cout << "nuuuu" << endl;
		// TODO: Find a better way to throw.
	}
	/**
	 * Construct a new T Diagram out of two previous ones.
	 *
	 * @param tool The T Diagram that is evaluating the other.
	 * @param from The T Diagram that is being evaluated.
	 */
	TDiagram(TDiagram tool, TDiagram from) {
		if (from.native != tool.source) {
			cout << "can't make that TDiagram" << endl;
			// TODO: Find a better way to throw.
		} else {
			source = from.source;
			destination = from.destination;
			native = tool.destination;
			production = {tool, from};
		}
	}
};

/**
 * Detect if two T Diagrams have the same effect.
 *
 * @param lhs The left TDiagram to compare.
 * @param rhs The right TDiagram to compare.
 * @return If they are the same.
 */
bool operator==(TDiagram const& lhs, TDiagram const& rhs) {
	// TODO: Add some from of "Difficulty to obtain" so that easier to
	// build diagrams are find-able.
	return lhs.source == rhs.source && lhs.destination == rhs.destination &&
	       lhs.native == rhs.native;
}

/**
 * Print out a T Diagram's file's name.
 *
 * @param o The stream to print to.
 * @param t The TDiagram to print.
 * @return The stream printed to.
 */
std::ostream& operator<<(std::ostream& o, TDiagram const& t) {
	if (t.destination.size())
		return o << t.source << "." << t.destination << "." << t.native;
	return o << t.source << "." << t.native;
}

/**
 * Print out a pair of T Diagrams for evaluating on the command line.
 *
 * @param o The stream to be printed into.
 * @param vt the pair of Diagrams.
 * @return The stream printed into.
 */
std::ostream& operator<<(std::ostream& o, vector<TDiagram> const& vt) {
	// TODO: Update this to a `pair` as well.
	for (auto& t : vt) o << t << " ";
	return o;
}

/**
 * Search if the target can be build from source with only the
 * existing tools.
 *
 * @param transforms OUT The set of transforms needed to build the
 *                       target.
 * @param tools The existing set of tools for building with.
 * @param targets The list of targets.
 * @return If the search was successfully.
 */
bool trivial(vector<TDiagram>& transforms, vector<TDiagram> const& tools,
             vector<TDiagram> const& targets) {
	size_t seen = 0;
	while (!std::all_of(targets.begin(), targets.end(), [&](TDiagram t) {
		return std::any_of(transforms.begin(), transforms.end(),
		                   [t](TDiagram r) { return r == t; });
	}) && transforms.size() != seen) {
		seen = transforms.size();
		for (auto& t : transforms)
			for (auto& b : tools)
				if (t.native == b.source) transforms.push_back({b, t});
	}
	return std::all_of(targets.begin(), targets.end(), [&](TDiagram t) {
		return std::any_of(transforms.begin(), transforms.end(),
		                   [t](TDiagram r) { return r == t; });
	});
}

/**
 * Search for if the target can be build by building new tools.
 *
 * Perform a (mostly) Breadth First Search through the combinations of T
 * Diagrams to find a path to building all of the targets.
 *
 * @param transforms OUT The set of transforms needed to build the
 *                       targets.
 * @param runnables The set of extensions that are executable on the
 *                       system.
 * @param targets The set of targets for the build.
 * @return If the search was successful.
 */
bool brute_force(vector<TDiagram>& transforms, vector<string> const& runnables,
                 vector<TDiagram> const& targets) {
	size_t seen = 0;
	while (!std::all_of(targets.begin(), targets.end(), [&](TDiagram t) {
		return std::any_of(transforms.begin(), transforms.end(),
		                   [t](TDiagram r) { return r == t; });
	}) && transforms.size() != seen) {
		seen = transforms.size();
	restart:  // The vector iterators are invalidated on mutation.
		for (auto& t : transforms)
			for (auto& b : transforms)
				if (std::any_of(runnables.begin(), runnables.end(),
				                [&](string r) { return b.native == r; }))
					if (t.native == b.source)
						if (!std::any_of(
						        transforms.begin(), transforms.end(),
						        [&](TDiagram t2) { return t2 == TDiagram(b, t); })) {
							transforms.push_back({b, t});
							goto restart;
						}
	}
	return std::all_of(targets.begin(), targets.end(), [&](TDiagram t) {
		return std::any_of(transforms.begin(), transforms.end(),
		                   [t](TDiagram r) { return r == t; });
	});
}

/**
 * Print usage information to the terminal.
 */
void usage(char const* program_name) {
	cout << "Usage: " << program_name << "e [results] [-r=runnable1,runnable2]"
	     << endl
	     << "Results in the form [from.to.in], [name.in], [name]" << endl
	     << "files of the type [name] are interpreted as [name.bin]" << endl
	     << "Runnable defaults to bin, sh" << endl
	     << endl
	     << "Example:" << endl
	     << "$ ls" << endl
	     << "null.hello.cpp cpp.bin.bin" << endl
	     << "$ " << program_name << " null.hello.bin" << endl
	     << "cpp.bin.bin null.hello.cpp" << endl
	     << "" << endl
	     << "Example:" << endl
	     << "$ ls" << endl
	     << "cpp.bin.bin bf.cpp.cpp null.hello.bf" << endl
	     << "$ " << program_name << " null.hello.bin" << endl
	     << "cpp.bin.bin bf.cpp.cpp" << endl
	     << "bf.cpp.bin null.hello.bf" << endl
	     << "cpp.bin.bin null.hello.cpp" << endl
	     << endl
	     << "Example:" << endl
	     << "$ ls" << endl
	     << "" << program_name << ".cpp cpp.bin.sh" << endl
	     << "$ " << program_name << " " << program_name << "" << endl
	     << "cpp.bin.sh " << program_name << ".cpp" << endl
	     << "" << endl
	     << "Specs:" << endl
	     << "Will accept folders as input. Will accept multiple runnables."
	     << endl;
}

/**
 * A function in desperate need of separation.
 */
int main(int argument_count, char** argument_values) {
	if (argument_count == 1) {
		usage(argument_values[0]);
		exit(0);
	}
	vector<string> arguments;
	for (int i = 1; i < argument_count; i++)
		arguments.push_back(argument_values[i]);

	vector<TDiagram> targets;
	vector<string> runnables;
	bool verbose = false;

	// Handel the command-line parameters.
	for (auto& argument : arguments)
		if (starts_with(argument, "-")) {
			if (starts_with(argument, "-r=")) {
				argument.erase(0, 3);
				runnables = split(argument, ",");
			}
			if (argument == "-v"s) {
				verbose = true;
			}
			if (argument == "-h"s) {
				usage(argument_values[0]);
				exit(0);
			} else {
				std::cerr << "Unknown option \"" << argument << "\"" << endl;
				exit(-1);
			}
		}
	if (!runnables.size()) runnables = {"bin", "sh"};

	for (auto& argument : arguments)
		if (!starts_with(argument, "-")) targets.push_back({argument, runnables});

	if (verbose) {
		cout << "# runnables: ";
		for (auto const& r : runnables) cout << r << ", ";
		cout << endl;
		cout << "# targets: ";
		for (auto const& t : targets) cout << t << ", ";
		cout << endl;
	}

	vector<TDiagram> sources, tools;
	// Find the existing manipulators.
	for (auto& p : fs::directory_iterator("."))
		if (!starts_with(p.path().filename(), ".")) {
			TDiagram t = {p.path().filename()};
			if (t.native != "" && *t.native.rbegin() != '~')
				if (std::find(runnables.begin(), runnables.end(), t.native) !=
				    runnables.end()) {
					if (t.destination != "") tools.push_back(t);
				} else
					sources.push_back(t);
		}

	if (verbose) {
		cout << "# sources: ";
		for (auto const& s : sources) cout << s << ", ";
		cout << endl;
		cout << "# bins: ";
		for (auto const& t : tools) cout << t << ", ";
		cout << endl;
	}

	// Find existing source with the correct (source, destination).
	// TODO: Move this into `bool trivial()`.
	vector<TDiagram> source;
	// TODO: Convert this loop to `std::views` in C++20.
	for (auto& s : sources)
		if (std::any_of(targets.begin(), targets.end(),
		                [&](TDiagram t) { return t.source == s.source; }))
			source.push_back(s);
	if (verbose) {
		cout << "# useful sources: ";
		for (auto& s : source) cout << s << ", ";
		cout << endl;
	}

	// Find transforms to get to target.
	// TODO: Wrap trivial and brute_force into a container function.
	vector<TDiagram> transforms = source;
	if (verbose) cout << "# Trying trivial search" << endl;
	bool success = trivial(transforms, tools, targets);

	if (success) {
		if (verbose) cout << "# Build Path:" << endl;
		for (auto& t : transforms) {
			if (!t.production.size()) continue;
			// ignore source files
			// TODO: convert to filter on transforms in c++20
			cout << "./" << t.production;
			if (verbose) cout << "# " << t;
			cout << endl;
		}
	} else {
		if (verbose) cout << "# Trivial search failed, brute force time" << endl;
		for (auto& t : tools) transforms.push_back(t);
		for (auto& s : sources) transforms.push_back(s);
		success = brute_force(transforms, runnables, targets);

		if (success) {
			// TODO: Move this trimming into `bool brute_force()`.
			if (verbose)
				cout << "# Finding Build Path:" << endl << "# Finding goals" << endl;
			vector<TDiagram> build_path_looking;
			for (auto& tf : transforms)
				if (std::any_of(targets.begin(), targets.end(),
				                [&](TDiagram t) { return t == tf; }) &&
				    !std::any_of(build_path_looking.begin(), build_path_looking.end(),
				                 [&](TDiagram t) { return t == tf; }))
					build_path_looking.push_back(tf);
			if (verbose) {
				cout << "# Found Goals:" << endl;
				for (auto& bp : build_path_looking)
					cout << "# " << bp.production << " -> " << bp << endl;
				cout << "# Finding paths" << endl;
			}
			vector<TDiagram> build_path;
			while (build_path_looking.size()) {
				auto bp = build_path_looking.front();
				if (!std::any_of(build_path.begin(), build_path.end(),
				                 [&](TDiagram bp2) { return bp2 == bp; }))
					build_path.push_back(bp);
				build_path_looking.erase(build_path_looking.begin());
				if (bp.production.size()) {
					if (!std::any_of(
					        build_path.begin(), build_path.end(),
					        [&](TDiagram bp2) { return bp2 == bp.production[0]; }))
						build_path_looking.push_back(bp.production[0]);
					if (!std::any_of(
					        build_path.begin(), build_path.end(),
					        [&](TDiagram bp2) { return bp2 == bp.production[1]; }))
						build_path_looking.push_back(bp.production[1]);
				}
			}
			std::reverse(build_path.begin(), build_path.end());
			if (verbose) cout << "# Finished building path" << endl;
			if (verbose) cout << "# Build Path:" << endl;
			for (auto& t : build_path) {
				if (!t.production.size()) continue;  // ignore source files
				cout << "./" << t.production;
				if (verbose) cout << "# " << t;
				cout << endl;
			}
			// TODO: Move this to apply to solvers forms.
			if (true /*delete temporaries*/) {
				for (auto& t : build_path) {
					if (!t.production.size()) continue;  // ignore source files
					if (!std::any_of(targets.begin(), targets.end(),
					                 [&](TDiagram tg) { return t == tg; }))
						cout << "rm " << t << endl;
				}
			}
		} else {
			if (verbose) cout << "# Brute Force failed" << endl;
			std::cerr << "No Build Path Found" << endl;
			exit(1);
		}
	}

	return 0;
}
