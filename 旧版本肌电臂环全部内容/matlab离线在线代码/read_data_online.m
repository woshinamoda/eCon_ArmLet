%���ߣ���ͨ����
%�����ʣ�500Hz
%���ݸ�ʽ����ͷ + ���ݳ��� + EMG���� +��IMU���ݣ�+У�� + �����
%BBAA + 1�ֽ� + 12�ֽ� +��6�ֽڣ�+ 1�ֽ� + 1�ֽ�
clear
clc

delete(instrfindall);
scom = 'COM36'; %name of serial
Baudrate = 460800; %serial baudrate, should be same as the STM32 setting
b = serial(scom);
b.InputBufferSize=2500;

set(b,'BaudRate',Baudrate);
fopen(b);
pause(1)
%%%%%%%%%%%%%%�޸Ĵ򿪴��ڵķ�ʽ_END %%%%%%%%%%%%%%%%%
EMG_CHANNEL=8;
emg_cnt_max=EMG_CHANNEL*1.5;
imu_cnt_max = 6; 

EMG_bytes=zeros(1,emg_cnt_max+imu_cnt_max);%%%��¼һ֡���ֽ�����
emg_cnt_state=0;%%%��¼switch������״̬
emg_cnt_count=0;%%%ÿ�μ�����emg_cnt_maxΪֹ
emg_idx=1;

EMG_frame=zeros(1,EMG_CHANNEL);%%%���ݽ����������֡
imu_idx=1;

IMU_CHANNEL=3;
IMU_frame=zeros(1,IMU_CHANNEL);%%%���ݽ����������֡

result_emg=zeros(2500,EMG_CHANNEL);%%%��ͼ���»�����
result_emg_idx=1;

result_imu=zeros(500,IMU_CHANNEL);
result_imu_idx=1;


fig=figure();
hold on;
for k=1:8
    subplot(5,2,k);
    line_EMG{k}=plot((1:size(result_emg,1))/500,result_emg(:,k));

    ylim([0,4096])
    xlim([0,size(result_emg,1)/500])
end

subplot(5,2,[9,10]);
line_IMU{1}=plot((1:size(result_imu,1))/100,result_imu(:,1),'b');%pitch��-90��,90��)%
hold on
line_IMU{2}=plot((1:size(result_imu,1))/100,result_imu(:,2),'g');%roll(-180��,180��)%
hold on
line_IMU{3}=plot((1:size(result_imu,1))/100,result_imu(:,3),'r');%yaw(-180��,180��)%
ylim([-18000,18000])
% xlim([0,size(result_imu,1)/100])

drawnow();
% ��ջ�����%
for i=1:10
    fread(b,1000,'uint8');
end


while true
    [buff,count]=fread(b,1000,'uint8');
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
                elseif(buff(index)==187)%�ų�0xBB BB AA�����
                    emg_cnt_state=1;
                else
                    emg_cnt_state=0;
                end
            case 2
                if((buff(index) == emg_cnt_max) || (buff(index) == emg_cnt_max + imu_cnt_max)) 
                    datalen = buff(index);%%������ĳ��ȵ���12��18
                    EMG_bytes=zeros(1,datalen);%%�������֡
                    emg_cnt_state = 3;
                else
                    emg_cnt_state = 0;
                end                
            case 3
                EMG_bytes(1,emg_cnt_count) = buff(index);
                emg_cnt_count = emg_cnt_count + 1;
                emg_sumchkm = emg_sumchkm + buff(index);
                if(emg_cnt_count == datalen+1)%%�ж����ݽ����Ƿ����
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
                EMG_Sequence(emg_idx,1) = buff(index);%%%��¼�����
                % ԭʼ���ݽ���%
                % EMG%

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
                emg_idx = emg_idx + 1;
                emg_cnt_state=0;  % switch״̬�л�%
                % IMU%
                if(datalen == emg_cnt_max + imu_cnt_max)
                    for j=1:3
                        IMU_frame(1,j)=typecast(uint8(EMG_bytes(j*2-1+12:j*2+12)),'int16');
                    end
                    IMU(imu_idx,1:3)=IMU_frame;
                    imu_idx = imu_idx + 1;
                    % ��ͼ%
                    result_imu(result_imu_idx,:)=IMU_frame;
                    if(result_imu_idx == 500)
                        result_imu = ones(500,IMU_CHANNEL);
                    end
                    result_imu_idx = mod(result_imu_idx,500)+1;                    
                end
    
                % ��ͼ%
                result_emg(result_emg_idx,:)=EMG_frame;
                if result_emg_idx == 2500
                    result_emg=zeros(2500,EMG_CHANNEL);
                end
                result_emg_idx=mod(result_emg_idx,2500)+1;
               
        end
    end
   
    for k=1:8
        set(line_EMG{k},'YData',result_emg(:,k));
    end
    
    set(line_IMU{1},'YData',result_imu(:,1));
    set(line_IMU{2},'YData',result_imu(:,2));
    set(line_IMU{3},'YData',result_imu(:,3));

    drawnow(); 
     
end

