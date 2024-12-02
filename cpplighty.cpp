#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

int set_brightness_absolute_percent(std::string backlight, int percentage) {
    int new_brightness = 0;
    std::string max_brightness_str = "";
    int max_brightness = 0;

    std::ifstream f_max_brightness (backlight+"/max_brightness");
    if (f_max_brightness.is_open()) {
        std::getline(f_max_brightness, max_brightness_str);
        max_brightness = stoi(max_brightness_str);
        f_max_brightness.close();   
    } else {
        std::cerr << "Error opening " << backlight << "/max_brightness" << std::endl;
        return 1;
    }

    new_brightness = ( percentage / 100.0 ) * max_brightness;
    if (new_brightness > max_brightness) {
        new_brightness = max_brightness;
    } else if (new_brightness < 0) {
        new_brightness = 0;
    }
    
    std::ofstream f_brightness(backlight+"/brightness");
    if (f_brightness.is_open()) {
        f_brightness << new_brightness;
        f_brightness.close();
    } else {
        std::cerr << "Error opening " << backlight << "/brightness" << std::endl;
        return 1;
    }
    return 0;
}

int set_brightness_relative_percent(std::string backlight, int percentage, std::string operation) {
    int new_brightness_percent = 0;
    std::string max_brightness_str = "";
    int max_brightness = 0;
    std::string brightness = "";
    
    std::ifstream f_max_brightness (backlight+"/max_brightness");
    if (f_max_brightness.is_open()) {
        std::getline(f_max_brightness, max_brightness_str);
        max_brightness = std::stoi(max_brightness_str);
        f_max_brightness.close();   
    } else {
        std::cerr << "Error opening " << backlight << "/max_brightness" << std::endl;
        return 1;
    }

    std::ifstream f_brightness (backlight+"/brightness");
    if (f_brightness.is_open()) {
        std::getline(f_brightness, brightness);
        f_brightness.close();
    } else {
        std::cerr << "Error opening " << backlight << "/brightness" << std::endl;
        return 1;
    }

    if (operation == "+") {
        new_brightness_percent = ((std::stof(brightness) / max_brightness) * 100) + percentage ;
    } else if (operation == "-") {
        new_brightness_percent = ((std::stof(brightness) / max_brightness) * 100) - percentage ;
    }
    set_brightness_absolute_percent(backlight, new_brightness_percent);
    return 0;
}

int main(int argc, char *argv[]) {
    
    bool verboseFlag = false;
    std::string filename;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            std::cout << "help" << std::endl;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verboseFlag = true;
        } else if (strcmp(argv[i], "--file") == 0 && i + 1 < argc) {
            filename = argv[++i]; 
        }
    }

    if (!filename.empty()) {
        std::cout << "Processing file: " << filename << std::endl;
    }
    return 0;
}