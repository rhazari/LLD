#pragma once
#include "FileSystemBase.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class Folder : public FileSystemBase {
private:
    std::unordered_map<std::string, std::shared_ptr<FileSystemBase>> children;

public:
    Folder(const std::string& name) : FileSystemBase(name) {}

    bool isDirectory() const override { return true; }

    bool addChild(std::shared_ptr<FileSystemBase> entry) {
        if (!entry || children.find(entry->getName()) != children.end()) return false;
        children[entry->getName()] = entry;
        entry->setParent(this);
        return true;
    }

    std::shared_ptr<FileSystemBase> removeChild(const std::string& name) {
        auto it = children.find(name);
        if (it == children.end()) return nullptr;
        auto entry = it->second;
        children.erase(it);
        entry->setParent(nullptr);
        return entry;
    }

    std::shared_ptr<FileSystemBase> getChild(const std::string& name) const {
        auto it = children.find(name);
        return (it != children.end()) ? it->second : nullptr;
    }

    bool hasChild(const std::string& name) const {
        return children.find(name) != children.end();
    }

    std::vector<std::shared_ptr<FileSystemBase>> getChildren() const {
        std::vector<std::shared_ptr<FileSystemBase>> result;
        for (const auto& pair : children) result.push_back(pair.second);
        return result;
    }
};