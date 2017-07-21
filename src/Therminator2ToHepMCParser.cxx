#include "Therminator2ToHepMCParser.h"

using namespace std;
using namespace HepMC;

Therminator2ToHepMCParser::Therminator2ToHepMCParser():_HepMC_writer(0)
{
    
}

Therminator2ToHepMCParser::Therminator2ToHepMCParser(string inputFileName, string outputFileName, int eventsToRead):_HepMC_writer(0)
{
    this->inputFileName = inputFileName;
    this->outputFileName = outputFileName;
    this->eventsToRead = eventsToRead;
}

void Therminator2ToHepMCParser::Run()
{
    const double fm_to_mm = 1.e-12;

    if(!initParser())
        return;

    string currentLine;
    string temp;

    int events = 0;
    
    /*while(true)
    {
        //inputStream>>currentLine;
        getline(inputStream, currentLine);
        if(currentLine.length()>0)
            cout<<currentLine<<endl;
    }*/
    
    cout<<"Reading "<<eventsToRead<<" events..."<<endl;
    for(int i=0; i<eventsToRead; i++)
    {

        inputStream.open(inputFileName.c_str());

        if(!inputStream.good())
        {
            cerr<<"Error, could not open file "<<inputFileName<<endl;
            return;
        }

        getline(inputStream, currentLine);  //Skip first line of event header
        //cout<<"First line: "<<currentLine<<endl;

        inputStream>>temp>>_particlesInEvent;   //Get number of particles
        getline(inputStream, currentLine);  //Skip the rest of the line
        //cout<<"Rest of line: "<<currentLine<<endl;
        
        cout<<"Event "<<events<<": Number of particles in event: "<<_particlesInEvent<<endl;


        GenEvent parsed_event(Units::GEV, Units::MM);
        
        parsed_event.weights().push_back(1.);

        // step 3) - add heavy ion information
        HeavyIon heavyion(  // add the information to the heavy-ion header. for fields that are
                            // not available in ampt, we provide -1, which is obviously nonsensical
            -1,                                     // Number of hard scatterings
            -1,                                     // Number of projectile participants
            -1,                                     // Number of target participants
            -1,                                     // Number of NN (nucleon-nucleon) collisions
            -1,                                     // Number of N-Nwounded collisions
            -1,                                     // Number of Nwounded-N collisons
            -1,                                     // Number of Nwounded-Nwounded collisions
            -1,                                     // Number of spectator neutrons
            -1,                                     // Number of spectator protons
            -1,                                     // Impact Parameter(fm) of collision
            0.,                                     // Azimuthal angle of event plane
            -1.,                                    // eccentricity of participating nucleons
                                                            //in the transverse plane 
                                                            //(as in phobos nucl-ex/0510031) 
            -1.);                                   // nucleon-nucleon inelastic 
                                                            //(including diffractive) cross-section
        parsed_event.set_heavy_ion(heavyion);
        

        // step 4) add two `artificial' beam particles. the hepmc reader in aliroot expects two beam particles
        // (which in hepmc have a special status) ,so we need to create them. they will not 
        // end up in the analysis, as they have only longitudinal momentum. 
        // the particles are produced at 0,0,0, the same production vertex that we'll later use for all
        // final state particles as well
        GenVertex* v = new GenVertex();
        parsed_event.add_vertex(v);

        FourVector beam1(0,0,.14e4, 1686.2977);
        GenParticle* bp1 = new GenParticle(beam1, 2212, 2);
        bp1->set_generated_mass(940);
        v->add_particle_in(bp1);

        FourVector beam2(0,0,.14e4, 1686.2977);
        GenParticle* bp2 = new GenParticle(beam2, 2212, 2);
        bp2->set_generated_mass(940);
        v->add_particle_in(bp2);

        // hepmc does not seem to provide for more than 1 beam particle ...
        parsed_event.set_beam_particles(bp1,bp2);

        for(int i=0; i<_particlesInEvent; i++)
        {
            fillParticle();

            FourVector fourmom(
                    _particle.px*fm_to_mm,
                    _particle.py*fm_to_mm, 
                    _particle.pz*fm_to_mm, 
                    _particle.energy);
            GenParticle* particle = new GenParticle(fourmom, _particle.pid, 1);
            particle->set_generated_mass(_particle.mass);

            // step 5c) connect the particle to its production vertex
            // all particles connect to the original beam vertex. this isn't strictly correct (and
            // ampt actually stores the freezeout vertex of particles), we do need however to have a 
            // 'mother-daughter' relationship for all particles .... 
              v->add_particle_out(particle);
        }
        
        getline(inputStream, currentLine);  //Skip last line of particles
        //cout<<"Last particle line: "<<currentLine<<endl;
        getline(inputStream, currentLine);  //Skip last line of particles
        _HepMC_writer->write_event(&parsed_event);
        events++;
        
        inputStream.close();
    }

    cout<<"End. Events parsed: "<<events<<endl;
        
}

bool Therminator2ToHepMCParser::initParser()
{
    /*
    inputStream.open(inputFileName.c_str());

    if(!inputStream.good())
    {
        cerr<<"Error, could not open file "<<inputFileName<<endl;
        return false;
    }
    */

    _HepMC_writer = new IO_GenEvent(outputFileName);
    if(!_HepMC_writer)
    {
        cerr<<"Error, could not initalize hepmc writer"<<endl;
        return false;
    }

    return true;
}

void Therminator2ToHepMCParser::fillParticle()
{
   inputStream>>_particle.eid>>_particle.fathereid>>_particle.pid>>_particle.fatherpid;
   inputStream>>_particle.rootpid>>_particle.decayed>>_particle.mass>>_particle.energy;
   inputStream>>_particle.px>>_particle.py>>_particle.pz>>_particle.t;
   inputStream>>_particle.x>>_particle.y>>_particle.z;
}
