#include "includes/Utils.hpp"
#include <iostream>

int main() {
    // Test string utilities
    std::cout << "=== Testing String Utilities ===" << std::endl;
    
    std::string test_str = "  Hello World  ";
    std::cout << "Original: '" << test_str << "'" << std::endl;
    std::cout << "Trimmed: '" << Utils::trim(test_str) << "'" << std::endl;
    
    std::string test_split = "one,two,three,four";
    std::vector<std::string> parts = Utils::split(test_split, ',');
    std::cout << "Split '" << test_split << "' by comma:" << std::endl;
    for (size_t i = 0; i < parts.size(); ++i) {
        std::cout << "  [" << i << "] = '" << parts[i] << "'" << std::endl;
    }
    
    std::cout << "toLowerCase('HELLO'): '" << Utils::toLowerCase("HELLO") << "'" << std::endl;
    std::cout << "toUpperCase('world'): '" << Utils::toUpperCase("world") << "'" << std::endl;
    
    // Test file utilities
    std::cout << "\n=== Testing File Utilities ===" << std::endl;
    std::cout << "fileExists('www/index.html'): " << (Utils::fileExists("www/index.html") ? "true" : "false") << std::endl;
    std::cout << "isDirectory('www'): " << (Utils::isDirectory("www") ? "true" : "false") << std::endl;
    
    std::string ext = Utils::getFileExtension("test.html");
    std::cout << "getFileExtension('test.html'): '" << ext << "'" << std::endl;
    std::cout << "getMimeType('" << ext << "'): '" << Utils::getMimeType(ext) << "'" << std::endl;
    
    // Test URL utilities
    std::cout << "\n=== Testing URL Utilities ===" << std::endl;
    std::string encoded = Utils::urlEncode("hello world!");
    std::cout << "urlEncode('hello world!'): '" << encoded << "'" << std::endl;
    std::cout << "urlDecode('" << encoded << "'): '" << Utils::urlDecode(encoded) << "'" << std::endl;
    
    // Test time utilities
    std::cout << "\n=== Testing Time Utilities ===" << std::endl;
    std::cout << "getCurrentDateTime(): '" << Utils::getCurrentDateTime() << "'" << std::endl;
    std::cout << "formatHttpDate(time(0)): '" << Utils::formatHttpDate(time(0)) << "'" << std::endl;
    
    return 0;
}
