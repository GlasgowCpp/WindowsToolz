#include <SFML\Graphics.hpp>
#include <windows.h>
#include <string>

enum class Screen {
    MainMenu,
    OptionsMenu
};

bool GetSystemColorMode() {
    HKEY hKey;
    DWORD value = 1;
    DWORD size = sizeof(value);
    if(RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0,
        KEY_READ,
        &hKey
    ) == ERROR_SUCCESS)
    {
        RegGetValueA(
            hKey,
            NULL,
            "AppsUseLightTheme",
            RRF_RT_REG_DWORD,
            NULL,
            &value,
            &size
        );
        RegCloseKey(hKey);
    }
    return value == 0;
}

void SaveColorMode(const std::string& mode) {
    HKEY hKey;
    RegCreateKeyExA(
        HKEY_CURRENT_USER,
        "Software\\WindowsToolz",
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL
    );
    RegSetValueExA(
        hKey,
        "ColorMode",
        0,
        REG_SZ,
        (const BYTE*)mode.c_str(),
        mode.size() + 1
    );
    RegCloseKey(hKey);
}

std::string LoadColorMode() {
    HKEY hKey;
    char buffer[32] = {0};
    DWORD size = sizeof(buffer);
    if(RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\WindowsToolz",
        0,
        KEY_READ,
        &hKey
    ) == ERROR_SUCCESS)
    {
        RegGetValueA(
            hKey,
            NULL,
            "ColorMode",
            RRF_RT_REG_SZ,
            NULL,
            buffer,
            &size
        );
        RegCloseKey(hKey);
        return buffer;
    }
    SaveColorMode("SystemDefault");
    return "SystemDefault";
}

int main() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    std::string dir = exePath.substr(0, exePath.find_last_of("\\/"));
    SetCurrentDirectoryA(dir.c_str());
    std::string mode = LoadColorMode();
    bool darkMode = false;
    if(mode == "Dark") {
        darkMode = true;
    } else if(mode == "Light") {
        darkMode = false;
    } else {
        darkMode = GetSystemColorMode();
    }
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Windows Toolz", sf::Style::Titlebar | sf::Style::Close);
    window.setView(sf::View(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f})));
    sf::Image icon;
    if(icon.loadFromFile("assets\\images\\icon.png")) window.setIcon(icon);
    sf::Font font;
    if(!font.openFromFile("assets\\fonts\\Roboto.ttf")) return -1;
    sf::Font titleF;
    if(!titleF.openFromFile("assets\\fonts\\Oswald.ttf")) return -1;
    sf::Text titleT(titleF, "WINDOWS TOOLZ");
    titleT.setPosition({125.f, 0.f});
    titleT.setCharacterSize(110);
    sf::Texture tex;
    if(!tex.loadFromFile("assets\\images\\icon.png")) return -1;
    sf::Sprite sprite(tex);
    sprite.setScale({0.25f, 0.25f});
    sprite.setPosition({0.f, 0.f});
    Screen currentScreen = Screen::MainMenu;
    //MENU GŁÓWNE
    sf::Text btnIconCache(font, "> Clear Icon Cache");
    btnIconCache.setCharacterSize(24);
    btnIconCache.setPosition({0.f, 125.f});
    sf::Text btnOptions(font, "> Settings");
    btnOptions.setCharacterSize(24);
    btnOptions.setPosition({0.f, 155.f});
    //USTAWIENIA
    sf::Text btnMenu(font, "> Back");
    btnMenu.setCharacterSize(24);
    btnMenu.setPosition({0.f, 0.f});
    sf::Text btnColor(font, "Mode: ???");
    btnColor.setCharacterSize(24);
    btnColor.setPosition({0.f, 50.f});
    if(mode == "Dark") btnColor.setString("> Mode: Dark");
    else if(mode == "Light") btnColor.setString("> Mode: Light");
    else btnColor.setString("> Mode: System");
    while(window.isOpen()) {
        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) window.close();
            if(event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse = sf::Mouse::getPosition(window);
                if(currentScreen == Screen::MainMenu) {
                    if(btnOptions.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::OptionsMenu;
                    }
                    if(btnIconCache.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        system("start \"\" batch\\IconCache.bat");
                    }
                }
                else if(currentScreen == Screen::OptionsMenu) {
                    if(btnMenu.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::MainMenu;
                    }
                    if(btnColor.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        if(mode == "Light") {
                            mode = "Dark";
                            SaveColorMode("Dark");
                            btnColor.setString("> Mode: Dark");
                            darkMode = true;
                        } else if (mode == "Dark") {
                            mode = "SystemDefault";
                            SaveColorMode("SystemDefault");
                            btnColor.setString("> Mode: System");
                            darkMode = GetSystemColorMode();
                        } else {
                            mode = "Light";
                            SaveColorMode("Light");
                            btnColor.setString("> Mode: Light");
                            darkMode = false;
                        }
                    }
                }
            }
        }
        if(darkMode) {
            window.clear(sf::Color::Black);
            titleT.setFillColor(sf::Color::White);
            btnOptions.setFillColor(sf::Color::White);
            btnMenu.setFillColor(sf::Color::White);
            btnColor.setFillColor(sf::Color::White);
        } else {
            window.clear(sf::Color::White);
            titleT.setFillColor(sf::Color::Black);
            btnOptions.setFillColor(sf::Color::Black);
            btnMenu.setFillColor(sf::Color::Black);
            btnColor.setFillColor(sf::Color::Black);
        }
        if(currentScreen == Screen::MainMenu) {
            window.draw(titleT);
            window.draw(sprite);
            window.draw(btnOptions);
            window.draw(btnIconCache);
        } else if(currentScreen == Screen::OptionsMenu) {
            window.draw(btnMenu);
            window.draw(btnColor);
        }
        window.display();
    }
    return 0;
}