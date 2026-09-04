#include "SafetyAnalyzer.h"
#include <iostream>
#include <stdexcept>
#include <system_error>
struct Stats{std::uintmax_t files=0,dirs=0,bytes=0;};
static void collect(const fs::path&p,Stats&s){std::error_code e;if(fs::is_regular_file(p,e)||fs::is_symlink(p,e)){s.files++;if(fs::is_regular_file(p,e)){auto z=fs::file_size(p,e);if(!e)s.bytes+=z;}return;}if(fs::is_directory(p,e)){s.dirs++;for(fs::recursive_directory_iterator it(p,fs::directory_options::skip_permission_denied,e),end;it!=end;it.increment(e)){if(e){e.clear();continue;}auto&q=*it;std::error_code x;if(q.is_directory(x))s.dirs++;else if(q.is_regular_file(x)){s.files++;auto z=q.file_size(x);if(!x)s.bytes+=z;}}}}
bool SafetyAnalyzer::is_dangerous(const fs::path&p,const fs::path&cur){std::error_code e;fs::path a=fs::weakly_canonical(p,e),c=fs::weakly_canonical(cur,e);if(e)return true;if(a==a.root_path()||a==c||a==c.parent_path())return true;return false;}
void SafetyAnalyzer::preview_remove(const fs::path&p){std::error_code e;if(!fs::exists(p,e)&&!fs::is_symlink(p,e))throw std::runtime_error("Path does not exist: "+p.string());Stats s;collect(p,s);std::cout<<"Deletion plan:\n"<<"────────────────────────\n"<<"Files:        "<<s.files<<"\nDirectories:  "<<s.dirs<<"\nTotal size:   "<<s.bytes<<" bytes\n\n";if(p.filename()==".git"||p.filename()=="CMakeLists.txt"||p.filename()=="src"||p.filename()=="include")std::cout<<"Potentially important: "<<p.filename().string()<<"\n\n";}
