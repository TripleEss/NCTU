function [y] = parity(x,k)
    k=abs(k);
    if k~=0
        i=floor((x+k)/(2*k));
        if x>=(2*i-1)*k && x<=2*i*k-1
            %fprintf('A: %d ~ %d\n',(2*i-1)*k,2*i*k-1);
            y=-power(-1,k);
        elseif x>=2*i*k && x<=(2*i+1)*k-1
            %fprintf('B: %d ~ %d\n',2*i*k,(2*i+1)*k-1);
            y=power(-1,k);
        else
            fprintf('parity function error\n');
        end
    else
        y=0;
    end
end
%{
P(x,k)= (-1)^k if                           <= 2i|k| <= x <= (2i+1)|k|-1
P(x,k)=-(-1)^k if (2i-1)|k| <= x <= 2i|k|-1 <=
P(x,k)=0       if k=0
%}
