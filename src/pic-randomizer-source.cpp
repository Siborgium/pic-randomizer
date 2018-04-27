

#include<iostream>
#include<vector>
//You will need C++17
#include<filesystem>

//asm random
//whoever needs fancy <random>?
//no need for crypt-safe random
namespace os{
    template<typename T>
	struct random{ static const T get(){ T value; __asm__ volatile("rdrand %[value]":[value]"=r"(value)::"cc"); return value; }; };
	unsigned int rand(const unsigned int &first, const unsigned int &last){ return (last > RAND_MAX || first > RAND_MAX) ? 0 : random<unsigned int>::get() % (last - first + 1) + first; }
}

int main(int argc, char* argv[]){
    bool is_copy = true, is_write = true, is_delete = false;
    if(argc <= 1){
	std::cout << "Run this program with '--help' argument to get help\n";
    	std::exit(0);	
    }
    else{
        for(int i = 0; i < argc; i++){
            std::string tmp = argv[i]; 
            if(tmp== "--help"){
                std::cout
                        << " (O w O) \n"
                        << " <nice to see ya there> \n"
                        << "Enter path to get 1-10 randomly chosen files from that directory.\n"
                        << "Use following arguments if needed:\n"
                        << "    -nocopy to prevent this program from copying files\n"
                        << "    -nowrite to prevent this program from writing any text to stdout\n"
                        << "    -nodelete to prevent this program from deleting copied files\n"
                        << "That's all. Have a good day. Enter any character to exit.\n\n"; getchar(); return 0; }
            if(tmp == "-nocopy"){ is_copy = false; is_delete = false; }
            if(tmp == "-nowrite"){ is_write = false; }
            if(tmp == "-delete"){ is_delete = true; }
        }
        if(is_write) std::cout << "\n[randompicturefromfolder], Siborgium, 2018 02 02\n";
    }
    namespace fs = std::filesystem;
    std::string directory = argv[1];

    //counting files
    uint32_t count = 0;
    const auto count_iter = fs::directory_iterator(directory);
    for(auto ci : count_iter){ count++; }
    
    //filling vector with random generated numbers
    const auto iter = fs::directory_iterator(directory);
    std::vector<uint32_t> data(os::rand(1, count > 10 ? 10 : count));
    for(auto &d : data){ d = os::rand(1, count); }

    //resizing string, appending random generated name for new subdirectory
    //1 / 64^26 name collision chance is small enough to ignore
    directory.resize(directory.size() + 64u);
    for(auto iter = directory.end() - 64; iter < directory.end(); iter++){ *iter = os::rand('a', 'z'); }

    //parsing directory
    //if index of file exists in our vector, then copy this file (if needed) and write its name (if needed)
    size_t id = 0;
    if(is_copy) fs::create_directories(directory);
    for(auto itr : iter){ 
        if(std::find(data.begin(), data.end(), id) != data.end()){
            if(is_copy) fs::copy(itr.path(), directory); 
            if(is_write) std::cout << itr.path().string() << '\n';
        }
        ++id;
    }
    //delete dialogue and stuff
    if(is_write){
	if(is_copy){
		auto sz = data.size();
		std::cout << sz << " file" << (sz != 1 ? "s " : " ") << " have been copied" << '\n';
	}
        if(is_delete) std::cout << "Write 'y' to purge temp directory\n";
    }
    //ask for char, delete directory if confirmed, exit otherwise
    if(is_delete){
        char c = ' ';
        while(true){
	    std::cin >> c;
	    if(c == 'Y' || c == 'y'){
	    	fs::remove_all(directory); break;
	    }
	    if(c == 'Y' || c == 'y')
		    break;
	}
    }

    return 0;
}
