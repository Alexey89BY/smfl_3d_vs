#ifndef PHYSICS_HPP
#define PHYSICS_HPP


class Physics
{
  public:
    static void Begin();
    static void update();
    static void enable(bool isPaused);
    static void End();

    static void simulateInit(void);
    static void simulate(double timeStep, double timeStepHalf);

  protected:

  private:
};

#endif // PHYSICS_HPP
