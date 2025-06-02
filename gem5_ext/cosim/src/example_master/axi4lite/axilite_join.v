
`timescale 1 ns / 1 ps

	module axilite_join #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer AXI_DATA_WIDTH	= 32,
		parameter integer AXI_ADDR_WIDTH	= 32
	)
	(
		// Users to add ports here
		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,

		input wire [AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,

		output wire  s00_axi_awready,
		input wire [AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready,

		// Ports of Axi Slave Bus Interface m00_AXI
		// output wire  m00_axi_aclk,
		// output wire  m00_axi_aresetn,

		output wire [AXI_ADDR_WIDTH-1 : 0] m00_axi_awaddr,
		output wire [2 : 0] m00_axi_awprot,
		output wire  m00_axi_awvalid,
		input wire  m00_axi_awready,
		output wire [AXI_DATA_WIDTH-1 : 0] m00_axi_wdata,
		output wire [(AXI_DATA_WIDTH/8)-1 : 0] m00_axi_wstrb,
		output wire  m00_axi_wvalid,
		input wire  m00_axi_wready,
		input wire [1 : 0] m00_axi_bresp,
		input wire  m00_axi_bvalid,
		output wire  m00_axi_bready,
		output wire [AXI_ADDR_WIDTH-1 : 0] m00_axi_araddr,
		output wire [2 : 0] m00_axi_arprot,
		output wire  m00_axi_arvalid,
		input wire  m00_axi_arready,
		input wire [AXI_DATA_WIDTH-1 : 0] m00_axi_rdata,
		input wire [1 : 0] m00_axi_rresp,
		input wire  m00_axi_rvalid,
		output wire  m00_axi_rready

	);

		// assign   m00_axi_aclk = s00_axi_aclk;
		// assign   m00_axi_aresetn = s00_axi_aresetn;

		// aw
		assign   m00_axi_awvalid = s00_axi_awvalid;
		assign   s00_axi_awready = m00_axi_awready;
		assign   m00_axi_awaddr = s00_axi_awaddr;
		assign   m00_axi_awprot = s00_axi_awprot;

		// w
		assign   m00_axi_wdata = s00_axi_wdata;

		assign   m00_axi_wstrb = s00_axi_wstrb;
		/*
		wire     strb = |s00_axi_wstrb;
		assign   m00_axi_wstrb = {4{strb}};
		*/

		assign   m00_axi_wvalid = s00_axi_wvalid;
		assign   s00_axi_wready = m00_axi_wready;

		// w-resp
		assign   s00_axi_bvalid = m00_axi_bvalid;
		assign   m00_axi_bready = s00_axi_bready;
		assign   s00_axi_bresp = m00_axi_bresp;


		// ar
		assign   m00_axi_arvalid = s00_axi_arvalid;
		assign   s00_axi_arready = m00_axi_arready;
		assign   m00_axi_araddr = s00_axi_araddr;
		assign   m00_axi_arprot = s00_axi_arprot;

		assign   s00_axi_rvalid = m00_axi_rvalid;
		assign   m00_axi_rready = s00_axi_rready;
		assign   s00_axi_rdata = m00_axi_rdata;
		assign   s00_axi_rresp = m00_axi_rresp;

	endmodule
