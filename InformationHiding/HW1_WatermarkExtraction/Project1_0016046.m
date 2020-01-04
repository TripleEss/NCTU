%% main program

% load work
% reshaping image from matrix to vertex for convenient looping
work = imread('work.tiff');
[work_height,work_width] = size(work);
work = reshape(work,1,work_height*work_width);
% set parameters
spread = 512;
alpha = 10;
% randomize watermark
% fixed seed for analysing result 
% rng(1);
watermark_bits = floor(size(work,2)/spread);
watermark = randi([0,1],1,watermark_bits)*2-1;
% spread watermark
for i = 1:watermark_bits
    b(spread*(i-1)+1:spread*i) = watermark(i);
end
% randomize pseudonoise
p = randi([0,1],size(b))*2-1;
% embed watermark into work
% embedded part + non-embedded part
watermarked_work = zeros(size(work));
for i = 1:spread*watermark_bits
	watermarked_work(i) = work(i)+alpha*b(i)*p(i);
end
for i = spread*watermark_bits+1:size(work) 
	watermarked_work(i) = work(i);
end
% extract watermark
extracted_watermark = zeros(size(watermark));
for i = 1:watermark_bits
    extracted_watermark(i) = sign(sum(watermarked_work(spread*(i-1)+1:spread*i).*p(spread*(i-1)+1:spread*i)));
end
% output information
watermark_bits
error_bits = sum(abs(extracted_watermark(1:watermark_bits)-watermark(1:watermark_bits)))/2
error_rate = error_bits/watermark_bits
% output watermarked work
% transfering from double to unit8 and reshaping back to matrix
watermarked_work = uint8(watermarked_work);
watermarked_work = reshape(watermarked_work,work_height,work_width);

str = sprintf('cr%d_alpha%d_watermarked_work.tiff',spread,alpha);
imwrite(watermarked_work,str);
figure;
imshow(watermarked_work);
str = sprintf('watermarked work with spread-degree %d and alpha-value %d',spread,alpha);
title(str);

%% fixed spread, variable alpha

alpha_num = 50;
error = zeros(alpha_num);
for a = 1:alpha_num
    watermarked_work = zeros(size(work));
    for i = 1:spread*watermark_bits
        watermarked_work(i) = work(i)+a*b(i)*p(i);
    end
    for i = spread*watermark_bits+1:size(work) 
        watermarked_work(i) = work(i);
    end

    extracted_watermark = zeros(size(watermark));
    for i = 1:watermark_bits
        extracted_watermark(i) = sign(sum(watermarked_work(spread*(i-1)+1:spread*i).*p(spread*(i-1)+1:spread*i)));
    end

    error_bits = sum(abs(extracted_watermark(1:watermark_bits)-watermark(1:watermark_bits)))/2;
    error(a) = error_bits/watermark_bits;
end

figure;
plot(1:alpha_num,error(1:alpha_num),'LineWidth',1,'Marker','o');
str = sprintf('relationship between alpha-value and error-rate with fixed-spread-degree %d',spread);
title(str);
xlabel('alpha-value');
ylabel('error-rate');

%% fixed alpha, variable spread

spread_num = 10;
sp = [32 64 128 200 256 300 350 400 450 512];
error = zeros(spread_num);
for spidx = 1:spread_num
    watermark_bits = floor(size(work,2)/sp(spidx));
    watermark = randi([0,1],1,watermark_bits)*2-1;
    for i = 1:watermark_bits
        b(sp(spidx)*(i-1)+1:sp(spidx)*i) = watermark(i);
    end
    p = randi([0,1],size(b))*2-1;

    watermarked_work = zeros(size(work));
    for i = 1:sp(spidx)*watermark_bits
        watermarked_work(i) = work(i)+alpha*b(i)*p(i);
    end
    for i = sp(spidx)*watermark_bits+1:size(work) 
        watermarked_work(i) = work(i);
    end

    extracted_watermark = zeros(size(watermark));
    for i = 1:watermark_bits
        extracted_watermark(i) = sign(sum(watermarked_work(sp(spidx)*(i-1)+1:sp(spidx)*i).*p(sp(spidx)*(i-1)+1:sp(spidx)*i)));
    end

    error_bits = sum(abs(extracted_watermark(1:watermark_bits)-watermark(1:watermark_bits)))/2;
    error(spidx) = error_bits/watermark_bits;
end

figure;
plot(sp(1:spread_num),error(1:spread_num),'LineWidth',1,'Marker','o');
str = sprintf('relationship between spread-degree and error-rate with fixed-alpha-value %d',alpha);
title(str);
xlabel('spread-degree');
ylabel('error-rate');
