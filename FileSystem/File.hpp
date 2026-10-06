#pragma once
#include "FileSystemBase.hpp"
#include <string>

class File : public FileSystemBase {
private:
    std::string content;

public:
    File(const std::string& name, const std::string& content)
        : FileSystemBase(name), content(content) {}

    std::string getContent() const { return content; }
    void setContent(const std::string& content) { this->content = content; }

    bool isDirectory() const override { return false; }
};