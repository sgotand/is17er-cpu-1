`default_nettype none


module uart_loopback #(parameter UART_ADDR = 32'h4060_0000) (
	input wire  clk,    // Clock
		input wire triger,
		input wire read_state,
		input wire read_data,
		input wire write_data,
	input wire rst_n,  // Asynchronous reset active lo

	// AXI4-lite master memory interface
    output wire                      axi_awvalid,
    input  wire                       axi_awready,
    output wire [31:0]              	 axi_awaddr,
    output wire [2:0]                 axi_awprot,

    output wire                      axi_wvalid,
    input  wire                       axi_wready,
	output wire [31:0]                axi_wdata,
    output wire [3:0]                 axi_wstrb,

    input  wire                       axi_bvalid,
    output wire                      axi_bready,
    input  wire [1:0]                 axi_bresp,

    output wire                      axi_arvalid,
    input wire                       axi_arready,
    output wire [31:0]                axi_araddr,
    output wire [2:0]                 axi_arprot,

    input wire                       axi_rvalid,
    output wire                       axi_rready,
	input wire [31:0]       		axi_rdata,
    input wire [1:0]                 axi_rresp
);



	uart_sv #(
			.base_addr(base_addr)
			) uart_sv (
			.clk         (clk),
			.triger      (triger),
			.read_state  (read_state),
			.read_data   (read_data),
			.write_data  (write_data),
			.rst_n       (rst_n),
			.axi_awvalid (axi_awvalid),
			.axi_awready (axi_awready),
			.axi_awaddr  (axi_awaddr),
			.axi_awprot  (axi_awprot),
			.axi_wvalid  (axi_wvalid),
			.axi_wready  (axi_wready),
			.axi_wdata   (axi_wdata),
			.axi_wstrb   (axi_wstrb),
			.axi_bvalid  (axi_bvalid),
			.axi_bready  (axi_bready),
			.axi_bresp   (axi_bresp),
			.axi_arready (axi_arready),
			.axi_arprot  (axi_arprot),
			.axi_rvalid  (axi_rvalid),
			.axi_rdata   (axi_rdata),
			.axi_rresp   (axi_rresp)
		);



endmodule


`default_nettype wire
