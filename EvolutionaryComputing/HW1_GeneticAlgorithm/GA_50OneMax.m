%% 50-bit OneMax with Roulette Wheel Selection
clear all;

bestfx = zeros(10,101);
for r=1:10
    rng(r);
    x = logical(randi([0 1],200,50));
    
    for g=1:101
        fx = sum(x,2);
        sumfx = sum(fx);
        meanfx = mean(fx);
        bestfx(r,g) = max(fx);
        px = fx/sumfx;

        for i=1:2:199
            m = [find(rand<cumsum(px),1) find(rand<cumsum(px),1)];
            while m(1)==m(2)
                m(2) = find(rand<cumsum(px),1);
            end
            mating = [x(m(1),:); x(m(2),:)];

            xpt = randi([2 50]);
            x(i,:) = [mating(1,1:xpt-1) mating(2,xpt:50)];
            x(i+1,:) = [mating(2,1:xpt-1) mating(1,xpt:50)];
        end
    end
end

meanbestfx = mean(bestfx);
figure;
plot(0:100,meanbestfx);
title('GA for \Sigma_{i=0}^{50}x_{i} 50-bit OneMax w/ Roulette Wheel Selection');
xlabel('Generation');
ylabel('Best Fitness');
axis([0 100 min(meanbestfx)-1 max(meanbestfx)+1]);

%% 50-bit OneMax + 1000 with Roulette Wheel Selection
clear all;

bestfx = zeros(10,101);
for r=1:10
    rng(r);
    x = logical(randi([0 1],200,50));
    
    for g=1:101
        fx = sum(x,2)+1000;
        sumfx = sum(fx);
        meanfx = mean(fx);
        bestfx(r,g) = max(fx);
        px = fx/sumfx;

        for i=1:2:199
            m = [find(rand<cumsum(px),1) find(rand<cumsum(px),1)];
            while m(1)==m(2)
                m(2) = find(rand<cumsum(px),1);
            end
            mating = [x(m(1),:); x(m(2),:)];

            xpt = randi([2 50]);
            x(i,:) = [mating(1,1:xpt-1) mating(2,xpt:50)];
            x(i+1,:) = [mating(2,1:xpt-1) mating(1,xpt:50)];
        end
    end
end

meanbestfx = mean(bestfx-1000);
figure;
plot(0:100,meanbestfx);
title('GA for \Sigma_{i=0}^{50}x_{i}+1000 50-bit OneMax w/ Roulette Wheel Selection');
xlabel('Generation');
ylabel('Best Fitness - 1000');
axis([0 100 min(meanbestfx)-1 max(meanbestfx)+1]);

%% 50-bit OneMax with Tournament Selection
clear all;

bestfx = zeros(10,101);
for r=1:10
    rng(r);
    x = logical(randi([0 1],200,50));
    
    for g=1:101
        fx = sum(x,2);
        sumfx = sum(fx);
        meanfx = mean(fx);
        bestfx(r,g) = max(fx);

        for i=1:2:199
            for p=1:2
                m = randi([1 200],2,1);
                while m(1)==m(2)
                    m(2) = randi([1 200]);
                end
                if fx(m(1))>=fx(m(2))
                   mating(p,:) = x(m(1),:); 
                else
                   mating(p,:) = x(m(2),:);
                end
            end
            
            xpt = randi([2 50]);
            x(i,:) = [mating(1,1:xpt-1) mating(2,xpt:50)];
            x(i+1,:) = [mating(2,1:xpt-1) mating(1,xpt:50)];
        end
    end
end

meanbestfx = mean(bestfx);
figure;
plot(0:100,meanbestfx);
title('GA for \Sigma_{i=0}^{50}x_{i} 50-bit OneMax w/ Tournament Selection');
xlabel('Generation');
ylabel('Best Fitness');
axis([0 100 min(meanbestfx)-1 max(meanbestfx)+1]);

%% 50-bit OneMax + 1000 with Tournament Selection
clear all;

bestfx = zeros(10,101);
for r=1:10
    rng(r);
    x = logical(randi([0 1],200,50));
    
    for g=1:101
        fx = sum(x,2)+1000;
        sumfx = sum(fx);
        meanfx = mean(fx);
        bestfx(r,g) = max(fx);

        for i=1:2:199
            for p=1:2
                m = randi([1 200],2,1);
                while m(1)==m(2)
                    m(2) = randi([1 200]);
                end
                if fx(m(1))>=fx(m(2))
                   mating(p,:) = x(m(1),:); 
                else
                   mating(p,:) = x(m(2),:);
                end
            end
            
            xpt = randi([2 50]);
            x(i,:) = [mating(1,1:xpt-1) mating(2,xpt:50)];
            x(i+1,:) = [mating(2,1:xpt-1) mating(1,xpt:50)];
        end
    end
end

meanbestfx = mean(bestfx-1000);
figure;
plot(0:100,meanbestfx);
title('GA for \Sigma_{i=0}^{50}x_{i}+1000 50-bit OneMax w/ Tournament Selection');
xlabel('Generation');
ylabel('Best Fitness - 1000');
axis([0 100 min(meanbestfx)-1 max(meanbestfx)+1]);