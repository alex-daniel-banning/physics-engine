#ifndef PROJECT_ROOT_HPP
#define PROJECT_ROOT_HPP

#include <string>

class ProjectRoot {
    public:
        static std::string getPath() {
            return projectRoot;
        }
        static std::string getPath(const char* postfix) {
            return std::string(projectRoot) + postfix;
        }
    private:
        static constexpr const char* projectRoot = PROJECT_ROOT;
};

#endif
