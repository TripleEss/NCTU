imgN=100;
imgW=512;
imgH=512;
waW=imgW/8;
waH=imgH/8;
waSize=waW*waH;

% load image
img=zeros(imgW,imgH,imgN);
for i=1:imgN
    str=sprintf('%d.tif',i);
    img(:,:,i)=imread(str);
end

% blind embedding
rng(1);
wr=rand(8,8);
wr=wr-mean(wr(:));
wr=wr/std(wr(:));
alpha=1;
zlcthreshold=0.7;

cw_block=zeros(imgW,imgH,imgN,2);
zcc_null=zeros(imgN,1);
zcc_block=zeros(imgN,2);
for i=1:imgN
    v_null=zeros(8,8);
    v_block=zeros(8,8,2);
    for px=1:imgW
        for py=1:imgH
            % null
            v_null(mod(px,8)+1,mod(py,8)+1)=v_null(mod(px,8)+1,mod(py,8)+1)+img(px,py,i);
            % block
            cw_block(px,py,i,1)=img(px,py,i)-alpha*wr(mod(px,8)+1,mod(py,8)+1);  
            v_block(mod(px,8)+1,mod(py,8)+1,1)=v_block(mod(px,8)+1,mod(py,8)+1,1)+cw_block(px,py,i,1);
            cw_block(px,py,i,2)=img(px,py,i)+alpha*wr(mod(px,8)+1,mod(py,8)+1); 
            v_block(mod(px,8)+1,mod(py,8)+1,2)=v_block(mod(px,8)+1,mod(py,8)+1,2)+cw_block(px,py,i,2);
        end
    end
    % null
    v_null=v_null/(waSize);
    r=corrcoef(v_null,wr);
    zcc_null(i)=r(1,2);
    % block
    v_block=v_block/(waSize);
    r=corrcoef(v_block(:,:,1),wr);
    zcc_block(i,1)=r(1,2);
    r=corrcoef(v_block(:,:,2),wr);
    zcc_block(i,2)=r(1,2);
end

% plot
figure;
histogram(zcc_null,'BinWidth',0.05,'FaceColor','g','FaceAlpha',0.5);hold on;
histogram(zcc_block(:,1),'BinWidth',0.05,'FaceColor','r','FaceAlpha',0.5);hold on;
histogram(zcc_block(:,2),'BinWidth',0.05,'FaceColor','b','FaceAlpha',0.5);hold on;
y=ylim;
plot([-zlcthreshold -zlcthreshold],[0 100],'k--',[zlcthreshold zlcthreshold],[0 100],'k--');hold off;
title('blocked blind embedding');
axis([-1.1 1.1 y(1) y(2)]);
xlabel('correlation coefficient value');
ylabel('number of images');
legend('{\itnull}','m = 0','m = 1','Location','northoutside','Orientation','horizontal');

clearvars i px py str imgN imgW imgH waW waH waSize r y;