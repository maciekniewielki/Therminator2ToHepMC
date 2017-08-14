//Therminator2ToHepMCParser
//Written by Maciej Buczynski
//Inspired by AmptToHepMCParser by Redmer Alexander Bertens

#include "Therminator2ToHepMCParser.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::string;

void printUsage(char* progName)
{
    printf("Converts Therminator2 text output file to the HepMC format. Usage:\n");
    printf("%s <input_file> <output_file> <number_of_events_to_read>\n", progName);
}

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    string firstArg = argv[1];
    if(firstArg == "-h" || firstArg == "--help" || argc<3)
    {
        printUsage(argv[0]);
        return 1;
    }
    
    string inputFileName = firstArg;
    string outputFileName = argv[2];
    int eventsToRead = atoi(argv[3]);

    Therminator2ToHepMCParser* parser = new Therminator2ToHepMCParser(inputFileName, outputFileName, eventsToRead);
    parser->Run();
    
    return 0;
}
