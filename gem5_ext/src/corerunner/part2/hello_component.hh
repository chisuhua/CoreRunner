#ifndef __CORERUNNER_HELLO_COMPONENT_HH__
#define __CORERUNNER_HELLO_COMPONENT_HH__

#include "params/HelloComponent.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class HelloComponent : public SimObject
{
  private:
    void processEvent();
    EventWrapper<HelloComponent, &HelloComponent::processEvent> event;


    const Tick latency;

    int timesLeft;

  public:
    HelloComponent(const HelloComponentParams &p);

    void startup() override;
};

} // namespace gem5

#endif // __CORERUNNER_HELLO_COMPONENT_HH__
