#include "SmartTree.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <stdexcept>
#include <system_error>
static std::string mark(const fs::path&p){auto e=p.extension().string();if(e==".cpp"||e==".c"||e==".cc")return "[SRC] ";if(e==".h"||e==".hpp")return "[HDR] ";if(e==".sh")return "[TEST] ";if(p.filename()=="CMakeLists.txt"||p.filename()=="Makefile")return "[BUILD] ";if(p.filename()=="README.md")return "[DOC] ";return "";}
void SmartTree::show(const fs::path&r){std::error_code e;if(!fs::exists(r,e))throw std::runtime_error("Path does not exist: "+r.string());std::cout<<fs::absolute(r).filename().string()<<"/\n";std::function<void(fs::path,std::string)> walk=[&](fs::path p,std::string pre){std::vector<fs::directory_entry> v;for(fs::directory_iterator it(p,e),end;it!=end;it.increment(e)){if(e){e.clear();continue;}v.push_back(*it);}std::sort(v.begin(),v.end(),[](auto&a,auto&b){if(a.is_directory()!=b.is_directory())return a.is_directory()>b.is_directory();return a.path().filename()<b.path().filename();});for(size_t i=0;i<v.size();++i){bool last=i+1==v.size();std::string n=v[i].path().filename().string();std::cout<<pre<<(last?"└── ":"├── ")<<(v[i].is_directory()?"[DIR] ":mark(v[i].path()))<<n<<(v[i].is_directory()?"/":"")<<"\n";if(v[i].is_directory())walk(v[i].path(),pre+(last?"    ":"│   "));}};walk(fs::absolute(r),"");}
