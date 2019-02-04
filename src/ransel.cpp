
#include <iostream>
#include <charconv>
#include <string_view>
#include <string>
#include <random>
#include <filesystem>

enum class Flag : unsigned char { none, terse, verbose };
enum class Type : unsigned char { boolean, integral };

struct Param;

// utility
auto urand(unsigned int from, unsigned int to) -> unsigned int;
auto starts_with(std::string_view lhs, std::string_view rhs) -> bool;
auto parse(Param* params, std::size_t params_count,
	   char** argv, std::size_t argc,
	   std::string& dirname) -> void;


template<typename ... Ts>
[[noreturn]]
auto fail(Ts&& ... ts) -> void {
    ((std::cerr << ts), ...);
    std::exit(-1);
}

// callbacks 
void help_call(Param& dummy1, unsigned int dummy2);
void version_call(Param& dummy1, unsigned int dummy2);
void copy_call(Param& copy, unsigned int dummy2);
void list_call(Param& copy, unsigned int dummy2);
void count_call(Param& count, unsigned int val);
void strict_call(Param& strict, unsigned int dummy2);

struct Param {
    using callback_type = decltype(&help_call);
    std::string_view  alias;
    std::string_view  full;
    Type              type;
    unsigned int      defval;
    callback_type     callback_ptr;
};

int main(int argc, char* argv[]) {
    namespace fs = std::filesystem;

    if(argc <= 1) {
	std::cout << "ransel: run this program with '--help' argument to get help\n";
    	std::exit(0);
    }

    constexpr std::size_t params_count = 6;
    constexpr unsigned int dummy = 0;
    Param parameters[params_count] = { { "-c", "--copy", Type::boolean, dummy, &copy_call },
				       { "-l", "--list", Type::boolean, dummy, &list_call },
				       { "-C", "--count", Type::integral, 10, &count_call },
				       { "-s", "--strict", Type::boolean, dummy, &strict_call },
				       { "-h", "--help", Type::boolean, dummy, &help_call },
				       { "-v", "--version", Type::boolean, dummy, &version_call }};
    
    auto& copy = parameters[0].defval;
    auto& list = parameters[1].defval;
    auto& req_count = parameters[2].defval;
    auto& strict = parameters[3].defval;

    auto dirname_src = std::string{ "" };
    parse(parameters, params_count, argv, argc, dirname_src);

    if(!list && !copy) { // Nothing to do, therefore we can just quit
	std::cout << "ransel: there's nothing to do\n";
	std::exit(0);
    }
    
    if(req_count == 0) {
	std::cout << "ransel: requested count is zero, nothing to do here\n";
	std::exit(0);
    }
    
    if(dirname_src.empty()) fail("ransel: no directory specified\n");

    // obtain canonical path
    auto dir = fs::canonical(dirname_src);
    if(!fs::exists(dir)) fail("ransel: source directory \'", dir.string(), "\' does not exist\n");
    
    if(!fs::is_directory(dir)) fail("ransel: \'", dir.string(), "\' is not a directory\n");

    // counting files
    auto iters = std::vector<fs::directory_entry>(); // optimize here if possible
    {   // no scope pollution
	auto count_iter = fs::directory_iterator(dir);
	for(auto& ci : count_iter){ if(fs::is_regular_file(ci)) { iters.push_back(ci); } }
    }
    
    const auto file_count = iters.size();
    
    if(file_count == 0) { // todo: check if there is more clean solution
	fail("ransel: source directory is empty\n");
    }

    unsigned int count = 0;
    if(auto lt = file_count < req_count; strict && lt) { // set count or fail
	fail("ransel: the requested number of files exceeds the number of present files [--strict]\n");
    } else {
	count = lt ? file_count : req_count;
    }

    //filling vector with random generated numbers
    // todo: force indices to be unique
    auto indices = std::vector<unsigned int>{};
    indices.resize(count);
    if(file_count == 1) {
	indices[0] = 0; // we have only one file, no reason to run rng
    } else {
	for(auto& index : indices) {
	    index = urand(1, file_count - 1);
	}
    }
    
    auto destination = fs::path{};
    if(copy) { // why do anything if copying is not required?
	//resizing string, appending random generated name for new subdirectory
	//1 / 32^26 name collision chance is small enough to ignore
	do {
	    auto destination_src = dir.string();
	    destination_src.resize(destination_src.size() + 33u);
	    *(destination_src.end() - 33) = fs::path::preferred_separator;
	    for(auto iter = destination_src.end() - 32; iter < destination_src.end() - 1; iter++) {
		*iter = urand(static_cast<unsigned char>('a'),
			      static_cast<unsigned char>('z')); 
	    }
	    destination_src.back() = fs::path::preferred_separator;
	    destination = fs::path{ std::move(destination_src) };
	} while(fs::exists(destination));
    }

    // processing obtained data
    if(copy) { // create directory if needed
	auto ec = std::error_code{};
	if(auto succ = fs::create_directory(destination, ec); !succ) {
	    fail("ransel: failed to create directory \'", destination, "\'\n");
	}
    }
    for(auto& index : indices) {
	auto& iter = iters.at(index);
	if(copy) fs::copy(iter.path(), destination);
	if(list) std::cout << iter.path().string() << '\n';
    }
    return 0;
} // main

auto starts_with(std::string_view lhs, std::string_view rhs) -> bool {
    auto a = lhs.begin(), b = rhs.begin();
    while(b < rhs.end()) {
	if(*a != *b || a == lhs.end()) return false;
	a++; b++;
    }
    return true;
}

auto urand(unsigned int from, unsigned int to) -> unsigned int {
    static std::random_device rd;
    static std::default_random_engine gen{ rd() };
    std::uniform_int_distribution<unsigned int> dist(from, to);
    return dist(gen);
}

auto parse(Param* params, std::size_t params_count,
	   char** argv, std::size_t argc,
	   std::string& dirname) -> void {
    namespace fs = std::filesystem;
    
    for(std::size_t i = 1; i < argc; i++) {
	auto argument = std::string_view{ argv[i] };
	auto kind_of = starts_with(argument, "--") ? Flag::verbose : starts_with(argument, "-")
	                                           ? Flag::terse : Flag::none;

	if(kind_of == Flag::none) {
	    auto dir = fs::path(argument);
	    if(fs::exists(dir)) {
		if(fs::is_directory(dir)) {
		    dirname = dir.string();
		} else {
		    fail("ransel: \'", argument, "\' is not a directory\n");
		}
	    } else {
		fail("ransel: directory \'", argument, "\' does not exist\n");
	    }
	} else {
	    for(std::size_t j = 0; j < params_count; j++) {
		auto& param = params[j];
		auto& [alias, fullname, type, defvalue, callback] = param;
	    
		bool is_alias = argument == alias;
		bool is_fullname = starts_with(argument, fullname);
		bool starts_with_fullname = starts_with(argument, fullname);
		if(is_alias || starts_with_fullname) { // if we found a match
                    if(type == Type::boolean) {
                        if(is_alias || argument == fullname) {
                            callback(param, defvalue); // set flag
                        } else {
                            fail("ransel: unrecognized flag \'", argument, "'\n");
                        }
		    } else {                       
			switch(kind_of) {
			case Flag::terse: {
			    if(i + 1 < argc) {
				// argument is an alias, therefore the next argument
				// contains value for this one
				auto next = std::string_view{ argv[i + 1] };
				auto value = 0u;
				if(auto [p, e] = std::from_chars(next.data(), next.data() + next.size(), value); e != std::errc()) {
				    fail("ransel: failed to decode value of \'", next, "\' for flag \'", argument, "\'\n");
				} else {
				    callback(param, value);
				}
			    } else { // argument is an alias and the last in the list
				// it's value isn't settable and was not provided
				// therefore it's an error
				fail("ransel: no value provided for flag \'", argument, "\'\n");
			    }
			    i++;
			} break;
		    
			case Flag::verbose: {
			    if(auto iter = argument.find('='); iter != argument.npos) {
				if(auto name = argument.substr(0, iter); name == fullname) {
				    auto source = argument.substr(iter + 1, argument.npos);
				    unsigned int value = 0u;
				    if(auto [p, e] = std::from_chars(source.data(), source.data() + source.size(), value); e == std::errc()) {
					callback(param, value);
				    } else {
					fail("ransel: failed to decode value of \'", source, "\' for flag \'", name, "\'\n");
				    }
				} else {
				    fail("ransel: unrecognized flag \'", name, "'\n");
				}
			    } else {
				fail("ransel: ill-formatted flag \'", argument, "\'\n");
			    }
			} break;

			case Flag::none: break; //unreachable
			} // switch
		    } // else branch (if type is not boolean)
		} // if match found
	    } // for-loop cycling through params
	} // else branch (if type is not none)
    } // for-loop cycling through args
} // parse
					 

void copy_call(Param& copy, unsigned int dummy2) {
    copy.defval = 1;
}

void list_call(Param& list, unsigned int dummy2) {
    list.defval = 1;
}

void count_call(Param& count, unsigned int val) {
    count.defval = val;
}

void strict_call(Param& strict, unsigned int dummy2) {
    strict.defval = 1;
}

const char* const version_number = "0.7.1";
const char* const author = "Siborgium (Sergey Smirnykh)";
const char* const license = "MIT License";

[[noreturn]]
void version_call(Param& dummy1, unsigned int dummy2) {
    std::cout << "ransel: " << version_number
	      << "\n    This software is distributed under " << license
	      << "\n    It was written by " << author << ", 2019\n";
    std::exit(0);
}

const char* const help_message = R"(Usage: ransel [OPTIONS] DIRECTORY
Select random files from DIRECTORY.
Example: ransel --count=15 --list example/

Options:
  -h  --help     Display this message and quit
  -v  --version  Display version and quit
  -l  --list     List all selected files to stdout
  -c  --copy     Copy selected files to the directory
                  Directory name is 32-characters long random character sequence
  -C  --count    Count of files to select
                  Set to 10 by default
  -s  --strict   Exit if the requested number of files 
                  exceeds the number of existing files)";

[[noreturn]]
void help_call(Param& dummy1, unsigned int dummy2) {
    std::cout << help_message << '\n';
    std::exit(0);
}
