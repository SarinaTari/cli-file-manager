#include "HealthChecker.h"
#include "GitAnalyzer.h"
#include "FileHasher.h"
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <set>
#include <system_error>
#include <vector>
namespace{void result(bool ok,const std::string&s){std::cout<<"["<<(ok?"✓":"!")<<"] "<<s<<"\n";}}
void HealthChecker::check(const fs::path&r){std::error_code e;if(!fs::exists(r,e))throw std::runtime_error("Path does not exist: "+r.string());std::cout<<"Project Health Check\n────────────────────────────\n";bool git=GitAnalyzer::is_repository(r);result(git,"Git repository detected");bool cm=fs::exists(r/"CMakeLists.txt",e);result(cm,"CMakeLists.txt found");bool src=fs::is_directory(r/"src",e);result(src,"Source directory found");bool inc=fs::is_directory(r/"include",e);result(inc,"Include directory found");size_t modified=0,untracked=0;if(git){auto st=GitAnalyzer::run(r,"status --porcelain");for(size_t i=0;i<st.size();){size_t j=st.find('\n',i);auto l=st.substr(i,j==std::string::npos?std::string::npos:j-i);if(l.size()>=2){if(l[0]=='?')untracked++;else if(l[1]!=' ')modified++;}if(j==std::string::npos)break;i=j+1;}}result(modified==0,"No modified files");if(modified)std::cout<<"  Modified files: "<<modified<<"\n";result(untracked==0,"No untracked files");if(untracked)std::cout<<"  Untracked files: "<<untracked<<"\n";bool broken=true;for(fs::recursive_directory_iterator it(r,fs::directory_options::skip_permission_denied,e),end;it!=end;it.increment(e)){if(e){e.clear();continue;}if(it->is_symlink(e)){std::error_code x;if(!fs::exists(it->path(),x)){broken=false;break;}}}result(broken,"No broken symbolic links");std::cout<<"\nHealth: "<<((git&&cm&&src&&inc&&modified==0&&untracked==0&&broken)?"GOOD":"CHECK")<<"\n";}
