```

  This configuration shows a simple setup of a TrafficGen (CPU) and an
  external TLM port for SystemC co-simulation

  Base System Architecture:
  +-------------+  +-----+    ^
  | System Port |  | CPU |    |
  +-------+-----+  +--+--+    |
          |           |       | gem5 World
          |      +----+       | (see this file)
          |      |            |
  +-------v------v------------------------------+    |
  |  system.caches.sys_port_proxy      Membus1       |    v
  +----------------+----------------------------+    External Port (see sc_slave_port.*)
                   |          
                   |          
  +----------------------------------------------------------------+
  |       |  sequencers0   |                                       |
  |       +----------------+                                       |
  |                                                                |
  |  system.caches.controllers0                                    |
  |                                                                |
  |                                                                |
  |     forward       request      response          response      |
  |     FromDir        ToDir     FromDirOrSibling   ToDirOrSibling |
  +----------------------------------------------------------------+
           ^              |             ^              |
           |              v             |              v                                              External Port (see sc_slave_port.*)
       +-------+      +-------+     +-------+      +-------+              | TLM World
       |  TLM  |      |  TLM  |     |  TLM  |      |  TLM  |              | (see sc_target.*)
       +-------+      +-------+     +-------+      +-------+              v
           |              |             |              |                  ^
       +-------+      +-------+     +-------+      +-------+              | TLM World
       |  TLM  |      |  TLM  |     |  TLM  |      |  TLM  |              | (see sc_initiator.*)
       +-------+      +-------+     +-------+      +-------+              v
           ^              |             ^              |                  |
           |              v             |              v                  |
  +------------------------------------------------------------+
  |   | master[0]       slave[0]       master[1]      slave[1]  |                 MessageBuffer
  |   |                 controllers0                            |
  |   +---------------------------------------------------------+
  |                         |
  |                         |
  |   +---------------------------------------------------------+
  |   |                  ext_node
  |   |   ext_links0
  |   |                  int_node
  |   +---------------------------------------------------------+
  |
  |  network
  |   +------------------------------------------+
  |   |   routers0                               |
  |   |   port_buffers00   ....  port_buffers13  |
  |   +------------------------------------------+
  |
  |
  +------------------------------------------------------------+
  |   |                 controllers2                            |
  |   | master[4]       slave[4]       master[5]      slave[5]  |                 MessageBuffer
  +------------------------------------------------------------+
           |              |             ^              |
           v              v             |              v                                              External Port (see sc_slave_port.*)
       +-------+      +-------+     +-------+      +-------+              | TLM World
       |  TLM  |      |  TLM  |     |  TLM  |      |  TLM  |              | (see sc_target.*)
       +-------+      +-------+     +-------+      +-------+              v
           |              |             |              |                  ^
       +-------+      +-------+     +-------+      +-------+              | TLM World
       |  TLM  |      |  TLM  |     |  TLM  |      |  TLM  |              | (see sc_initiator.*)
       +-------+      +-------+     +-------+      +-------+              v
           |              |             ^              |                  |
           v              v             |              v                  |
  +----------------------------------------------------------------+
  |   | request  |   |response |   | response |   | forward |      |
  |   | FromCache|   | ToCache |   | FromCache|   | ToCache |      |
  |   +----------+   +---------+   +----------+   +---------+      |
  |                                                                |
  |  system.caches.controllers2                                    |
  |                                                                |
  |               Memory : system.mem_ctrl.port                    |
  |               directory: system.caches.controllers.directory   |
  |    +----------------+                                          |
  |    |                |    response                              |
  |    |  sequencers0   |   FromMemory       Directory             |
  +----------------------------------------------------------------+
 




```
