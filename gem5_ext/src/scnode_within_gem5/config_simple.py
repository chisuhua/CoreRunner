import m5
import os
from m5.objects import *
from m5.defines import buildEnv
from m5.util import fatal
from ring import ring

# 创建系统
system = System()

# 设置时钟域
system.clk_domain = SrcClockDomain(clock='1GHz', voltage_domain=VoltageDomain())

# 生成 TrafficGen 配置文件
config_path = os.path.join(os.getcwd(), "trafficgen.cfg")
with open(config_path, "w") as f:
    f.write("STATE 0 10000000 RANDOM 65 0 16777216 8 5000 15000 0\n")
    f.write("STATE 1 10000000 LINEAR 65 0 16777216 64 5000 15000 0\n")
    f.write("STATE 2 10000000 IDLE\n")
    f.write("INIT 0\n")
    f.write("TRANSITION 0 1 0.5\n")
    f.write("TRANSITION 0 2 0.5\n")
    f.write("TRANSITION 1 0 0.5\n")
    f.write("TRANSITION 1 2 0.5\n")
    f.write("TRANSITION 2 0 0.5\n")
    f.write("TRANSITION 2 1 0.5\n")

system.cpu = TrafficGen(config_file=config_path)

# 创建 SimpleMemory 实例（作为目标内存）
system.physmem = (SimpleMemory(range=AddrRange(1 * 1024 * 1024 * 1024)))  # 1 GB 内存

# 将 TrafficGen 连接到 SimpleMemory
if False:
    system.cpu.port = system.physmem.port
elif False:
    system.target = TLM_Target(scname="ScMemory")
    system.transactor = Gem5ToTlmBridge32()
    system.transactor.gem5 = system.cpu.port
    system.transactor.tlm = system.target.tsck
elif True:
    ring = ring(system, scname="Passthrough", length=2)
    ring.chain_nodes[0]["up_bridge"].gem5 = system.cpu.port
    ring.chain_nodes[-1]["down_bridge"].gem5 = system.physmem.port
elif False:
    tlm_ring = ring(system, name_prefix="ring0", length=3)
    tlm_ring.get_tlm_out = system.physmem.port
else:
    system.wrapper = TLM_Wrapper(scname="Passthrough")
    system.g2t_passthrough = Gem5ToTlmBridge32()
    system.t2g_passthrough = TlmToGem5Bridge32()

    # Connect everything:
    system.g2t_passthrough.gem5 = system.cpu.port
    system.g2t_passthrough.tlm = system.wrapper.tsck

    system.t2g_passthrough.tlm = system.wrapper.isck
    if True:
        system.t2g_passthrough.gem5 = system.physmem.port
    else:
        system.target = TLM_Target(scname="ScMemory")
        system.transactor = Gem5ToTlmBridge32()
        system.transactor.gem5 = system.t2g_passthrough.gem5
        system.transactor.tlm = system.target.tsck



# 设置系统内存模式和地址范围
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange(1 * 1024 * 1024 * 1024)]

kernel = SystemC_Kernel(system=system)
# 设置 root 对象
#root = Root(full_system=False, system=system)
root = Root(full_system=False, systemc_kernel=kernel)

# 启动模拟
m5.instantiate()

print("Beginning simulation!")
#exit_event = m5.simulate(100000000)  # 模拟 10,000 ticks
#print(f'Exiting @ tick {m5.curTick()} because {exit_event.getCause()}')

cause = m5.simulate(m5.MaxTick).getCause()
print(cause)
