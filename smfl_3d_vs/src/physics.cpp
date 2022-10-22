#include "physics.hpp"
#include <SFML/System.hpp>
#include "player.hpp"
#include "world.hpp"

//--------------------------------------------------------------------------------------------------
static const double g_physicsTimeScale = 1.0e-6;
static const double g_physicsMaxTimeStep = 1.0 / 32.0;
static const double g_physicsMinTimeStep = 1.0 / 1024.0;

static sf::Clock *g_physicsClock;
//static D3DXVECTOR3 GravityAcceleration(0,-9.80665f,0);
//static bool Paused;
static double g_physicsTimeNew;
static double g_physicsTimeOld;
static double g_physicsTimeTotal;
static bool g_isPaused;
//static unsigned int SimPointIdx;

//-------------------------------------------------------------------------------------------------
void Physics::Begin()
{
  g_physicsClock = new sf::Clock();
  g_physicsTimeOld = g_physicsClock->getElapsedTime().asMicroseconds() * g_physicsTimeScale;

  simulateInit();
}

//-------------------------------------------------------------------------------------------------
void Physics::update()
{
  double physicsTimeStep = 0.0;

  g_physicsTimeNew = g_physicsClock->getElapsedTime().asMicroseconds() * g_physicsTimeScale;
  physicsTimeStep = g_physicsTimeNew - g_physicsTimeOld;

  if (physicsTimeStep < g_physicsMinTimeStep)
  {
    return;
  }

  g_physicsTimeOld = g_physicsTimeNew;

  if (g_isPaused)
  {
    return;
  }

  if (physicsTimeStep > g_physicsMaxTimeStep)
  {
    physicsTimeStep = g_physicsMaxTimeStep;
  }

  Player::computeMatricesFromInputs(physicsTimeStep);

  double physicsTimeStepHalf = physicsTimeStep * 0.5;

  simulate(physicsTimeStep, physicsTimeStepHalf);
  World::Refresh(physicsTimeStep, physicsTimeStepHalf);


  g_physicsTimeTotal += physicsTimeStep;
}

//-------------------------------------------------------------------------------------------------
void Physics::simulateInit()
{
}

//-------------------------------------------------------------------------------------------------
void Physics::simulate(double timeStep, double timeStepHalf)
{
  //World::Refresh(TimeStep, TimeStepHalf);

/*
    for (int i=0;i<BOXES_COUNT;i++) Box[i].NullForces();
		for (i=0;i<BOXES_COUNT;i++)
{
Box[i].Linear.Force.y-=(0.981f/SYNC_PHYSICSTAKTS)*Box[i].Linear.Mass;
Box[i].Linear.Force.x-=0.01f*Box[i].Linear.Velocity.x;
Box[i].Linear.Force.y-=0.01f*Box[i].Linear.Velocity.y;
Box[i].Linear.Force.z-=0.01f*Box[i].Linear.Velocity.z;
Box[i].Angular.Force.x-=0.1f*Box[i].Angular.Velocity.x;
Box[i].Angular.Force.y-=0.1f*Box[i].Angular.Velocity.y;
Box[i].Angular.Force.z-=0.1f*Box[i].Angular.Velocity.z;
Box[i].CollidePlane(&FloorPlane);
//for (int j=i+;j<BOXES_COUNT;j++)
//	Box[i].CollideBox(&Box[j]);
Box[i].Step();
}
*/

}

//-------------------------------------------------------------------------------------------------
void Physics::enable(bool isPaused)
{
  g_physicsClock->restart();
  g_isPaused = isPaused;
}

//-------------------------------------------------------------------------------------------------
void Physics::End()
{
  delete g_physicsClock;
}

