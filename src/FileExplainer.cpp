#include "FileExplainer.h"
#include "GitAnalyzer.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <system_error>
void FileExplainer::explain(const fs::path&p){std::error_code e;if(!fs::exists(p,e))throw std::runtime_error("Path does not exist: "+p.string());auto a=fs::absolute(p);std::cout<<"File: "<<a.string()<<"\n";std::cout<<"Type: "<<(fs::is_directory(a,e)?"Directory":(fs::is_symlink(a,e)?"Symbolic Link":"File"))<<"\n";if(fs::is_regular_file(a,e))std::cout<<"Size: "<<fs::file_size(a,e)<<" bytes\n";std::string ext=a.extension().string();if(ext==".cpp"||ext==".cc"||ext==".c"||ext==".hpp"||ext==".h"){std::ifstream f(a);std::regex re(R"(^\s*#\s*include\s*[<\"]([^>\"]+)[>\"])"),m;std::string line;std::cout<<"\nIncludes:\n";bool any=false;while(std::getline(f,line)){std::smatch x;if(std::regex_search(line,x,re)){std::cout<<"  "<<x[1].str()<<"\n";any=true;}}if(!any)std::cout<<"  none\n";}std::cout<<"\nGit status: ";if(GitAnalyzer::is_repository(a.parent_path())){std::string root=GitAnalyzer::run(a.parent_path(),"rev-parse --show-toplevel");std::string st=GitAnalyzer::run(a.parent_path(),"status --porcelain -- "+a.string());std::cout<<(st.empty()?"Clean":"Modified or untracked")<<"\nProject root: "<<root<<"\n";}else std::cout<<"Not in a Git repository\n";}
