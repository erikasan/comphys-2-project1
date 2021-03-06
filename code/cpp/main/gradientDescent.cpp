#include <iostream>
#include <string>
#include "../system.h"
#include "../particle.h"
#include "../metropolis_langevin.h"
#include "../sampler.h"
#include "../GDsampler.h"
#include "../WaveFunctions/wavefunction.h"
#include "../WaveFunctions/simplegaussian.h"
#include "../WaveFunctions/numericgaussian.h"
#include "../Hamiltonians/hamiltonian.h"
#include "../Hamiltonians/harmonicoscillator.h"
#include "../InitialStates/initialstate.h"
#include "../InitialStates/randomuniform.h"
#include "../Math/random.h"
#include "../WaveFunctions/complexfunction.h"
#include "../Hamiltonians/ellipticoscillator.h"
#include "../InitialStates/randomuniform2.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Seed for the random number generator
    int seed = 2020;

    int numberOfDimensions = 3;
    int numberOfParticles  = 10;
    int numberOfSteps      = (int) 1e6;
    double alpha           = 0.5;          // Variational parameter.
    double beta             = 2.82843;
    double a                = 0.0043;
    double stepLength      = 0.1;          // Metropolis step length.
    int equilibration      = (int) 1e5;          // Amount of the total steps used
    string wF_type         = "HO"; // HO for Harmonic Oscillator, EO for Elliptic Oscillator, "NHO" for numerical harmonic oscillator
    string sampler_type    = "VMC"; //VMC for Brute Force, IMP for Importance sampling
    string filename_blocking = "no";  // Legacy code, nothing is written to file anywasy
    System* system;
    string path= "../../../output/";
    double tol = 0.0000001;
    int maxIter = 200;
    double learningRate = 0.01;
    if (argc>=11){
          numberOfDimensions = atoi(argv[1]);
          numberOfParticles  = atoi(argv[2]);
          numberOfSteps      = atoi(argv[3]);
          alpha              = atof(argv[4]);
          stepLength         = atof(argv[5]);
          equilibration      = atoi(argv[6]);
          seed               = atoi(argv[7]);
          wF_type            = argv[8];
          sampler_type       = argv[9];
          filename_blocking  = argv[10];
          if(argc>=12){
            path               = argv[11];
          }
          if(argc>=15){
            maxIter=atof(argv[12]);
            learningRate=atof(argv[13]);
            tol=atof(argv[14]);
          }
    }
    if (sampler_type.compare("VMC")==0){
      system = new System(seed);
    }
    else if (sampler_type.compare("IMP")==0){
      system = new MetropolisLangevin(seed);
    }
    else{
      cout << "Not a legal sampler type" << endl;
      return 1;
    }
    system->m_energyfile=filename_blocking;
    system->setPath(path); //Change this is you want the output somewhere else

    system->setSampler               (new Sampler(system));
    system->setOmega(25);
    if(wF_type.compare("HO")==0){
      system->setHamiltonian           (new HarmonicOscillator(system, 25));
      system->setWaveFunction          (new SimpleGaussian(system, alpha));
      system->setInitialState          (new RandomUniform(system, numberOfDimensions, numberOfParticles));
    }
    else if(wF_type.compare("NHO")==0){
      system->setHamiltonian           (new HarmonicOscillator(system, 25));
      system->setWaveFunction          (new NumericGaussian(system, alpha));
      system->setInitialState          (new RandomUniform(system, numberOfDimensions, numberOfParticles));
    }
    else if(wF_type.compare("EO")==0){
      system->setHamiltonian              (new EllipticOscillator(system, 25));
      system->setWaveFunction             (new ComplexFunction(system, alpha, beta, a));
      system->setInitialState             (new RandomUniformMinDist(system, numberOfDimensions, numberOfParticles,a));
    }
    else{
      cout << "Not a legal Oscillator type" << endl;
      return 2;
    }
    system->setEquilibrationSteps (equilibration);
    system->setStepLength            (stepLength);
    system->setMetropolisSteps       (numberOfSteps);

    system->gradientDescent(tol, learningRate, maxIter);
    alpha=system->getWaveFunction()->getParameters()[0];
    cout << "The ideal value for alpha is "<<alpha << endl;
    return 0;
}
