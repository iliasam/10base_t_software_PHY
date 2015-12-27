`timescale 1ns / 1ps

module phy_tx_module(
    input clk_in,
    input MII_TX_EN,
    input [3:0] MII_TXD,
	 output MII_TX_CLK,//2.5 mhz
    output TX_p,
    output TX_n
    );
	 
//clk_in - 80mhz

reg tx_clk = 1'd0;//10mhz
reg mii_tx_clk_reg = 'd0;//2.5mhz
assign MII_TX_CLK = mii_tx_clk_reg;

//this counters are only for clk generation
reg [3:0] clk_cnt = 'd0;//ethernet clk counter
reg [1:0] clk_cnt2 = 'd0;//MII_TX_CLK counter

reg [3:0] in_data_latch = 4'd0;
reg mii_tx_clk_edge = 1'd0;//2.5 mhz
reg serial_data = 1'd0;

reg tx_enable = 1'd0;//latched MII_TX_EN

wire manch_data = (serial_data^tx_clk) & tx_enable;//manchester data


// generate the NLP
//counter is 20 bit length
reg [19:0] link_pulse_count = 'd0; 
always @(posedge clk_in) link_pulse_count <= tx_enable ? 20'd0 : link_pulse_count + 20'd1;
reg link_pulse = 'd0; 
always @(posedge clk_in) link_pulse <= &link_pulse_count[19:3];

// generate the TP_IDL
reg [5:0] idlecount = 'd33; // enough bits to count 3 bit-times (8*3 = 24)
always @(posedge clk_in) 
	if (tx_enable) idlecount<=0; else if(~&idlecount) idlecount<=idlecount+5'd1;
wire idle_pulse = (~tx_enable) & (idlecount<24);//idle_pulse must not lock data, so it's disabled during tx time


wire tx_data2 = link_pulse | manch_data | idle_pulse;//include additional link data
wire tx_en2 = tx_enable | link_pulse | idle_pulse;//include additional link data

assign TX_p =  tx_en2? tx_data2: 1'd0;
assign TX_n =  tx_en2? ~tx_data2: 1'd0;

//80 div 8 = 10
//10 div 4 = 2.5
always@(posedge clk_in)
begin
	if (clk_cnt > 'd6)
	begin
		clk_cnt = 'd0;
		if (clk_cnt2 > 'd2) clk_cnt2 = 'd0; 
		else clk_cnt2 = clk_cnt2 + 2'd1;//2+1=3, and (3) will be hold for a tick
	end
	else clk_cnt = clk_cnt + 4'd1;//6+1=7, and (7) will be hold for a tick
	
	if (clk_cnt < 'd3) tx_clk<= 'd1;//10 mhz
	else tx_clk<= 'd0;
	
	if (clk_cnt2 < 'd2) mii_tx_clk_reg<='d1;//2.5 mhz
	else mii_tx_clk_reg<='d0;
	
	mii_tx_clk_edge = ((clk_cnt2 == 'd0) && (clk_cnt == 'd0))? 1'd1: 1'd0;//2.5 mhz
	
	if (mii_tx_clk_edge == 1'd1) 
	begin
		in_data_latch = MII_TXD;//capture data from input
		tx_enable <= MII_TX_EN;
		serial_data <= MII_TXD[0];//first bit
	end
	else if (clk_cnt == 'd0)
	begin
		in_data_latch = {1'd0,in_data_latch[3:1]};
		serial_data <= in_data_latch[0];
	end
end


endmodule
