#include <iostream>
#include <SDL3/SDL.h>
#include <cstdio>
#include "Window.h"
#include "GamepadManager.h"
#include "GUI.h"

int main(int argc, char **argv) {
    // 1. Initialize SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) == false) {
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    Window window("Gamepad Tester", 1280, 800);

    //
    GamepadManager gamepad;
    GUI gui(window, gamepad);

    // --- State Variables ---
    bool running = true;
    SDL_Event event;
    // Main Application Loop
    while (running) {
        // Events
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            gamepad.passEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        // Update
        gui.newFrame();
        gui.updateUI();

        // Draw
        SDL_SetRenderDrawColor(window.getRenderer(), 30, 30, 30, 255);

        // Clear the buffer
        SDL_RenderClear(window.getRenderer());


        gui.render();                          // Draw UI on top
        SDL_RenderPresent(window.getRenderer()); // Swap buffers
    }

    return 0;
}
