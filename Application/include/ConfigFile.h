#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <conio.h>

struct Pollutant
{
    float x;
    float y;
    float emission;
    bool isValid() {return x != -1.0f;}
};

struct ConfigFile
{
    ConfigFile(const char* filename);

    Pollutant ReadNextPollutant();

    uint32_t size;
    uint32_t diffSteps;
    uint32_t windSteps;
    float unitLength;

    private:
    std::ifstream file;
    char windFile[256];
    char startFilename[256];
    char overlay[256];
};

struct WindFile
{
    WindFile(const char* filename);
    ~WindFile();
    bool isValid(){return data != nullptr;}
    void Close();

    uint32_t width;
    uint32_t height;
    float* data;
};