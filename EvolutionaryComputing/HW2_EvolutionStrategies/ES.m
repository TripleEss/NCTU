run=3;

%% basic
sigma=[0.01 0.1 1.0];
 
% (1+1) ES
type11_run=zeros(3,10);
type11_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        g=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;

            delta_x=sigma(s)*randn(1,10);
            child_x=x+delta_x;
            if sum(child_x.*child_x)<sum(x.*x)
                x=child_x;
            end
        end
        type11_run(s,r)=g
        type11_result_x(r,:,s)=x;
    end
end

% (1,1) ES
type12_run=zeros(3,10);
type12_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        g=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;

            delta_x=sigma(s)*randn(1,10);
            child_x=x+delta_x;
            x=child_x;
        end
        type12_run(s,r)=g
        type12_result_x(r,:,s)=x;
    end
end


%% 1/5-rule
sigma=[0.01 0.1 1.0];

gr=50;
alpha=0.85;

% (1+1) ES
type21_run=zeros(3,10);
type21_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        sig=sigma(s);
        g=0;
        gs=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;

            delta_x=sig*randn(1,10);
            child_x=x+delta_x;
            if sum(child_x.*child_x)<sum(x.*x)
                x=child_x;
                gs=gs+1;
            end
            
            if mode(g,gr)==0
                if gs/gr>1/5
                    sig=sig/alpha;
                elseif gs/gr<1/5
                    sig=sig*alpha;
                end
                gs=0;
            end
        end
        type21_run(s,r)=g
        type21_result_x(r,:,s)=x;
    end
end

% (1,1) ES
type22_run=zeros(3,10);
type22_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        sig=sigma(s);
        g=0;
        gs=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;

            delta_x=sig*randn(1,10);
            child_x=x+delta_x;
            x=child_x;
            if sum(child_x.*child_x)<sum(x.*x)
                gs=gs+1;
            end
            
            if mode(g,gr)==0
                if gs/gr>1/5
                    sig=sig/alpha;
                elseif gs/gr<1/5
                    sig=sig*alpha;
                end
                gs=0;
            end
        end
        type22_run(s,r)=g
        type22_result_x(r,:,s)=x;
    end
end


%% n step-sizes uncorrelated mutation
sigma=[0.01 0.1 1.0];

rng(1);
base_mutation=1/sqrt(2*10)*randn;

% (1+1) ES
type31_run=zeros(3,10);
type31_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        sig=repmat(sigma(s),1,10);
        g=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;
            
            tau=1/sqrt(2*sqrt(10));
            sig=sig.*exp(base_mutation+tau*randn(1,10));
            for i=1:10    
                if sig(i)<0.001
                    sig(i)=0.001;
                end
            end
            delta_x=sig.*randn(1,10);
            child_x=x+delta_x;
            if sum(child_x.*child_x)<sum(x.*x)
                x=child_x;
            end
        end
        type31_run(s,r)=g
        type31_result_x(r,:,s)=x;
    end
end

% (1,1) ES
type32_run=zeros(3,10);
type32_result_x=zeros(10,10,3);
for s=1:3
    for r=1:run
        x=ones(1,10,1);
        sig=repmat(sigma(s),1,10);
        g=0;
        while sum(x.*x)>0.005 && g<=10000000
            g=g+1;
            
            tau=1/sqrt(2*sqrt(10));
            sig=sig.*exp(base_mutation+tau*randn(1,10));
            for i=1:10    
                if sig(i)<0.001
                    sig(i)=0.001;
                end
            end
            delta_x=sig.*randn(1,10);
            child_x=x+delta_x;
            x=child_x;
        end
        type32_run(s,r)=g
        type32_result_x(r,:,s)=x;
    end
end