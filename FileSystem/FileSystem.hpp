#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>

class FileSystem {
private:
    std::shared_ptr<Folder> root;

    std::shared_ptr<FileSystemBase> resolvePath(const std::string& path) const {
        if (path.empty()) {
            throw std::invalid_argument("Path cannot be empty");
        }

        if (path[0] != '/') {
            throw std::invalid_argument("Path must be absolute");
        }

        if (path == "/") {
            return root;
        }

        std::vector<std::string> parts;
        std::string part;
        std::istringstream stream(path.substr(1));
        while (std::getline(stream, part, '/')) {
            parts.push_back(part);
        }

        std::shared_ptr<FileSystemBase> current = root;

        for (const auto& p : parts) {
            if (p.empty()) {
                throw std::invalid_argument("Invalid path: consecutive slashes");
            }

            if (!current->isDirectory()) {
                throw std::invalid_argument("Not a directory");
            }

            auto folder = std::dynamic_pointer_cast<Folder>(current);
            auto child = folder->getChild(p);
            if (!child) {
                throw std::invalid_argument("Path not found: " + path);
            }

            current = child;
        }

        return current;
    }

    std::shared_ptr<Folder> resolveParent(const std::string& path) const {
        if (path == "/") {
            throw std::invalid_argument("Root has no parent");
        }

        size_t lastSlash = path.rfind('/');
        std::string parentPath = lastSlash == 0 ? "/" : path.substr(0, lastSlash);

        auto parent = resolvePath(parentPath);

        if (!parent->isDirectory()) {
            throw std::invalid_argument("Parent is not a directory");
        }

        return std::dynamic_pointer_cast<Folder>(parent);
    }

    std::string extractName(const std::string& path) const {
        size_t lastSlash = path.rfind('/');
        return path.substr(lastSlash + 1);
    }

public:
    FileSystem() : root(std::make_shared<Folder>("/")) {}

    std::shared_ptr<File> createFile(const std::string& path, const std::string& content) {
        if (path == "/") {
            throw std::invalid_argument("Cannot create file at root");
        }

        auto parent = resolveParent(path);
        std::string fileName = extractName(path);

        if (parent->hasChild(fileName)) {
            throw std::runtime_error("Entry already exists: " + fileName);
        }

        auto file = std::make_shared<File>(fileName, content);
        parent->addChild(file);
        return file;
    }

    std::shared_ptr<Folder> createFolder(const std::string& path) {
        if (path == "/") {
            throw std::runtime_error("Root already exists");
        }

        auto parent = resolveParent(path);
        std::string folderName = extractName(path);

        if (parent->hasChild(folderName)) {
            throw std::runtime_error("Entry already exists: " + folderName);
        }

        auto folder = std::make_shared<Folder>(folderName);
        parent->addChild(folder);
        return folder;
    }

    void deleteEntry(const std::string& path) {
        if (path == "/") {
            throw std::invalid_argument("Cannot delete root");
        }

        auto parent = resolveParent(path);
        std::string name = extractName(path);

        auto removed = parent->removeChild(name);
        if (!removed) {
            throw std::invalid_argument("Entry not found: " + path);
        }
    }

    std::vector<std::shared_ptr<FileSystemBase>> list(const std::string& path) const {
        auto entry = resolvePath(path);

        if (!entry->isDirectory()) {
            throw std::invalid_argument("Cannot list a file");
        }

        return std::dynamic_pointer_cast<Folder>(entry)->getChildren();
    }

    std::shared_ptr<FileSystemBase> get(const std::string& path) const {
        return resolvePath(path);
    }

    void rename(const std::string& path, const std::string& newName) {
        if (path == "/") {
            throw std::invalid_argument("Cannot rename root");
        }

        if (newName.empty() || newName.find('/') != std::string::npos) {
            throw std::invalid_argument("Invalid name");
        }

        auto parent = resolveParent(path);
        std::string oldName = extractName(path);

        if (!parent->hasChild(oldName)) {
            throw std::invalid_argument("Entry not found");
        }

        if (parent->hasChild(newName)) {
            throw std::runtime_error("Entry already exists: " + newName);
        }

        auto entry = parent->removeChild(oldName);
        entry->setName(newName);
        parent->addChild(entry);
    }

    void move(const std::string& srcPath, const std::string& destPath) {
        if (srcPath == "/") {
            throw std::invalid_argument("Cannot move root");
        }

        auto srcParent = resolveParent(srcPath);
        std::string srcName = extractName(srcPath);
        auto entry = srcParent->getChild(srcName);

        if (!entry) {
            throw std::invalid_argument("Source not found: " + srcPath);
        }

        auto destParent = resolveParent(destPath);
        std::string destName = extractName(destPath);

        if (entry->isDirectory()) {
            Folder* current = destParent.get();
            while (current != nullptr) {
                if (current == entry.get()) {
                    throw std::invalid_argument("Cannot move folder into itself");
                }
                current = current->getParent();
            }
        }

        if (destParent->hasChild(destName)) {
            throw std::runtime_error("Destination already exists: " + destPath);
        }

        srcParent->removeChild(srcName);
        entry->setName(destName);
        destParent->addChild(entry);
    }
};