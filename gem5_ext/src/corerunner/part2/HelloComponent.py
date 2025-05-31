from m5.params import *
from m5.SimObject import SimObject

class HelloComponent(SimObject):
    type = 'HelloComponent'
    cxx_header = "corerunner/part2/hello_component.hh"
    cxx_class = "gem5::HelloComponent"

    time_to_wait = Param.Latency("Time before firing the event")
    number_of_fires = Param.Int(1, "Number of times to fire the event before "
                                   "goodbye")
