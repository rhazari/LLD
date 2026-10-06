#include <iostream>
#include "FileSystemBase.hpp"
#include "Folder.hpp"
#include "File.hpp"
#include "FileSystem.hpp"

#include <iostream>
#include <memory>
#include <cassert>


void printDirectoryContents(const FileSystem& fs, const std::string& path) {
    std::cout << "Listing contents of '" << path << "':\n";
    try {
        auto items = fs.list(path);
        if (items.empty()) {
            std::cout << "  (empty)\n";
            return;
        }
        for (const auto& item : items) {
            std::cout << "  " << (item->isDirectory() ? "[DIR]  " : "[FILE] ") 
                      << item->getName() << " (Path: " << item->getPath() << ")\n";
        }
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << "\n";
    }
}

int main() {
    FileSystem fs;

    std::cout << "=== 1. Creating Folders and Files ===\n";
    fs.createFolder("/documents");
    fs.createFolder("/photos");
    fs.createFile("/documents/resume.txt", "C++ Software Engineer");
    fs.createFile("/documents/cover_letter.txt", "Dear Hiring Manager...");
    
    printDirectoryContents(fs, "/");
    printDirectoryContents(fs, "/documents");

    std::cout << "\n=== 2. Reading and Updating File Content ===\n";
    auto entry = fs.get("/documents/resume.txt");
    if (!entry->isDirectory()) {
        auto file = std::dynamic_pointer_cast<File>(entry);
        std::cout << "Original Content: " << file->getContent() << "\n";
        file->setContent("Senior C++ Systems Engineer");
        std::cout << "Updated Content:  " << file->getContent() << "\n";
    }

    std::cout << "\n=== 3. Moving and Renaming Entries ===\n";
    // Rename file inside folder
    fs.rename("/documents/cover_letter.txt", "letter.txt");
    
    // Move folder into another directory
    fs.createFolder("/archive");
    fs.move("/documents", "/archive/docs_backup");

    printDirectoryContents(fs, "/");
    printDirectoryContents(fs, "/archive");
    printDirectoryContents(fs, "/archive/docs_backup");

    std::cout << "\n=== 4. Deleting Entries ===\n";
    fs.deleteEntry("/photos");
    printDirectoryContents(fs, "/");

    std::cout << "\n=== 5. Handling Exception Guardrails ===\n";
    
    // Try accessing non-existent path
    try {
        fs.get("/non_existent_file.txt");
    } catch (const std::exception& e) {
        std::cout << "Caught Expected Error: " << e.what() << "\n";
    }

    // Try creating duplicate entry
    try {
        fs.createFolder("/archive");
    } catch (const std::exception& e) {
        std::cout << "Caught Expected Error: " << e.what() << "\n";
    }

    // Try moving directory inside itself
    try {
        fs.move("/archive", "/archive/docs_backup/loop");
    } catch (const std::exception& e) {
        std::cout << "Caught Expected Error: " << e.what() << "\n";
    }

    return 0;
}