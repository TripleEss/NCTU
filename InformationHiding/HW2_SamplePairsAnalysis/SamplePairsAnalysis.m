%% jpg

% load image
img=zeros(105,512*512);
for i=1:105
    str=sprintf('%d.jpg',i);
    image=imread(str);
    img(i,:)=reshape(image,1,512*512);
end

% lsb embedding
%rng(1);
watermark_msg=randi([0,1],1,512*512*0.5);
watermark_pos=randperm(512*512);
watermark_img=repmat(img,1,1,3);
for i=1:105
    for po=1:round(512*512*0.05)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,1:3)=img(i,px)+1;
        else
            watermark_img(i,px,1:3)=img(i,px)-1;
        end
    end
    for po=round(512*512*0.05)+1:round(512*512*0.25)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,2:3)=img(i,px)+1;
        else
            watermark_img(i,px,2:3)=img(i,px)-1;
        end
    end
    for po=round(512*512*0.25)+1:round(512*512*0.5)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,3)=img(i,px)+1;
        else
            watermark_img(i,px,3)=img(i,px)-1;
        end
    end
end

%watermark_image=reshape(uint8(watermark_img(i,:,j)),512,512);
%figure;imshow(watermark_image);

% sample pairs analysis
P=512*512*0.5;
q=zeros(3,105);
for i=1:105
    X=zeros(3);
    Y=zeros(3);
    gamma=zeros(3);
    for px=1:2:512*512-1
        for j=1:3
            if (watermark_img(i,px,j)<watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==0) || (watermark_img(i,px,j)>watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==1)
                X(j)=X(j)+1;
            elseif (watermark_img(i,px,j)<watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==1) || (watermark_img(i,px,j)>watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==0)
                Y(j)=Y(j)+1;
                if abs(watermark_img(i,px,j)-watermark_img(i,px+1,j))==1
                    gamma(j)=gamma(j)+1;
                end
            else
                gamma(j)=gamma(j)+1;
            end
        end
    end
    for j=1:3
        r=roots([0.5*gamma(j) 2*X(j)-P Y(j)-X(j)]);
        q(j,i)=max(0,min(r));
    end
end

% dot plot
figure;
xaxis=1:105;
plot(xaxis(:),q(1,:)*100,'ro',xaxis(:),q(2,:)*100,'gs',xaxis(:),q(3,:)*100,'bd','MarkerSize',5);hold on;
plot([1 105],[5 5],'r:',[1 105],[25 25],'g:',[1 105],[50 50],'b:');hold off;
legend('5%','25%','50%','Location','northoutside','Orientation','horizontal');
title('sample pairs analysis for jpg');
axis image;
axis ([1 105 0 100]);
set(gca,'XTick',10:10:100);
set(gca,'YTick',0:5:100);
xlabel('image index');
ylabel('estimated message length %');

% box plot
figure;
subplot(1,3,1);
boxplot(transpose(q(1,:)*100));
title('5%');
set(gca,'XTickLabel','');
ylabel('estimated message length %');
subplot(1,3,2);
boxplot(transpose(q(2,:)*100));
title('25%');
set(gca,'XTickLabel','');
subplot(1,3,3);
boxplot(transpose(q(3,:)*100));
title('50%');
set(gca,'XTickLabel','');

% descriptive statistics
fprintf('------------------------------------------------------\n');
fprintf('----------------descriptive statistics----------------\n');
fprintf('------------------------------------------------------\n');
[maxv,maxi]=max(q(1,:));
[minv,mini]=min(q(1,:));
fprintf(' 5%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(1,:)),var(q(1,:)),std(q(1,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(1,:)),mini,minv);
[maxv,maxi]=max(q(2,:));
[minv,mini]=min(q(2,:));
fprintf('25%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(2,:)),var(q(2,:)),std(q(2,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(2,:)),mini,minv);
[maxv,maxi]=max(q(3,:));
[minv,mini]=min(q(3,:));
fprintf('50%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(3,:)),var(q(3,:)),std(q(3,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(3,:)),mini,minv);
fprintf('\n');

clear str image i j p po px P X Y gamma r xaxis maxv maxi minv mini;

%% png

% load image
img=zeros(105,512*512);
for i=1:105
    str=sprintf('%d.png',i);
    image=imread(str);
    img(i,:)=reshape(image,1,512*512);
end

% lsb embedding
%rng(1);
%watermark_msg=randi([0,1],1,512*512*0.5);
%watermark_pos=randperm(512*512);
watermark_img=repmat(img,1,1,3);
for i=1:105
    for po=1:round(512*512*0.05)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,1:3)=img(i,px)+1;
        else
            watermark_img(i,px,1:3)=img(i,px)-1;
        end
    end
    for po=round(512*512*0.05)+1:round(512*512*0.25)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,2:3)=img(i,px)+1;
        else
            watermark_img(i,px,2:3)=img(i,px)-1;
        end
    end
    for po=round(512*512*0.25)+1:round(512*512*0.5)
        px=watermark_pos(po);
        if mod(img(i,px),2)==watermark_msg(po)
        elseif mod(img(i,px),2)==0 && watermark_msg(po)==1
            watermark_img(i,px,3)=img(i,px)+1;
        else
            watermark_img(i,px,3)=img(i,px)-1;
        end
    end
end

% sample pairs analysis
P=512*512*0.5;
q=zeros(3,105);
for i=1:105
    X=zeros(3);
    Y=zeros(3);
    gamma=zeros(3);
    for px=1:2:512*512-1
        for j=1:3
            if (watermark_img(i,px,j)<watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==0) || (watermark_img(i,px,j)>watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==1)
                X(j)=X(j)+1;
            elseif (watermark_img(i,px,j)<watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==1) || (watermark_img(i,px,j)>watermark_img(i,px+1,j) && mod(watermark_img(i,px+1,j),2)==0)
                Y(j)=Y(j)+1;
                if abs(watermark_img(i,px,j)-watermark_img(i,px+1,j))==1
                    gamma(j)=gamma(j)+1;
                end
            else
                gamma(j)=gamma(j)+1;
            end
        end
    end
    for j=1:3
        r=roots([0.5*gamma(j) 2*X(j)-P Y(j)-X(j)]);
        q(j,i)=max(0,min(r));
    end
end

% dot plot
figure;
xaxis=1:105;
plot(xaxis(:),q(1,:)*100,'ro',xaxis(:),q(2,:)*100,'gs',xaxis(:),q(3,:)*100,'bd','MarkerSize',5);hold on;
plot([1 105],[5 5],'r:',[1 105],[25 25],'g:',[1 105],[50 50],'b:');hold off;
legend('5%','25%','50%','Location','northoutside','Orientation','horizontal');
title('sample pairs analysis for png');
axis image;
axis ([1 105 0 100]);
set(gca,'XTick',10:10:100);
set(gca,'YTick',0:5:100);
xlabel('image index');
ylabel('estimated message length %');

% box plot
figure;
subplot(1,3,1);
boxplot(transpose(q(1,:)*100));
title('5%');
set(gca,'XTickLabel','');
ylabel('estimated message length %');
subplot(1,3,2);
boxplot(transpose(q(2,:)*100));
title('25%');
set(gca,'XTickLabel','');
subplot(1,3,3);
boxplot(transpose(q(3,:)*100));
title('50%');
set(gca,'XTickLabel','');

% descriptive statistics
fprintf('------------------------------------------------------\n');
fprintf('----------------descriptive statistics----------------\n');
fprintf('------------------------------------------------------\n');
[maxv,maxi]=max(q(1,:));
[minv,mini]=min(q(1,:));
fprintf(' 5%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(1,:)),var(q(1,:)),std(q(1,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(1,:)),mini,minv);
[maxv,maxi]=max(q(2,:));
[minv,mini]=min(q(2,:));
fprintf('25%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(2,:)),var(q(2,:)),std(q(2,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(2,:)),mini,minv);
[maxv,maxi]=max(q(3,:));
[minv,mini]=min(q(3,:));
fprintf('50%% : mean    =%f , variance=%f , stddev  =%f\n',mean(q(3,:)),var(q(3,:)),std(q(3,:)));
fprintf('      max(%3d)=%f , median  =%f , min(%3d)=%f\n',maxi,maxv,median(q(3,:)),mini,minv);
fprintf('\n');

clear str image i j p po px P X Y gamma r xaxis maxv maxi minv mini;
