`timescale 1ns / 1ps
module phy_rx_module(
    input clk_in,
	 input data_in,
	 output MII_RX_CLK,
    output MII_RX_EN,
    output [3:0] MII_RXD
    );
	 
reg mii_rx_clk_reg = 'd0;//2.5 mhz
reg [3:0] mii_rxd_reg = 'd0;//buffered MII_RXD value
reg mii_en_reg = 'd0;//buffered MII_RX_EN value

assign MII_RX_CLK = mii_rx_clk_reg;//2.5 MHz
assign MII_RXD = mii_rxd_reg;
assign MII_RX_EN = mii_en_reg;
	 
reg [4:0] clk_cnt = 5'd0;//used for 2.5 MHz generation
reg [3:0] data = 4'd0;//shift register
reg [3:0] data_latch = 4'd0;//internal received data

reg [4:0] fifo_buf [0:3];
reg [1:0] index = 2'd0;

//fifo is used to keep delta time between edges
initial
begin
	fifo_buf[0] = 'd0;
	fifo_buf[1] = 'd0;
	fifo_buf[2] = 'd0;
	fifo_buf[3] = 'd0;
end

reg [4:0] delta_cnt = 5'd0;//time betveen EDGES
wire ext_stop;//ext stop is extending stop_signal (protection from TP_IDLE part)
wire stop_signal = ((delta_cnt > 'd13)? 1'd1: 1'd0) | ext_stop;//high the end of packet
pulse_extend local_pulse_extend(.clk_in(clk_in), .pulse_in(stop_signal), .pulse_out(ext_stop));

reg new_data = 'd0;//buffered signal for MII_RX_EN

//80 div 24 = 2.5 MHz
//clk generation
always@(posedge clk_in)
begin
	if (clk_cnt > 'd30)
	begin
		clk_cnt = 'd0;
	end
	else clk_cnt = clk_cnt + 5'd1;//30+1=31, and (31) will be hold for a tick
	
	if (clk_cnt < 'd16) mii_rx_clk_reg<= 'd1;//10 mhz
	else mii_rx_clk_reg<= 'd0;
	
	if (clk_cnt == 'd0)
	begin
		mii_rxd_reg<= data_latch;//synchronize data
		mii_en_reg<= new_data;//synchronize enable
	end
end


reg [3:0] shift_reg = 4'd0;
always @(posedge clk_in) shift_reg <= {shift_reg[2:0], data_in};

wire [1:0] window = shift_reg[3:2];

reg pos_edge  = 1'd0;
reg neg_edge  = 1'd0;
wire any_edge  = pos_edge | neg_edge;

//edge detection part
always @(posedge clk_in)
begin
	if (window == 2'b01) pos_edge <= 1'd1;
	else if (window == 'b10) neg_edge<= 1'd1;
	else
	begin
		pos_edge<= 1'd0;
		neg_edge<= 1'd0;
	end
end

//fifo - store delay between edges
always @(posedge clk_in)
begin
	if (any_edge) 
	begin
		for(index = 2'd3; index > 2'd0; index = index - 2'd1)
      begin
			fifo_buf[index] = fifo_buf[index - 'd1];
		end
		fifo_buf[0] = delta_cnt + 5'd1;
		delta_cnt = 'd0;
	end
	else if (delta_cnt < 'd16) delta_cnt = delta_cnt + 'd1;
end


wire center = (((fifo_buf[0] == fifo_buf[2]) && (fifo_buf[1] == fifo_buf[3]) && (fifo_buf[0] > 0)) & any_edge)? 1'd1:1'd0;

reg [9:0] timeout2 = 'd40;//used to remove noise, NLP
wire capt_en = (timeout2 < 'd60)? 1'd1: 1'd0;//capture enabled
reg end_reset = 1'd0;//1 tick pulse at the end of the packet (used to reset pos_edge_cnt)


reg [9:0] timeout1 = 'd100;//clears at every signal pos edge
wire capt_en2 = (timeout1 < 'd99)? 1'd1: 1'd0;//capture enabled
always @(posedge clk_in)
begin
	if (pos_edge)//first edge in packet is pos
		timeout1<= 'd0;
	else if (capt_en2) timeout1 <= timeout1 +10'd1;//overflow protection
	
	end_reset <= (timeout1 == 'd90)? 1'd1: 1'd0;//value is buffered
end

reg [1:0] pos_edge_cnt = 'd0;//used for sync with right edge
always @(posedge clk_in)
begin
	if (pos_edge && (!capt_en2))//first edge in packet is pos
		pos_edge_cnt <= 'd0;
	else if (any_edge) pos_edge_cnt <= pos_edge_cnt + 2'd1;
	else if (end_reset) pos_edge_cnt <= 'd0;//additional reset at the end of pulse or packet
end


reg [3:0] symb_cnt = 4'd0;//bit time count (synchronized with signal edges)
always @(posedge clk_in)
begin
	if (center && (!capt_en) && (pos_edge_cnt == 'd2))//if capture disabled
	begin
		timeout2 <= 'd0;//reset at the begin of the packet
	end
	else if (capt_en && any_edge) timeout2 <= 'd0;
	else if (capt_en) timeout2 <= timeout2 + 10'd1;//overflow protection
	
	if (center && (!capt_en)) symb_cnt <= 4'd0;
	else if (symb_cnt < 4'd7) symb_cnt <= symb_cnt + 4'd1;
	else if (symb_cnt > 4'd6) symb_cnt <= 4'd0;
end

reg [1:0] bit_cnt = 2'd0;//number of received bits

//captures data from "data_in"
wire capture = capt_en && (symb_cnt == 'd5);
always @(posedge clk_in)
begin
	if (capture)
	begin
		data = {data_in,data[3:1]};
		bit_cnt = bit_cnt + 2'd1;
		if (bit_cnt == 2'd0) 
		begin
			data_latch = data;
			if (stop_signal == 1'd0)
				new_data <= 1'd1;
			else
				new_data <= 1'd0;
		end
	end
	else if (!capt_en) bit_cnt<= 2'd0;//reset at the end of the packet
end

endmodule

//find pos edge at "pulse_in" and create 30 ticks pulse at "pulse_out"
module pulse_extend(
    input clk_in,
	 input pulse_in,
	 output pulse_out
    );
reg [5:0] cnt = 6'd0;

reg [1:0] trig = 2'd0;
always@(posedge clk_in) trig <= {trig[0],pulse_in};
wire start_edge = (trig == 2'b01) ? 1'b1 : 1'b0;

assign pulse_out = (cnt < 'd30);

always @(posedge clk_in)
begin
	if (start_edge) cnt<= 6'd0;
	else if (pulse_out) cnt<= cnt+ 6'd1;
end

endmodule
