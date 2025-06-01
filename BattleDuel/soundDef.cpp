//
// Created by Michał on 01.06.2025.
//
#include<iostream>
#include"sound.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


    void playWav(const char* filename) {
        ma_result result;
        ma_engine engine;

        result = ma_engine_init(NULL, &engine);  // Domyślna konfiguracja
        if (result != MA_SUCCESS) {
            std::cerr << "Nie udało się zainicjować silnika audio.\n";
            return;
        }

        result = ma_engine_play_sound(&engine, filename, NULL);
        if (result != MA_SUCCESS) {
            std::cerr << "Nie udało się odtworzyć pliku: " << filename << "\n";
            ma_engine_uninit(&engine);
            return;
        }

        std::cout << "Odtwarzanie: " << filename << "\n";
        ma_sleep(1000);  // Czekaj 2 sekundy (lub dłużej, jeśli potrzeba)

        ma_engine_uninit(&engine);
    }