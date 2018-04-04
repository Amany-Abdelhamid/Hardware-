module RAM(A1,Di1,Do1,rd1,wr1);
input [19:0] A1;           //for 1MB long Memory..32bit word
input [31:0] Di1;          //for 32bit word
output [31:0] Do1;
input rd1,wr1;
reg [31:0] Mem [0:1048575];       //for 4MB RAM
integer i;
initial                           //filling RAM
begin
for(i=0;i<1048576;i=i+1)
Mem[i]=i;
end
always @ (wr1)
if(wr1==1)
Mem[A1]=Di1;
assign Do1=(rd1)?Mem[A1]:32'bz;
endmodule

module Cache_Controller(A2,Di2,Do2,rd2,wr2,whit,rhit,NS);
input [19:0] A2;           //Address of RAM
input [31:0] Di2;          //for 32bit word
output [31:0] Do2;
wire [1:0] offset;      //for 4B word
wire [9:0] index;       //for 1KB long cache
wire [7:0] tag;
output whit,rhit; reg hitw,hitr;         //for write hit and read hit     
input rd2,wr2;
reg [31:0] Mem [0:1023];       //for 4KB cache without valid and tag columns
assign offset=A2[1:0],index=A2[11:2],tag=A2[19:12];
reg [0:7] tagc [0:1023]; reg validc [0:1023] ;     //tag column and valid column
wire [31:0] c; RAM x(A2,Di2,c,rd2,wr2);           //for write through and no write allocate

integer y;
initial
begin
for(y=0 ; y <1024  ; y=y+1)
validc[y]=0;
end
 input NS;           //in order to enter always block each time validc[index] changes
always @ (wr2 or NS)     //write through for hit and no write allocate for miss
begin
if(wr2 && !validc[index])
begin
tagc[index]=tag;
Mem[index]=Di2;
validc[index]=1;
hitw=1;                   //for write hit
end
else if(wr2 && validc[index])
hitw=0;                  //for write miss
end

always @ (rd2 or NS)             //direct mapped Cache
begin
if(rd2 && ( !validc[index] || (tag!=tagc[index])))
begin
Mem[index]=c;
validc[index]=1;
tagc[index]=tag;
hitr=0;
end
else if(rd2 && validc[index] && (tag==tagc[index]))
hitr=1;
end
assign Do2=(rd2)?Mem[index] :32'bz;
assign rhit=(rd2)?hitr:1'bz;
assign whit=(wr2)?hitw:1'bz;
endmodule

module Controller_tb();
reg [31:0] di; reg [19:0] a; reg r,w,ns; wire [31:0] do; wire rh,wh;
Cache_Controller q(a,di,do,r,w,wh,rh,ns);
initial
begin
$monitor("(A=%d) ----- (Di=%d) ------- (Do=%d) ------- (write= %b) --- (read= %b) ---- (writehit= %b) --- (readhit= %b)",a,di,do,w,r,wh,rh);
a=100; w=1; di=10; r=0; ns=0;        //write hit and index=25 in cache is filled  //Do=zzz
#5
di=2; ns=1;                          //write miss because location[25]=index in cache is datafull(valid=1)  //Do=zzz
#5
a=4396; r=1; ns=2; w=0;                //read miss as index=25 has different tag=0 &this tag=1 but valid=1   //Do=4396 from RAM
#5
ns=3; a=100;  w=0;  r=1;              //read hit index=25 has data=10           //Do=10
#5
ns=4; a=2348;                       //read miss as diff index=587 &same tag=0 but valid=0   //Do=2348  from RAM
#5
ns=5;  a=5000;                     //read miss as tag#tag & valid=0     ///Do=5000 from RAM

end
endmodule
