#include "metropolis_langevin.h"
#include <cassert>
#include "sampler.h"
#include "GDsampler.h"
#include "particle.h"
#include "WaveFunctions/wavefunction.h"
#include "Hamiltonians/hamiltonian.h"
#include "InitialStates/initialstate.h"
#include "Math/random.h"
#include <chrono>
#include <cmath>



using namespace std::chrono;
using namespace std;

bool MetropolisLangevin::metropolisStep(){
  //Pick random Particle
  int particle_id = m_random->nextInt(m_numberOfParticles-1);
  double wf_old = m_waveFunction->evaluate(m_particles,particle_id);
  std::vector<double> position = m_particles[particle_id]->getPosition(); //old position
  std::vector<double> quantumForceOld = m_waveFunction->quantumForce(m_particles,particle_id);
  double adjustment = 0;

  for (int i = 0; i < m_numberOfDimensions; i++){
    adjustment = 0.5*quantumForceOld[i]*m_stepLength + m_random->nextGaussian(0,1)*m_stepLengthRoot;
    m_particles[particle_id]->adjustPosition(adjustment,i);
  }

  m_waveFunction->updateDistances(m_particles,particle_id);
  std::vector<double> newPosition = m_particles[particle_id]->getPosition();
  double wf_new = m_waveFunction->evaluate(m_particles,particle_id);
  std::vector<double> quantumForceNew = m_waveFunction->quantumForce(m_particles,particle_id);

  double green = 0;
  for (int j = 0; j < m_numberOfDimensions; j++){
    green += 0.5*(quantumForceOld[j] + quantumForceNew[j])*
            (0.5*m_stepLength*0.5*(quantumForceOld[j] - quantumForceNew[j]) - newPosition[j] + position[j]);
  }

  if ((wf_new*wf_new)/(wf_old*wf_old)*exp(green) > m_random->nextDouble() ){
    return true;
  }

  else{
    m_particles[particle_id]->setPosition(position);
    m_waveFunction->updateDistances(m_particles,particle_id);
  }

  return false;
}

void MetropolisLangevin::runMetropolisSteps(int numberOfMetropolisSteps, bool desire_output){
  m_particles               = m_initialState->getParticles();
  //m_sampler                 = new Sampler(this);
  m_numberOfMetropolisSteps = numberOfMetropolisSteps;

  m_waveFunction->initiateDistances(m_particles);
  m_sampler->setNumberOfMetropolisSteps(numberOfMetropolisSteps);

  auto start = high_resolution_clock::now();
  for (int i = 0; i < numberOfMetropolisSteps; i++){
      bool acceptedStep = metropolisStep();
      if (i > (int)(m_equilibrationFraction*numberOfMetropolisSteps)){
          m_sampler->sample(acceptedStep);
      }
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  m_duration = duration.count();

  m_sampler->computeAverages();

  if (desire_output){
    m_sampler->printOutputToTerminal();
    m_sampler->printOutputToFile();
  }
}