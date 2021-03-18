#include <iostream>
#include "../system.h"
#include "../sampler.h"
#include "../GDsampler.h"
#include "../metropolis_langevin.h"
#include "../particle.h"
#include "../WaveFunctions/wavefunction.h"
#include "../WaveFunctions/simplegaussian.h"
#include "../WaveFunctions/numericgaussian.h"
#include "../WaveFunctions/complexfunction.h"

#include "../Hamiltonians/hamiltonian.h"
#include "../Hamiltonians/harmonicoscillator.h"
#include "../Hamiltonians/ellipticoscillator.h"
#include "../InitialStates/randomuniform2.h"

#include "../Math/random.h"
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
    // Seed for the random number generator
    int seed = 2021;

    int numberOfDimensions  = 3;
    int numberOfParticles   = 10;
    int numberOfSteps       = (int) 1e5;
    double omega            = 25;          // Oscillator frequency.
    double alpha            = 0.5;          // Variational parameter.
    double beta             = 2.82843;
    double a                = 0.0043;
    double stepLength       = 0.1;          // Metropolis step length.
    double equilibration    = 0.1;          // Amount of the total steps used

    if (argc>1){
    try{
        numberOfDimensions=atoi(argv[1]);
        numberOfParticles   = atoi(argv[2]);
        numberOfSteps   = atoi(argv[3]);
        omega= atoi(argv[4]);
        alpha=atof(argv[5]);
        stepLength=atof(argv[6]);
        equilibration=atof(argv[7]);
        a=atof(argv[8]);
        beta=atof(argv[9]);
        seed=atoi(argv[10]);
    }
    catch (int e)
    {
      cout << "An exception occurred. Exception Nr. " << e << '\n';
    }
    }
    System* system = new MetropolisLangevin(seed);
    system->setSampler               (new Sampler(system));
    system->setOmega(omega);
    system->setHamiltonian              (new EllipticOscillator(system, omega));
    system->setWaveFunction             (new ComplexFunction(system, alpha, beta, a));
    system->setInitialState             (new RandomUniformMinDist(system, numberOfDimensions, numberOfParticles,a));
    system->setEquilibrationFraction    (equilibration);
    system->setStepLength               (stepLength);
    system->setMetropolisSteps          (numberOfSteps);
    system->runMetropolisSteps       (numberOfSteps,true);
    return 0;
}
