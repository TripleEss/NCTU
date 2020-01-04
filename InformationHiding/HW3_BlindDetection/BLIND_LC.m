imgN=100;
imgW=512;
imgH=512;
imgSize=imgW*imgH;

% load image
img=zeros(imgN,imgSize);
for i=1:imgN
    str=sprintf('%d.tif',i);
    image=imread(str);
    img(i,:)=reshape(image,1,imgSize);
end

% blind embedding
%rng(1);
wr=rand(1,imgSize);
wr=wr-mean(wr);
wr=wr/std(wr);
wr_lowpass=zeros(size(imgSize));
lowpassthreshold=min(wr(:))+(max(wr(:))-min(wr(:)))*0.7;
for i=1:size(wr,2)
    if abs(wr(i))<=lowpassthreshold
        wr_lowpass(i)=wr(i);
    else
        wr_lowpass(i)=sign(wr(i))*lowpassthreshold;
    end
end
alpha=1;
zlcthreshold=alpha*sum(wr(1,:).*wr(1,:))/imgSize*0.7;
beta=alpha*sum(wr(1,:).*wr(1,:))/imgSize*0.3;

cw_basic=zeros(imgN,imgSize,2);
cw_lowpass=zeros(imgN,imgSize,2);
cw_sideinfo=zeros(imgN,imgSize,2);
zlc_null=zeros(imgN,1);
zlc_basic=zeros(imgN,2);
zlc_lowpass=zeros(imgN,2);
zlc_sideinfo=zeros(imgN,2);
for i=1:imgN
    % null
    zlc_null(i)=sum(double(img(i,:)).*wr(1,:))/imgSize;
    % basic
    cw_basic(i,:,1)=img(i,:)-alpha*wr(1,:);
    zlc_basic(i,1)=sum(double(cw_basic(i,:,1)).*wr(1,:))/imgSize;
    cw_basic(i,:,2)=img(i,:)+alpha*wr(1,:);
    zlc_basic(i,2)=sum(double(cw_basic(i,:,2)).*wr(1,:))/imgSize;
    % low pass
    cw_lowpass(i,:,1)=img(i,:)-alpha*wr_lowpass(1,:);
    zlc_lowpass(i,1)=sum(double(cw_lowpass(i,:,1)).*wr_lowpass(1,:))/imgSize;
    cw_lowpass(i,:,2)=img(i,:)+alpha*wr_lowpass(1,:);
    zlc_lowpass(i,2)=sum(double(cw_lowpass(i,:,2)).*wr_lowpass(1,:))/imgSize;
    % side information
    alpha_sideinfo=(imgSize*(zlcthreshold+beta)+sum(double(img(i,:)).*wr(1,:)))/sum(wr(1,:).*wr(1,:));
    cw_sideinfo(i,:,1)=img(i,:)-alpha_sideinfo*wr(1,:);
    zlc_sideinfo(i,1)=sum(double(cw_sideinfo(i,:,1)).*wr(1,:))/imgSize;
    alpha_sideinfo=(imgSize*(zlcthreshold+beta)-sum(double(img(i,:)).*wr(1,:)))/sum(wr(1,:).*wr(1,:));
    cw_sideinfo(i,:,2)=img(i,:)+alpha_sideinfo*wr(1,:);
    zlc_sideinfo(i,2)=sum(double(cw_sideinfo(i,:,2)).*wr(1,:))/imgSize;
end

% plot
figure;
histogram(zlc_null,'BinWidth',0.05,'FaceColor','g','FaceAlpha',0.5);hold on;
histogram(zlc_basic(:,1),'BinWidth',0.05,'FaceColor','r','FaceAlpha',0.5);hold on;
histogram(zlc_basic(:,2),'BinWidth',0.05,'FaceColor','b','FaceAlpha',0.5);hold on;
y=ylim;
plot([-zlcthreshold -zlcthreshold],[0 100],'k--',[zlcthreshold zlcthreshold],[0 100],'k--');hold off;
title('blind embedding');
axis([-alpha-1 alpha+1 y(1) y(2)]);
xlabel('linear correlation value');
ylabel('number of images');
legend('{\itnull}','m = 0','m = 1','Location','northoutside','Orientation','horizontal');

figure;
histogram(zlc_null,'BinWidth',0.05,'FaceColor','g','FaceAlpha',0.5);hold on;
histogram(zlc_lowpass(:,1),'BinWidth',0.05,'FaceColor','r','FaceAlpha',0.5);hold on;
histogram(zlc_lowpass(:,2),'BinWidth',0.05,'FaceColor','b','FaceAlpha',0.5);hold on;
y=ylim;
plot([-zlcthreshold -zlcthreshold],[0 100],'k--',[zlcthreshold zlcthreshold],[0 100],'k--');hold off;
title('blind embedding with low pass reference pattern');
axis([-alpha-1 alpha+1 y(1) y(2)]);
xlabel('linear correlation value');
ylabel('number of images');
legend('{\itnull}','m = 0','m = 1','Location','northoutside','Orientation','horizontal');

figure;
histogram(zlc_null,'BinWidth',0.05,'FaceColor','g','FaceAlpha',0.5);hold on;
histogram(zlc_sideinfo(:,1),'BinWidth',0.05,'FaceColor','r','FaceAlpha',0.5);hold on;
histogram(zlc_sideinfo(:,2),'BinWidth',0.05,'FaceColor','b','FaceAlpha',0.5);hold on;
y=ylim;
plot([-zlcthreshold -zlcthreshold],[0 100],'k--',[zlcthreshold zlcthreshold],[0 100],'k--');hold off;
title('blind embedding with side information');
axis([-alpha-1 alpha+1 y(1) y(2)]);
xlabel('linear correlation value');
ylabel('number of images');
legend('{\itnull}','m = 0','m = 1','Location','northoutside','Orientation','horizontal');

clearvars i str image imgN imgW imgH imgSize lowpassthreshold beta y;