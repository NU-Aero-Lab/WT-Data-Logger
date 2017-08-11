#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Interface.h>

void Interface::setSampleRate()
{
    printf("Sample Rate: ");
    std::cin >> sampleRate;
}

void Interface::setSamplePerChan()
{
    printf("Samples Per Channel: ");
    std::cin >> samplePerChan;
}

int Interface::getSampleRate()
{
    return sampleRate;
}

int Interface::getSamplePerChan()
{
    return samplePerChan;
}
