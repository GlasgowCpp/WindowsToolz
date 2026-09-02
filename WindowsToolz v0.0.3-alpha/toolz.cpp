#include <SFML\Graphics.hpp>
#include <windows.h>
#include <string>
#include <vector>
#include <dwmapi.h>

enum class Screen {
    MainMenu,
    SettingsMenu,
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
    HWND hwnd = window.getSystemHandle();
        BOOL useDark = darkMode ? TRUE : FALSE;
        DwmSetWindowAttribute(
            hwnd,
            20,
            &useDark,
            sizeof(useDark)
        );
    window.setView(sf::View(sf::FloatRect({0.f, 0.f}, {1280.f, 720.f})));
    sf::Image icon;
    if(icon.loadFromFile("assets\\images\\icon.png")) {
        window.setIcon(
            icon.getSize().x,
            icon.getSize().y,
            icon.getPixelsPtr()
        );
    }
    HCURSOR hand = LoadCursor(NULL, IDC_HAND);
    HCURSOR arrow = LoadCursor(NULL, IDC_ARROW);
    //FONTS
    sf::Font font; if(!font.loadFromFile("assets\\fonts\\Roboto.ttf")) return -1;
    sf::Font titleF; if(!titleF.loadFromFile("assets\\fonts\\Oswald.ttf")) return -1;
    //TEXTURES
    sf::Texture logoT; if(!logoT.loadFromFile("assets\\images\\icon.png")) return -1;
    sf::Texture BackT; if(!BackT.loadFromFile("assets\\images\\Arrow.png")) return -1;
    sf::Texture UnknownT; if(!UnknownT.loadFromFile("assets\\images\\Unknown.png")) return -1;
    sf::Texture IconCacheT; if(!IconCacheT.loadFromFile("assets\\images\\IconCache.png")) return -1;
    sf::Texture Sun; if(!Sun.loadFromFile("assets\\images\\Light.png")) return -1;
    sf::Texture Moon; if(!Moon.loadFromFile("assets\\images\\Dark.png")) return -1;
    sf::Texture SystemFixT; if(!SystemFixT.loadFromFile("assets\\images\\SystemFix.png")) return -1;
    sf::Texture SettingsT; if(!SettingsT.loadFromFile("assets\\images\\Settings.png")) return -1;
    sf::Texture AboutT; if(!AboutT.loadFromFile("assets\\images\\About.png")) return -1;
    sf::Texture BatchT; if(!BatchT.loadFromFile("assets\\images\\Batch.png")) return -1;
    //MAIN-MENU SPRITES
    sf::Sprite BackS(BackT);
    sf::Sprite IconCacheS(IconCacheT);
    sf::Sprite logoS(logoT);
    sf::Sprite SystemFixS(SystemFixT);
    sf::Sprite SettingsS(SettingsT);
    sf::Sprite AboutS(AboutT);
    //SETTINGS SPRITES
    sf::Sprite ModeIcon(UnknownT);
    sf::Sprite BatchS(BatchT);
    //MAIN-MENU TEXTS
    sf::Text titleT("WINDOWS TOOLZ", titleF);
    sf::Text btnIconCache("Clear Icon Cache", font);
    sf::Text btnSystemFix("Find missing Windows Files", font);
    sf::Text btnSettings("Settings", font);
    sf::Text btnAbout("About", font);
    //SETTINGS TEXTS
    sf::Text SettingsTitle("Settings", font);
    sf::Text SettingsBack("Back", font);
    sf::Text btnColor("Mode: Unknown", font);
    sf::Text btnBatch("Edit Batch Files", font);
    //BATCH EDITING TEXTS
    sf::Text BatchTitle("Which Batch file do you want to edit?", font);
    sf::Text BatchBack("Back", font);
    sf::Text IconCacheEdit("Clear Icon Cache", font);
    sf::Text SystemFixEdit("Find missing Windows Files", font);
    Screen currentScreen = Screen::MainMenu;
    //ABOUT TEXTS
    sf::Text AboutTitle("About Windows Toolz", font);
    sf::Text AboutBack("Back", font);
    sf::Text AboutAuthor("Author: AnNeedle", font);
    sf::Text AboutVersion("Version: 0.0.3-alpha", font);
    std::vector<sf::Text*> texts = {
        &titleT,
        &btnSettings,
        &SettingsBack,
        &btnColor,
        &SettingsTitle,
        &btnIconCache,
        &btnSystemFix,
        &btnBatch,
        &BatchTitle,
        &BatchBack,
        &IconCacheEdit,
        &SystemFixEdit,
        &btnAbout,
        &AboutTitle,
        &AboutBack,
        &AboutAuthor,
        &AboutVersion
    };
    while(window.isOpen()) {
        bool hovering = false;
        auto mouse = sf::Mouse::getPosition(window);
        sf::Vector2f m((float)mouse.x, (float)mouse.y);
        if(currentScreen == Screen::MainMenu) {
            if(IconCacheS.getGlobalBounds().contains(m) || btnIconCache.getGlobalBounds().contains(m)) hovering = true;
            if(SystemFixS.getGlobalBounds().contains(m) || btnSystemFix.getGlobalBounds().contains(m)) hovering = true;
            if(SettingsS.getGlobalBounds().contains(m) || btnSettings.getGlobalBounds().contains(m)) hovering = true;
            if(AboutS.getGlobalBounds().contains(m) || btnAbout.getGlobalBounds().contains(m)) hovering = true;
        } else if(currentScreen == Screen::SettingsMenu) {
            if(BackS.getGlobalBounds().contains(m) || SettingsBack.getGlobalBounds().contains(m)) hovering = true;
            if(ModeIcon.getGlobalBounds().contains(m) || btnColor.getGlobalBounds().contains(m)) hovering = true;
            if(BatchS.getGlobalBounds().contains(m) || btnBatch.getGlobalBounds().contains(m)) hovering = true;
        } else if(currentScreen == Screen::BatchMenu) {
            if(BackS.getGlobalBounds().contains(m) || BatchBack.getGlobalBounds().contains(m)) hovering = true;
            if(IconCacheS.getGlobalBounds().contains(m) || IconCacheEdit.getGlobalBounds().contains(m)) hovering = true;
            if(SystemFixS.getGlobalBounds().contains(m) || SystemFixEdit.getGlobalBounds().contains(m)) hovering = true;
        } else if(currentScreen == Screen::AboutMenu) {
            if(BackS.getGlobalBounds().contains(m) || AboutBack.getGlobalBounds().contains(m)) hovering = true;
        }
        if(hovering)
            SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)hand);
        else
            SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)arrow);
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::MouseButtonPressed) {
                auto mouse = sf::Mouse::getPosition(window);
                if(currentScreen == Screen::MainMenu) {
                    if(IconCacheS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnIconCache.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) system("start \"\" batch\\IconCache.bat");
                    if(SystemFixS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnSystemFix.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) ShellExecuteA(NULL, "runas", "batch\\SystemFix.bat", NULL, NULL, SW_SHOW);
                    if(SettingsS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnSettings.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::SettingsMenu;
                    if(AboutS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnAbout.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::AboutMenu;
                }
                else if(currentScreen == Screen::SettingsMenu) {
                    if(BackS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || SettingsBack.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::MainMenu;
                    if(ModeIcon.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnColor.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) {
                        if(mode == "Light") {
                            mode = "Dark";
                            SaveColorMode("Dark");
                            btnColor.setString("Mode: Dark");
                            darkMode = true;
                            targetBg = sf::Color::Black;
                            animClock.restart();
                            animating = true;
                        } else if (mode == "Dark") {
                            mode = "SystemDefault";
                            SaveColorMode("SystemDefault");
                            btnColor.setString("Mode: System");
                            darkMode = GetSystemColorMode();
                            targetBg = darkMode ? sf::Color::Black : sf::Color::White;
                            animClock.restart();
                            animating = true;
                        } else {
                            mode = "Light";
                            SaveColorMode("Light");
                            btnColor.setString("Mode: Light");
                            darkMode = false;
                            targetBg = sf::Color::White;
                            animClock.restart();
                            animating = true;
                        }
                        BOOL useDark = darkMode ? TRUE : FALSE;
                        DwmSetWindowAttribute(window.getSystemHandle(), 20, &useDark, sizeof(useDark));
                    }
                    if(BatchS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || btnBatch.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::BatchMenu;
                } else if(currentScreen == Screen::BatchMenu) {
                    if(BackS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || BatchBack.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::SettingsMenu;
                    if(IconCacheS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || IconCacheEdit.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) system("start notepad batch\\IconCache.bat");
                    if(SystemFixS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || SystemFixEdit.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) system("start notepad batch\\SystemFix.bat");
                } else if(currentScreen == Screen::AboutMenu) {
                    if(BackS.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y}) || AboutBack.getGlobalBounds().contains({(float)mouse.x, (float)mouse.y})) currentScreen = Screen::MainMenu;
                }
            }
        }
        //COLOR SWITCHING ANIMATION
        if(animating) {
            float t = animClock.getElapsedTime().asSeconds() / 10.f;
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
        //WINDOW CONTENTS RENDERING
        if(currentScreen == Screen::MainMenu) {
            logoS.setScale({0.25f, 0.25f}); logoS.setPosition({0.f, 0.f}); window.draw(logoS);
            titleT.setPosition({125.f, 0.f}); titleT.setCharacterSize(110); window.draw(titleT);
            IconCacheS.setPosition({0.f, 130.f}); window.draw(IconCacheS);
            btnIconCache.setCharacterSize(24); btnIconCache.setPosition({30.f, 125.f}); window.draw(btnIconCache);
            SystemFixS.setPosition({0.f, 160.f}); window.draw(SystemFixS);
            btnSystemFix.setCharacterSize(24); btnSystemFix.setPosition({30.f, 155.f}); window.draw(btnSystemFix);
            SettingsS.setPosition({0.f, 190.f}); window.draw(SettingsS);
            btnSettings.setCharacterSize(24); btnSettings.setPosition({30.f, 185.f}); window.draw(btnSettings);
            AboutS.setPosition({0.f, 220.f}); window.draw(AboutS);
            btnAbout.setCharacterSize(24); btnAbout.setPosition({30.f, 215.f}); window.draw(btnAbout);
        } else if(currentScreen == Screen::SettingsMenu) {
            SettingsTitle.setCharacterSize(50); SettingsTitle.setPosition({0.f, 0.f}); window.draw(SettingsTitle);
            BackS.setPosition({0.f, 80.f}); window.draw(BackS);
            SettingsBack.setCharacterSize(24); SettingsBack.setPosition({30.f, 75.f}); window.draw(SettingsBack);
            ModeIcon.setPosition({0.f, 110.f}); window.draw(ModeIcon);
            if(mode == "Light") ModeIcon.setTexture(Sun);
            else if(mode == "Dark") ModeIcon.setTexture(Moon);
            else {
                if(darkMode) ModeIcon.setTexture(Moon);
                else ModeIcon.setTexture(Sun);
            }
            if(mode == "Dark") btnColor.setString("Mode: Dark");
            else if(mode == "Light") btnColor.setString("Mode: Light");
            else btnColor.setString("Mode: System");
            btnColor.setCharacterSize(24); btnColor.setPosition({30.f, 105.f}); window.draw(btnColor);
            BatchS.setPosition({0.f, 140.f}); window.draw(BatchS);
            btnBatch.setCharacterSize(24); btnBatch.setPosition({30.f, 135.f}); window.draw(btnBatch);
        } else if(currentScreen == Screen::BatchMenu) {
            BatchTitle.setCharacterSize(50); BatchTitle.setPosition({0.f, 0.f}); window.draw(BatchTitle);
            BackS.setPosition({0.f, 80.f}); window.draw(BackS);
            BatchBack.setCharacterSize(24); BatchBack.setPosition({30.f, 75.f}); window.draw(BatchBack);
            IconCacheS.setPosition({0.f, 110.f}); window.draw(IconCacheS);
            IconCacheEdit.setCharacterSize(24); IconCacheEdit.setPosition({30.f, 105.f}); window.draw(IconCacheEdit);
            SystemFixS.setPosition({0.f, 140.f}); window.draw(SystemFixS);
            SystemFixEdit.setCharacterSize(24);SystemFixEdit.setPosition({30.f, 135.f}); window.draw(SystemFixEdit);
        } else if(currentScreen == Screen::AboutMenu) {
            AboutTitle.setCharacterSize(50); AboutTitle.setPosition({0.f, 0.f}); window.draw(AboutTitle);
            BackS.setPosition({0.f, 80.f}); window.draw(BackS);
            AboutBack.setCharacterSize(24); AboutBack.setPosition({30.f, 75.f}); window.draw(AboutBack);
            AboutAuthor.setCharacterSize(24); AboutAuthor.setPosition({0.f, 105.f}); window.draw(AboutAuthor);
            AboutVersion.setCharacterSize(24); AboutVersion.setPosition({0.f, 135.f}); window.draw(AboutVersion);
        }
        window.display();
    }
    return 0;
}