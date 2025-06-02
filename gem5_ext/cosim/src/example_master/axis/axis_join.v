
`timescale 1 ns / 1 ps

	module axis_join #
	(
		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer AXIS_DATA_WIDTH	= 32
		// parameter integer AXI_ADDR_WIDTH	= 29
	)
	(
		input wire  s_tvalid,
		output wire s_tready,
		input wire [AXIS_DATA_WIDTH-1 : 0] s_tdata,
		input wire [(AXIS_DATA_WIDTH/8)-1 : 0] s_tstrb,
		// input wire [AXISignal(USER_WIDTH)] s_tuser,
		input wire  s_tlast,

		output wire  m_tvalid,
		input wire m_tready,
		output wire [AXIS_DATA_WIDTH-1 : 0] m_tdata,
		output wire [(AXIS_DATA_WIDTH/8)-1 : 0] m_tstrb,
		// input wire [AXISignal(USER_WIDTH)] s_tuser,
		output wire  m_tlast
	);
		assign   s_tready = m_tready;
		assign   m_tvalid = s_tvalid;
		assign   m_tdata = s_tdata;
		assign   m_tstrb = s_tstrb;
		assign   m_tlast = s_tlast;

	endmodule
