%m-file to adquire ERIKA-FLEX FULL data from the serial port using RS232 

clear all
close all

number_of_samples=2000;                  %set the number of adquisitions
delete(instrfind)                       %clean all open ports
                                        %configuring serial port
s=serial('/dev/ttyS0');                 %creates a matlab object from the serial port 'COM1' 
s.BaudRate=115200;                      %baudrate=115200bps
s.Parity='none';                        %no parity
s.DataBits=8;                           %data sended in 8bits format
s.StopBits=1;                           %1 bit to stop
s.FlowControl='none';                   %no flowcontrol
s.Terminator='LF';                      %LineFeed character as terminator
s.Timeout=10;                           %maximum time in seconds since the data is readed
s.InputBufferSize=100000;               %increment this value when data is corrupted
fopen(s)                                %open serial port object
q = quantizer('float',[32 8]);          %cast 32bits hex to float

disp('Waiting for data... ')

%Read data until header character(0x01) is received 
data=zeros(9,1);
while(data~=1)
    data=fread(s,1,'char');
end
data=fread(s,9-1,'char');

disp('Beginning data adquisition... ')
figure;
for n = 1:number_of_samples  %get data and plot 
    data=fread(s,9,'char');
    t(n,1)= hex2dec([dec2hex(data(2),2) dec2hex(data(3),2) ...
            dec2hex(data(4),2) dec2hex(data(5),2)])*1e-3;
    c(n,1)=hex2num(q,[dec2hex(data(9),2) dec2hex(data(8),2) ...
	    dec2hex(data(7),2) dec2hex(data(6),2)]);%joint four bytes, float value
    plot(t,c);
    axis([max(t)-10 max(t) 0 9]);%
    grid on;
    drawnow; 
end

