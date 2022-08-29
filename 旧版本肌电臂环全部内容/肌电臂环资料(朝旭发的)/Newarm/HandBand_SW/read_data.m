
clear
clc

delete(instrfindall);
scom = 'COM3'; %name of serial
Baudrate = 250000; %serial baudrate, should be same as the STM32 setting
b = serial(scom);
b.InputBufferSize=2500;

set(b,'BaudRate',Baudrate);
fopen(b);
pause(1)
%%%%%%%%%%%%%%修改打开串口的方式_END %%%%%%%%%%%%%%%%%
EMG_CHANNEL=8;
emg_cnt_max=12;
data_buff=zeros(1,EMG_CHANNEL);
emg_cnt_state=0;
emg_cnt_count=0;
result_emg=zeros(2500,EMG_CHANNEL);
result_emg_idx=1;
%%%%%%%%%%%%%%%%%%50Hz陷波器%%%%%%%%%%%%%%%%%%%%%%%%
% fs = 500; fo = 50;  q = 35; bw = (fo/(fs/2))/q;
% [c,a] = iircomb(fs/fo,bw,'notch');

fig=figure();
hold on;
for k=1:8
    subplot(4,2,k);
    line_EMG{k}=plot((1:size(result_emg,1))/500,result_emg(:,k));

    ylim([0,4096])
    xlim([0,size(result_emg,1)/500])
end

drawnow();

while true
    [buff,count]=fread(b,1000,'uint8');
    for index=1:length(buff)
        switch(emg_cnt_state)
            case 0
                if(buff(index)==85) %0x55
                    emg_cnt_state=1;
                end
            case 1
                if(buff(index)==170) %0xAA
                    emg_cnt_state=2;
                    emg_cnt_count=0;
                else
                    emg_cnt_state=0;
                end
            case 2
                temp_idx = fix(mod(emg_cnt_count, emg_cnt_max)/3)*2 + 1;
                if(mod(emg_cnt_count,3) == 0)
                    data_buff(temp_idx)=buff(index)*16;
                elseif (mod(emg_cnt_count, 3) == 1)
                    data_buff(temp_idx) =  data_buff(temp_idx) + double(bitshift(uint8(buff(index)),-4));
%                     data_buff(temp_idx) =  filter(c, a, data_buff(temp_idx)) - 2048;
                    data_buff(temp_idx + 1) = double(bitand(buff(index), 15)*256);
                else
                    data_buff(temp_idx + 1)=data_buff(temp_idx + 1)+buff(index);
%                     data_buff(temp_idx + 1) =  filter(c, a, data_buff(temp_idx + 1)) - 2048;
                end
                emg_cnt_count=emg_cnt_count+1;
                if(emg_cnt_count==emg_cnt_max)
                    emg_cnt_state=3;
                end
            case 3
                result_emg(result_emg_idx,:)=data_buff(1:EMG_CHANNEL);
                if result_emg_idx == 2500
                    result_emg=zeros(2500,EMG_CHANNEL);
                end
                result_emg_idx=mod(result_emg_idx,2500)+1;
                emg_cnt_state=0;
        end
    end
   
    for k=1:8
        set(line_EMG{k},'YData',result_emg(:,k));
    end

    drawnow(); 
     
end

