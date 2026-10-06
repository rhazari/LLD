#include "FileSystemBase.hpp"
#include "Folder.hpp"

std::string FileSystemBase::getPath() const {
    if (parent == nullptr) {
        return name;
    }

    std::string parentPath = parent->getPath();
    if (parentPath == "/") {
        return "/" + name;
    }
    return parentPath + "/" + name;
}