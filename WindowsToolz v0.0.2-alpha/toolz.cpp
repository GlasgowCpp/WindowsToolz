#include <SFML\Graphics.hpp>
#include <windows.h>
#include <string>
#include <vector>

enum class Screen {
    MainMenu,
    OptionsMenu,
    AboutMenu,
    BatchMenu
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
    bool darkMode = false;
    std::string mode = LoadColorMode();
    if(mode == "Dark") {
        darkMode = true;
    } else if(mode == "Light") {
        darkMode = false;
    } else {
        darkMode = GetSystemColorMode();
    }
    sf::Color currentBg = darkMode ? sf::Color::Black : sf::Color::White;
    sf::Color targetBg = currentBg;
    bool animating = false;
    sf::Clock animClock;
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
    //MAIN MENU
    sf::Text btnIconCache(font, "> Clear Icon Cache");
    btnIconCache.setCharacterSize(24);
    btnIconCache.setPosition({0.f, 125.f});
    sf::Text btnSystemFix(font, "> Find missing Windows Files");
    btnSystemFix.setCharacterSize(24);
    btnSystemFix.setPosition({0.f, 155.f});
    sf::Text btnOptions(font, "> Settings");
    btnOptions.setCharacterSize(24);
    btnOptions.setPosition({0.f, 185.f});
    sf::Text btnAbout(font, "> About");
    btnAbout.setCharacterSize(24);
    btnAbout.setPosition({0.f, 215.f});
    //SETTINGS
    sf::Text btnMenu(font, "> Back");
    btnMenu.setCharacterSize(24);
    btnMenu.setPosition({0.f, 0.f});
    sf::Text btnColor(font, "Mode: Unknown");
    btnColor.setCharacterSize(24);
    btnColor.setPosition({0.f, 50.f});
    if(mode == "Dark") btnColor.setString("> Mode: Dark");
    else if(mode == "Light") btnColor.setString("> Mode: Light");
    else btnColor.setString("> Mode: System");
    sf::Text btnBatch(font, "> Edit Batch Files");
    btnBatch.setCharacterSize(24);
    btnBatch.setPosition({0.f, 80.f});
    //BATCH EDITING MENU
    sf::Text BatchTitle(font, "Which Batch file do you want to edit?");
    BatchTitle.setCharacterSize(50);
    BatchTitle.setPosition({0.f, 0.f});
    sf::Text btnBatchBack(font, "> Back");
    btnBatchBack.setCharacterSize(24);
    btnBatchBack.setPosition({0.f, 75.f});
    sf::Text IconCacheEdit(font, "> Clear Icon Cache");
    IconCacheEdit.setCharacterSize(24);
    IconCacheEdit.setPosition({0.f, 105.f});
    sf::Text SystemFixEdit(font, "> Find missing Windows Files");
    SystemFixEdit.setCharacterSize(24);
    SystemFixEdit.setPosition({0.f, 135.f});
    //ABOUT
    sf::Text AboutTitle(font, "About Windows Toolz");
    AboutTitle.setCharacterSize(50);
    AboutTitle.setPosition({0.f, 0.f});
    sf::Text AboutBack(font, "> Back");
    AboutBack.setCharacterSize(24);
    AboutBack.setPosition({0.f, 75.f});
    sf::Text AboutAuthor(font, "Author: AnNeedle");
    AboutAuthor.setCharacterSize(24);
    AboutAuthor.setPosition({0.f, 105.f});
    sf::Text AboutVersion(font, "Version: 0.0.2-alpha");
    AboutVersion.setCharacterSize(24);
    AboutVersion.setPosition({0.f, 135.f});
    std::vector<sf::Text*> texts = {
        &titleT,
        &btnOptions,
        &btnMenu,
        &btnColor,
        &btnIconCache,
        &btnSystemFix,
        &btnBatch,
        &BatchTitle,
        &btnBatchBack,
        &IconCacheEdit,
        &SystemFixEdit,
        &btnAbout,
        &AboutTitle,
        &AboutBack,
        &AboutAuthor,
        &AboutVersion
    };
    while(window.isOpen()) {
        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) window.close();
            if(event->is<sf::Event::MouseButtonPressed>()) {
                auto mouse = sf::Mouse::getPosition(window);
                if(currentScreen == Screen::MainMenu) {
                    if(btnIconCache.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        system("start \"\" batch\\IconCache.bat");
                    }
                    if(btnSystemFix.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        ShellExecuteA(NULL, "runas", "batch\\SystemFix.bat", NULL, NULL, SW_SHOW);
                    }
                    if(btnOptions.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::OptionsMenu;
                    }
                    if(btnAbout.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::AboutMenu;
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
                            targetBg = sf::Color::Black;
                            animClock.restart();
                            animating = true;
                        } else if (mode == "Dark") {
                            mode = "SystemDefault";
                            SaveColorMode("SystemDefault");
                            btnColor.setString("> Mode: System");
                            darkMode = GetSystemColorMode();
                            targetBg = darkMode ? sf::Color::Black : sf::Color::White;
                            animClock.restart();
                            animating = true;
                        } else {
                            mode = "Light";
                            SaveColorMode("Light");
                            btnColor.setString("> Mode: Light");
                            darkMode = false;
                            targetBg = sf::Color::White;
                            animClock.restart();
                            animating = true;
                        }
                    }
                    if(btnBatch.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::BatchMenu;
                    }
                } else if(currentScreen == Screen::AboutMenu) {
                    if(AboutBack.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::MainMenu;
                    }
                } else if(currentScreen == Screen::BatchMenu) {
                    if(btnBatchBack.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        currentScreen = Screen::OptionsMenu;
                    }
                    if(IconCacheEdit.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        system("start notepad batch\\IconCache.bat");
                    }
                    if(SystemFixEdit.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        system("start notepad batch\\SystemFix.bat");
                    }
                }
            }
        }
        if(animating) {
            float t = animClock.getElapsedTime().asSeconds() / 0.5f;
            if(t >= 1.f) {
                t = 1.f;
                animating = false;
            }
            auto lerp = [](sf::Color a, sf::Color b, float t) {
                return sf::Color(
                    a.r + (b.r - a.r) * t,
                    a.g + (b.g - a.g) * t,
                    a.b + (b.b - a.b) * t
                );
            };
            currentBg = lerp(currentBg, targetBg, t);
        }
        window.clear(currentBg);
        sf::Color textColor = darkMode ? sf::Color::White : sf::Color::Black;
        for (auto t : texts) t->setFillColor(textColor);
        if(currentScreen == Screen::MainMenu) {
            window.draw(titleT);
            window.draw(sprite);
            window.draw(btnOptions);
            window.draw(btnIconCache);
            window.draw(btnSystemFix);
            window.draw(btnAbout);
        } else if(currentScreen == Screen::OptionsMenu) {
            window.draw(btnMenu);
            window.draw(btnColor);
            window.draw(btnBatch);
        } else if(currentScreen == Screen::AboutMenu) {
            window.draw(AboutTitle);
            window.draw(AboutBack);
            window.draw(AboutAuthor);
            window.draw(AboutVersion);
        } else if(currentScreen == Screen::BatchMenu) {
            window.draw(BatchTitle);
            window.draw(btnBatchBack);
            window.draw(IconCacheEdit);
            window.draw(SystemFixEdit);
        }
        window.display();
    }
    return 0;
}