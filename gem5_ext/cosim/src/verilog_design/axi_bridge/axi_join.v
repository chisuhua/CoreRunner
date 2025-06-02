
`timescale 1 ns / 1 ps

module axi_join #
(
        parameter integer AXI_ADDR_WIDTH       = 32,
        parameter integer AXI_DATA_WIDTH       = 32,
        parameter integer AXI_ID_WIDTH         = 16,
        parameter integer AXI_AXLEN_WIDTH      = 8,
        parameter integer AXI_AXLOCK_WIDTH     = 2,
        parameter integer AXI_AWUSER_WIDTH     = 2,
        parameter integer AXI_WUSER_WIDTH      = 2,
        parameter integer AXI_BUSER_WIDTH      = 2,
        parameter integer AXI_ARUSER_WIDTH     = 2,
        parameter integer AXI_RUSER_WIDTH      = 2
)
(
    //Clock and reset
    input                                      axi_aclk,
    input                                      axi_aresetn,

    //S_AXI_USR
    input [AXI_ID_WIDTH-1:0]                 s_axi_awid ,
    input [AXI_ADDR_WIDTH-1:0]               s_axi_awaddr ,
    input [AXI_AXLEN_WIDTH-1:0]                s_axi_awlen ,
    input [2:0]                                s_axi_awsize ,
    input [1:0]                                s_axi_awburst ,
    input [AXI_AXLOCK_WIDTH-1:0]               s_axi_awlock ,
    input [3:0]                                s_axi_awcache ,
    input [2:0]                                s_axi_awprot ,
    input [3:0]                                s_axi_awqos ,
    input [3:0]                                s_axi_awregion,
    input [AXI_AWUSER_WIDTH-1:0]               s_axi_awuser ,
    input                                      s_axi_awvalid ,
    output                                     s_axi_awready ,
    input [AXI_DATA_WIDTH-1:0]                 s_axi_wdata ,
    input [(AXI_DATA_WIDTH/8)-1:0]             s_axi_wstrb ,
    input                                      s_axi_wlast ,
    // input [AXI_ID_WIDTH-1:0]                   s_axi_wid ,
    input [AXI_WUSER_WIDTH-1:0]                s_axi_wuser ,
    input                                      s_axi_wvalid ,
    output                                     s_axi_wready ,
    output [AXI_ID_WIDTH-1:0]                  s_axi_bid ,
    output [1:0]                               s_axi_bresp ,
    output [AXI_BUSER_WIDTH-1:0]               s_axi_buser ,
    output                                     s_axi_bvalid ,
    input                                      s_axi_bready ,
    input [AXI_ID_WIDTH-1:0]                 s_axi_arid ,
    input [AXI_ADDR_WIDTH-1:0]               s_axi_araddr ,
    input [AXI_AXLEN_WIDTH-1:0]                s_axi_arlen ,
    input [2:0]                                s_axi_arsize ,
    input [1:0]                                s_axi_arburst ,
    input [AXI_AXLOCK_WIDTH-1:0]               s_axi_arlock ,
    input [3:0]                                s_axi_arcache ,
    input [2:0]                                s_axi_arprot ,
    input [3:0]                                s_axi_arqos ,
    input [3:0]                                s_axi_arregion,
    input [AXI_ARUSER_WIDTH-1:0]             s_axi_aruser ,
    input                                      s_axi_arvalid ,
    output                                     s_axi_arready ,
    output [AXI_ID_WIDTH-1:0]                s_axi_rid ,
    output [AXI_DATA_WIDTH-1:0]              s_axi_rdata ,
    output [1:0]                               s_axi_rresp ,
    output                                     s_axi_rlast ,
    output [AXI_RUSER_WIDTH-1:0]             s_axi_ruser ,
    output                                     s_axi_rvalid ,
    input                                      s_axi_rready ,


    output [AXI_ID_WIDTH-1:0]                  m_axi_awid,
    output [AXI_ADDR_WIDTH-1:0]                m_axi_awaddr,
    output [AXI_AXLEN_WIDTH-1:0]               m_axi_awlen,
    output [2:0]                               m_axi_awsize,
    output [1:0]                               m_axi_awburst,
    output [AXI_AXLOCK_WIDTH-1:0]              m_axi_awlock,
    output [3:0]                               m_axi_awcache,
    output [2:0]                               m_axi_awprot,
    output [3:0]                               m_axi_awqos,
    output [3:0]                               m_axi_awregion,
    output [AXI_AWUSER_WIDTH-1:0]              m_axi_awuser,
    output                                     m_axi_awvalid,
    input                                      m_axi_awready,
    output [AXI_DATA_WIDTH-1:0]                m_axi_wdata,
    output [(AXI_DATA_WIDTH/8)-1:0]            m_axi_wstrb,
    output                                     m_axi_wlast,
    // output [AXI_ID_WIDTH-1:0]                  m_axi_wid ,
    output [AXI_WUSER_WIDTH-1:0]               m_axi_wuser,
    output                                     m_axi_wvalid,
    input                                      m_axi_wready,
    input [AXI_ID_WIDTH-1:0]                   m_axi_bid,
    input [1:0]                                m_axi_bresp,
    input [AXI_BUSER_WIDTH-1:0]                m_axi_buser,
    input                                      m_axi_bvalid,
    output                                     m_axi_bready,
    output [AXI_ID_WIDTH-1:0]                  m_axi_arid,
    output [AXI_ADDR_WIDTH-1:0]                m_axi_araddr,
    output [AXI_AXLEN_WIDTH-1:0]               m_axi_arlen,
    output [2:0]                               m_axi_arsize,
    output [1:0]                               m_axi_arburst,
    output [AXI_AXLOCK_WIDTH-1:0]              m_axi_arlock,
    output [3:0]                               m_axi_arcache,
    output [2:0]                               m_axi_arprot,
    output [3:0]                               m_axi_arqos,
    output [3:0]                               m_axi_arregion,
    output [AXI_ARUSER_WIDTH-1:0]            m_axi_aruser,
    output                                     m_axi_arvalid,
    input                                      m_axi_arready,
    input [AXI_ID_WIDTH-1:0]                 m_axi_rid,
    input [AXI_DATA_WIDTH-1:0]               m_axi_rdata,
    input [1:0]                                m_axi_rresp,
    input                                      m_axi_rlast,
    input [AXI_RUSER_WIDTH-1:0]              m_axi_ruser,
    input                                      m_axi_rvalid,
    output                                     m_axi_rready
    );

    assign	m_axi_awid	=	s_axi_awid;
    assign	m_axi_awaddr	=	s_axi_awaddr;
    assign	m_axi_awlen	=	s_axi_awlen;
    assign	m_axi_awsize	=	s_axi_awsize;
    assign	m_axi_awburst	=	s_axi_awburst;
    assign	m_axi_awlock	=	s_axi_awlock;
    assign	m_axi_awcache	=	s_axi_awcache;
    assign	m_axi_awprot	=	s_axi_awprot;
    assign	m_axi_awqos	=	s_axi_awqos;
    assign	m_axi_awregion	=	s_axi_awregion;
    assign	m_axi_awuser	=	s_axi_awuser;
    assign	m_axi_awvalid	=	s_axi_awvalid;
    assign	s_axi_awready	=	m_axi_awready;
    assign	m_axi_wdata	=	s_axi_wdata;
    assign	m_axi_wstrb	=	s_axi_wstrb;
    assign	m_axi_wlast	=	s_axi_wlast;
    assign	m_axi_wuser	=	s_axi_wuser;
    // assign	s_axi_wid	=	m_axi_wid;
    assign	m_axi_wvalid	=	s_axi_wvalid;
    assign	s_axi_wready	=	m_axi_wready;
    assign	s_axi_bid	=	m_axi_bid;
    assign	s_axi_bresp	=	m_axi_bresp;
    assign	s_axi_buser	=	m_axi_buser;
    assign	s_axi_bvalid	=	m_axi_bvalid;
    assign	m_axi_bready	=	s_axi_bready;
    assign	m_axi_arid	=	s_axi_arid;
    assign	m_axi_araddr	=	s_axi_araddr;
    assign	m_axi_arlen	=	s_axi_arlen;
    assign	m_axi_arsize	=	s_axi_arsize;
    assign	m_axi_arburst	=	s_axi_arburst;
    assign	m_axi_arlock	=	s_axi_arlock;
    assign	m_axi_arcache	=	s_axi_arcache;
    assign	m_axi_arprot	=	s_axi_arprot;
    assign	m_axi_arqos	=	s_axi_arqos;
    assign	m_axi_arregion	=	s_axi_arregion;
    assign	m_axi_aruser	=	s_axi_aruser;
    assign	m_axi_arvalid	=	s_axi_arvalid;
    assign	s_axi_arready	=	m_axi_arready;
    assign	s_axi_rid	=	m_axi_rid;
    assign	s_axi_rdata	=	m_axi_rdata;
    assign	s_axi_rresp	=	m_axi_rresp;
    assign	s_axi_rlast	=	m_axi_rlast;
    assign	s_axi_ruser	=	m_axi_ruser;
    assign	s_axi_rvalid	=	m_axi_rvalid;
    assign	m_axi_rready	=	s_axi_rready;

endmodule
