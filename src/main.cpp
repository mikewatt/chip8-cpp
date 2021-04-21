#include "SDL.h"
#include "Chip8.h"

#include <cstdint>
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: chip8 image_file" << std::endl;
        return 1;
    }
    const char *image_file = argv[1];

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow(
            "cppsdl",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            Display::WIDTH * 8, Display::HEIGHT * 8,
            SDL_WINDOW_OPENGL
    );
    if (window == nullptr) {
        std::cerr << "Failed to create window";
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer" << std::endl;
        return 1;
    }

    Mem memory;
    if (!memory.init_from_file(image_file)) {
        std::cerr << "Failed to read input file" << std::endl;
        return 1;
    }

    Display display;
    display.clear();

    Keypad keypad;

    Chip8 cpu(memory, display, keypad);

    // FIXME: improve/consolidate time accumulation and simulation
    float insns_to_simulate = 0.0;
    float time_acc = 0.0;
    auto sof_ticks = SDL_GetTicks();

    /* Main loop */
    // FIXME: factor render/input/simulate
    auto shutdown = false;
    while (!shutdown) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                shutdown = true;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym)
                {
                    case SDLK_4: keypad.key_down(0x1); break;
                    case SDLK_5: keypad.key_down(0x2); break;
                    case SDLK_6: keypad.key_down(0x3); break;
                    case SDLK_7: keypad.key_down(0xc); break;
                    case SDLK_r: keypad.key_down(0x4); break;
                    case SDLK_t: keypad.key_down(0x5); break;
                    case SDLK_y: keypad.key_down(0x6); break;
                    case SDLK_u: keypad.key_down(0xd); break;
                    case SDLK_f: keypad.key_down(0x7); break;
                    case SDLK_g: keypad.key_down(0x8); break;
                    case SDLK_h: keypad.key_down(0x9); break;
                    case SDLK_j: keypad.key_down(0xe); break;
                    case SDLK_c: keypad.key_down(0xa); break;
                    case SDLK_v: keypad.key_down(0x0); break;
                    case SDLK_b: keypad.key_down(0xb); break;
                    case SDLK_n: keypad.key_down(0xf); break;
                }
            }
            if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym)
                {
                    case SDLK_4: keypad.key_up(0x1); break;
                    case SDLK_5: keypad.key_up(0x2); break;
                    case SDLK_6: keypad.key_up(0x3); break;
                    case SDLK_7: keypad.key_up(0xc); break;
                    case SDLK_r: keypad.key_up(0x4); break;
                    case SDLK_t: keypad.key_up(0x5); break;
                    case SDLK_y: keypad.key_up(0x6); break;
                    case SDLK_u: keypad.key_up(0xd); break;
                    case SDLK_f: keypad.key_up(0x7); break;
                    case SDLK_g: keypad.key_up(0x8); break;
                    case SDLK_h: keypad.key_up(0x9); break;
                    case SDLK_j: keypad.key_up(0xe); break;
                    case SDLK_c: keypad.key_up(0xa); break;
                    case SDLK_v: keypad.key_up(0x0); break;
                    case SDLK_b: keypad.key_up(0xb); break;
                    case SDLK_n: keypad.key_up(0xf); break;
                }
            }
        }
        if (shutdown) {
            continue;
        }

        /* Simulate */
        auto current_ticks = SDL_GetTicks();
        auto ticks_to_simulate = current_ticks - sof_ticks;
        time_acc += ticks_to_simulate;
        sof_ticks = current_ticks;

        while (time_acc > (1000.0 / 60.0)) {
            cpu.timer_interrupt();
            time_acc -= (1000.0 / 60.0);
        }

        insns_to_simulate += 2.0 * ticks_to_simulate;
        int insns_simulated = 0;
        while (insns_simulated < insns_to_simulate) {
            cpu.step();
            ++insns_simulated;
        }
        insns_to_simulate -= insns_simulated;

        /* Render */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        {
            if (cpu.is_speaker_on()) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            }
            SDL_Rect rect;
            rect.w = 8;
            rect.h = 8;
            for (auto y = 0; y < Display::HEIGHT; ++y) {
                for (auto x = 0; x < Display::WIDTH; ++ x) {
                    if (display.is_on(x, y)) {
                        rect.x = x * 8;
                        rect.y = y * 8;

                        SDL_RenderFillRect(renderer, &rect);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
