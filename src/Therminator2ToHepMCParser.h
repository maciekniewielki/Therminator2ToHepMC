#ifndef _Therminator2ToHepMCParser_H_
#define _Therminator2ToHepMCParser_H_

//Standard libraries
#include <iostream>
#include <string>
#include <fstream>

//HepMC libraries
#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"

class Therminator2ToHepMCParser
{
  public:
    Therminator2ToHepMCParser();
    Therminator2ToHepMCParser(std::string inputFileName, std::string outputFileName, int eventsToRead);
       
    void Run();
  private:
    
    bool initParser();
    void fillParticle();

    std::string inputFileName;
    std::string outputFileName;
    int eventsToRead;

    std::ifstream inputStream;

    struct CurrentParticle
    {
        int eid;
        int fathereid;
        int pid;
        int fatherpid;
        int rootpid;
        bool decayed;
        double mass;
        double energy;
        double px;
        double py;
        double pz;
        double t;
        double x;
        double y;
        double z;
    };

    CurrentParticle _particle;
    int _particlesInEvent;
    HepMC::IO_GenEvent* _HepMC_writer;
    
};

#endif
