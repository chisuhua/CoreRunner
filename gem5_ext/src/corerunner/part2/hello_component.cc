#include "corerunner/part2/hello_component.hh"
#include "base/trace.hh"
#include "debug/Component.hh"

#include <iostream>

namespace gem5
{

HelloComponent::HelloComponent(const HelloComponentParams &params) :
    SimObject(params),// event([this]{processEvent();}, name()),
    event(*this),
    latency(params.time_to_wait),
    timesLeft(params.number_of_fires)
{
    DPRINTF(Component, "Hello World! From a SimObject!");
}

void HelloComponent::processEvent()
{
    timesLeft--;
    DPRINTF(Component, "Hello world! Processing the event! %d left\n", timesLeft);

    if (timesLeft <= 0) {
        DPRINTF(Component, "Done firing!\n");
    } else {
        schedule(event, curTick() + latency);
    }
}

void HelloComponent::startup()
{
    schedule(event, latency);
}

} // namespace gem5
