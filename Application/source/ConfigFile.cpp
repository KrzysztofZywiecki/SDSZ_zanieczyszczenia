#include "ConfigFile.h"


ConfigFile::ConfigFile(const char* filename)
{
    char line[256];
    file.open(filename);
    if(file.bad())
    {
        throw std::runtime_error("Failed to open config file");
    }

    float realSize;
    file.getline(line, 256);
    sscanf(line, "%f", realSize);

    file.getline(line, 256);
    sscanf(line, "%u", size);
    unitLength = realSize / float(size);

    file.getline(line, 256);
    sscanf(line, "%u", diffSteps);

    file.getline(line, 256);
    sscanf(line, "%u", windSteps);

    file.getline(windFile, 256);
    file.getline(startFilename, 256);
    file.getline(overlay, 256);
}

Pollutant ConfigFile::ReadNextPollutant()
{
    char line[256];
    Pollutant pollutant;
    if(file.rdstate() & std::ifstream::eofbit)
    {
        pollutant.x = -1;
        pollutant.y = -1;
        pollutant.emission = -1;
    }
    else
    {
        file.getline(line, 256);
        sscanf(line, "%f %f %f", pollutant.x, pollutant.y, pollutant.emission);
    }
    return pollutant;
}