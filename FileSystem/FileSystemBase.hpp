#pragma once
#include <string>

class Folder;

class FileSystemBase {
protected:
    std::string name;
    Folder* parent;

public:
    FileSystemBase(const std::string& name) : name(name), parent(nullptr) {}
    virtual ~FileSystemBase() = default;

    std::string getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }

    Folder* getParent() const { return parent; }
    void setParent(Folder* parent) { this->parent = parent; }

    std::string getPath() const; // Declaration only

    virtual bool isDirectory() const = 0;
};