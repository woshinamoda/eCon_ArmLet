%作者：念通智能
%采样率：500Hz
%数据格式：包头 + 数据长度 + EMG数据 +（IMU数据）+校验 + 包序号
%BBAA + 1字节 + 12字节 +（6字节）+ 1字节 + 1字节
clear
clc

filename = 'emgtest.txt';
data=textread(filename,'%2c');
buff=hex2dec(data(:,:));
%%%%%%%%%%%%%%修改打开串口的方式_END %%%%%%%%%%%%%%%%%
EMG_CHANNEL=8;
emg_cnt_max=EMG_CHANNEL*1.5;
imu_cnt_max = 6; 

EMG_bytes=zeros(1,emg_cnt_max+imu_cnt_max);%%%记录一帧的字节数据
emg_cnt_state=0;%%%记录switch函数的状态
emg_cnt_count=0;%%%每次计数到emg_cnt_max为止
emg_idx=1;

EMG_frame=zeros(1,EMG_CHANNEL);%%%数据解析后的数据帧
imu_idx=1;


for index=1:length(buff)
    switch(emg_cnt_state)
        case 0
            if(buff(index)==187) %0xBB
                emg_cnt_state=1;
            else 
                emg_cnt_state=0;
            end
        case 1
            if(buff(index)==170) %0xAA
                emg_cnt_state=2;
                emg_cnt_count=1;
                emg_sumchkm = 0;
            elseif(buff(index)==187)%排除0xBB BB AA的情况
                emg_cnt_state=1;
            else
                emg_cnt_state=0;
            end
        case 2
            if((buff(index) == emg_cnt_max) || (buff(index) == emg_cnt_max + imu_cnt_max)) 
                datalen = buff(index);%%数据域的长度等于12或18
                EMG_bytes=zeros(1,datalen);%%清空数据帧
                emg_cnt_state = 3;
            else
                emg_cnt_state = 0;
            end        
            
        case 3
            EMG_bytes(1,emg_cnt_count) = buff(index);
            emg_cnt_count = emg_cnt_count + 1;
            emg_sumchkm = emg_sumchkm + buff(index);
            if(emg_cnt_count == datalen+1)%%判断数据接收是否结束
                emg_cnt_state=4;
            else
                emg_cnt_state=3;
            end    
        case 4
            if(mod(emg_sumchkm,256) == buff(index))
                emg_cnt_state=5;
            else
                emg_cnt_state=0;
            end
        case 5
            EMG_Sequence(emg_idx,1) = buff(index);%%%记录包序号
            % 原始数据解析%
            %EMG%
            
            for i=1:12
                temp_idx = fix(mod(i-1, 12)/3)*2 + 1;
                if(mod(i,3) == 1)
                    EMG_frame(temp_idx) = EMG_bytes(i)*16;
                elseif (mod(i, 3) == 2)
                    EMG_frame(temp_idx) =  EMG_frame(temp_idx) + double(bitshift(uint8(EMG_bytes(i)),-4));
                    EMG_frame(temp_idx + 1) = double(bitand(EMG_bytes(i), 15)*256);
                else
                    EMG_frame(temp_idx + 1)=EMG_frame(temp_idx + 1)+EMG_bytes(i);
                end                
            end            
            EMG(emg_idx,1:8)=EMG_frame;
            %IMU%
            if(datalen == emg_cnt_max + imu_cnt_max)
                for j=1:3
                    IMU(imu_idx,j)=typecast(uint8(EMG_bytes(j*2-1+12:j*2+12)),'int16');
                end
                imu_idx = imu_idx + 1;
            end
                        
            emg_idx = emg_idx + 1;
            emg_cnt_state=0;
            
    end
end

ZZ = diff(EMG_Sequence);
   

