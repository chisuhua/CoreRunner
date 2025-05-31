from m5.objects import Gem5ToTlmBridge32, TlmToGem5Bridge32, TLM_Wrapper

class ring:
    def __init__(self, system, scname, name_prefix="chain", length=2, parent=None):
        """
        创建一个 TLM 链式模块
        :param system: gem5 的 system 对象
        :param name_prefix: 模块前缀名（如 chain0）
        :param length: 链条长度（即包含几个 passthrough 模块）
        :param parent: 如果传入，则作为所有组件的父对象
        """

        self.system = system
        self.scname = scname
        self.name_prefix = name_prefix
        self.length = length
        self.parent = parent if parent else system

        self.chain_nodes = []

        self._build_chain()

    def _build_chain(self):
        prev_down_tlm = None

        for i in range(self.length):
            name = f"{self.name_prefix}_{i}"

            # 创建组件
            wrapper = TLM_Wrapper(scname=f"{self.scname}")
            up_bridge = Gem5ToTlmBridge32()
            down_bridge = TlmToGem5Bridge32()

            # 加入 hierarchy（绑定到 system 或 parent）
            setattr(self.system, f"{name}_wrapper", wrapper)
            setattr(self.system, f"{name}_up_bridge", up_bridge)
            setattr(self.system, f"{name}_down_bridge", down_bridge)

            # 保存节点信息
            node = {
                "name": name,
                "wrapper": wrapper,
                "up_bridge": up_bridge,
                "down_bridge": down_bridge,
            }
            self.chain_nodes.append(node)

            # 设置连接关系
            if i == 0:
                pass
                #up_bridge.gem5 = self.system.cpu.port  # 第一个接 CPU
            else:
                up_bridge.gem5 = prev_down_gem5  # 后续接前一个的输出

            up_bridge.tlm = wrapper.tsck
            down_bridge.tlm = wrapper.isck

            # 保存当前输出供下一个使用
            prev_down_gem5 = down_bridge.gem5

    def get_gem5_port(self):
        """返回第一个模块的 gem5 port（用于连接外部）"""
        return self.chain_nodes[0]["up_bridge"].gem5

    def get_tlm_out(self):
        """返回最后一个模块的 tlm 输出"""
        return self.chain_nodes[-1]["down_bridge"].tlm
