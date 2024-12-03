#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <algorithm>

int list_devices() {
    std::string paths[] = {"/sys/class/backlight", "/sys/class/leds"};
    for (std::string path : paths) {
        try {
            if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
                for (const auto& entry : std::filesystem::directory_iterator(path)) {
                    if (std::filesystem::is_symlink(entry.symlink_status())) {
                        if (entry.path().string().find("::numlock") != std::string::npos || 
                            entry.path().string().find("::scrolllock") != std::string::npos || 
                            entry.path().string().find("::capslock") != std::string::npos) {
                            
                            continue;
                        }
                        std::cout << entry.path().string() << std::endl; 
                    }
                }
            } else {
                std::cout << "Not a directory." << std::endl;
                return 1;
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }
    return 0;
}

float get_brightness(std::string backlight) {
    float brightness = 0;
    std::string brightness_str = "";

    std::ifstream f_brightness (backlight+"/brightness");
    if (f_brightness.is_open()) {
        std::getline(f_brightness, brightness_str);
        try {
            brightness = stof(brightness_str);
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalied input: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cout << "Out of range: " << e.what() << std::endl;
        }
        f_brightness.close();
        return brightness;
    } else {
        std::cerr << "Error opening: " << backlight << "/brightness" << std::endl;
        return 1;
    }
}

int get_max_brightness(std::string backlight) {
    int max_brightness = 0;
    std::string max_brightness_str = "";
    std::ifstream f_max_brightness (backlight+"/max_brightness");
    
    if (f_max_brightness.is_open()) {
        std::getline(f_max_brightness, max_brightness_str);
        try {
            max_brightness = stoi(max_brightness_str);
        } catch (const std::invalid_argument& e) {
            std::cout << "Invalied input: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cout << "Out of range: " << e.what() << std::endl;
        }
        f_max_brightness.close();
        return max_brightness;
    } else {
        std::cerr << "Error opening " << backlight << "/max_brightness" << std::endl;
        return 1;
    }
}

int get_brightness_percent(std::string backlight) {
    float brightness = get_brightness(backlight);
    int max_brightness = get_max_brightness(backlight);
    int brightness_percent = ((brightness / max_brightness) * 100);
    return brightness_percent;
}

int set_brightness_absolute_percent(std::string backlight, int percentage) {
    if (backlight.empty()) {
        std::cerr << "Error backlight empty." << std::endl;
    }

    int new_brightness = 0;
    int max_brightness = get_max_brightness(backlight);

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
        std::cerr << "Error opening: " << backlight << "/brightness" << std::endl;
        return 1;
    }
    return 0;
}

int set_brightness_relative_percent(std::string backlight, int percentage, std::string operation) {
    int new_brightness_percent = 0;
    int max_brightness = get_max_brightness(backlight);
    float brightness = get_brightness(backlight);
    
    if (operation == "+") {
        new_brightness_percent = ((brightness / max_brightness) * 100) + percentage;
    } else if (operation == "-") {
        new_brightness_percent = ((brightness / max_brightness) * 100) - percentage;
    }
    set_brightness_absolute_percent(backlight, new_brightness_percent);
    return 0;
}

std::string check_backlight(std::string device) {
    if (std::filesystem::exists(device) && device.find("/sys/class/") != std::string::npos) {
        return device;
    } else if (std::filesystem::exists("/sys/class/backlight/" + device) && !std::filesystem::exists("/sys/class/leds/" + device)) {
        return "/sys/class/backlight/" + device;
    } else if (!std::filesystem::exists("/sys/class/backlight/" + device) && std::filesystem::exists("/sys/class/leds/" + device)) {
        return "/sys/class/leds/" + device;
    } else {
        throw std::runtime_error("Error: device does not exist: " + device);   
    }
}

void help() {
    std::cout << "usage: cpplighty [--help] [--list] [--device DEVICE] [--get] [--get-steps]" << std::endl;
    std::cout << "                 [--set PERCENT] [--inc PERCENT] [--dec PERCENT]" << std::endl;

}

int main(int argc, char *argv[]) {
    std::string percent = "";
    std::string operation = "";
    std::string device = "auto";
    bool get_percent = false;
    bool get_steps = false;

    if (argc == 1) {
        help();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help();
            return 0;
        } else if (strcmp(argv[i], "--list") == 0) {
            list_devices();
            return 0;
        } else if (strcmp(argv[i], "--device") == 0 && i + 1 < argc) {
            device = check_backlight(argv[++i]);
        } else if (strcmp(argv[i], "--get") == 0) {
            get_percent = true;
        } else if (strcmp(argv[i], "--get-steps") == 0) {
            get_steps = true;
        } else if (strcmp(argv[i], "--set") == 0 && i + 1 < argc) {
            operation = "abs";
            percent = argv[++i];
        } else if (strcmp(argv[i], "--inc") == 0 && i + 1 < argc) {
            operation = "+";
            percent = argv[++i];
        } else if (strcmp(argv[i], "--dec") == 0 && i + 1 < argc) {
            operation = "-";
            percent = argv[++i]; 
        } else {
            help();
            return 0;
        }
    }
    if (device == "auto") {
        auto backlightDir = std::filesystem::directory_iterator("/sys/class/backlight/");
        auto it = std::find_if(backlightDir, std::filesystem::directory_iterator{}, [](const std::filesystem::directory_entry& entry) {
            return entry.is_directory(); 
        });

        if (it == std::filesystem::directory_iterator{}) {
            std::cout << "No backlights found in /sys/class/backlight/" << std::endl;
            return 1;
        }
        device = it->path().string();
        std::cout << "Backlight not specified using " << device << std::endl;
        #ifdef DEBUG
        std::cout << "Selected backlight device: " << device << std::endl;
        #endif 
    }

    if (get_percent) {
        std::cout << get_brightness_percent(device) << std::endl;
        return 0;
    }

    if (get_steps) {
        std::cout << get_max_brightness(device) << std::endl;
        return 0;
    }

    try {
        if (operation == "abs") {
            set_brightness_absolute_percent(device, stoi(percent));
        } else if (operation == "+") {
            set_brightness_relative_percent(device, stoi(percent), "+");
        } else if (operation == "-") {
            set_brightness_relative_percent(device, stoi(percent), "-");
        }
    } catch (const std::invalid_argument& e) {
        std::cout << "Invalied input: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Out of range: " << e.what() << std::endl;
    }

    return 0;
}