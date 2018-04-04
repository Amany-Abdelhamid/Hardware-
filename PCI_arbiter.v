module arbiter(clk,REQ,GNT,FRAME,IRDY,TRDY);

input clk,FRAME,IRDY,TRDY;
input wire [7:0] REQ;
output reg [7:0] GNT=8'b11111111;
reg [2:0] stack [0:20];
integer i,j=0,k,u,q;


wire idle;
assign idle = FRAME & IRDY ;


always  @ (posedge clk or REQ)
begin
if(idle )           
   begin 
   for(i=0;i<8;i=i+1)
    begin
       if(REQ[i]==1'b0)
         begin 
        @ (negedge clk)
         GNT[i]<=1'b0;
           k<=i;
           u<=i;
         end
     end
   end
end


always @ (posedge clk or REQ)
begin
for(i=0;i<8;i=i+1)
begin
if ((REQ[i]==1'b0) && i!=k)
begin
 k<=i;
stack[j]=i;
j=j+1;  
end
end
end

always @ (posedge clk or REQ)
begin
for(i=0;i<8;i=i+1)
begin
if(REQ[i]==1'b1)
GNT[i]<=1'b1;
end
end

always @ (posedge clk or FRAME)
begin
if(~FRAME)
  begin @(negedge clk) 
    // GNT[u]<=1'b1; 
    GNT=8'b11111111;
    GNT[stack[0]]<=1'b0;
       j=j-1;
   for(q=0;q<21;q=q+1)
   stack[q]=stack[q+1];
  end
end


endmodule
