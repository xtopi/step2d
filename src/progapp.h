#pragma once

#include <memory>

// This is the Application class (That is instantiated by the Program Class)
class TApplication {
public:
    TApplication() {
        quit = next = restart = zoomOut = zoomIn = viewnext = viewnextcycle = false;
        view = true;
    }
    int start(const std::vector<std::string> &args);

private:
    void on_input_up(const InputEvent &key);
    void on_window_close();

private:
    bool quit, next, view, viewnext, viewnextcycle, restart, zoomOut, zoomIn;
};
