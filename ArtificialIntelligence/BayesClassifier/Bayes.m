% constructor
ClassNum=2;
FeatureVectorSize=228;

% initialization
TrainClassNum(1:ClassNum)=0;
TestClassNum(1:ClassNum)=0;    
m(1:ClassNum,1:FeatureVectorSize)=0;
c(1:FeatureVectorSize,1:FeatureVectorSize,1:ClassNum)=0;

% load training data
fin_train=fopen('train.txt','r');
fin_trainlabel=fopen('train_label.txt','r');

TrainNum=1;
while ~feof(fin_train)
    for i=1:FeatureVectorSize
        TrainSample(TrainNum).fv(i)=fscanf(fin_train,'%d',1);
    end
    TrainSample(TrainNum).label=fscanf(fin_trainlabel,'%d',1);
    TrainClassNum(TrainSample(TrainNum).label)=TrainClassNum(TrainSample(TrainNum).label)+1;
    TrainNum=TrainNum+1;
end
TrainNum=TrainNum-1;

clear fin_train;
clear fin_trainlabel;

% Mean
for i=1:TrainNum
    for u=1:FeatureVectorSize
        m(TrainSample(i).label,u)=m(TrainSample(i).label,u)+TrainSample(i).fv(u);
    end
end
for i=1:ClassNum
    for j=1:FeatureVectorSize
        m(i,j)=m(i,j)/TrainClassNum(i);
    end
end
disp('Mean done');

% Covariance
for i=1:TrainNum
    for u=1:FeatureVectorSize
        for v=1:FeatureVectorSize
            du=TrainSample(i).fv(u)-m(TrainSample(i).label,u);
            dv=TrainSample(i).fv(v)-m(TrainSample(i).label,v);
            c(u,v,TrainSample(i).label)=c(u,v,TrainSample(i).label)+(du*dv);
        end
    end
end
for i=1:ClassNum
    for j=1:FeatureVectorSize
        for k=1:FeatureVectorSize
            c(j,k,i)= c(j,k,i)/(TrainClassNum(i)-1);
        end
    end
end
disp('Covariance done');

% cross validation
for test=1:TrainNum
    
    cno=TrainSample(test).label;
    for i=1:ClassNum
        for j=1:FeatureVectorSize
            mtest(i,j)=m(i,j);
            for k=1:FeatureVectorSize
                ctest(j,k,i)=c(j,k,i);
            end
        end
    end
    mtest(cno,1:FeatureVectorSize)=0;
    ctest(1:FeatureVectorSize,1:FeatureVectorSize,cno)=0;
    
    % recompute Mean
    for i=1:TrainNum
        if(i~=test && TrainSample(i).label==cno)
            for j=1:FeatureVectorSize
                mtest(cno,j)=mtest(cno,j)+TrainSample(i).fv(j);
            end
        end
    end
    
    for i=1:FeatureVectorSize
        mtest(cno,i)=mtest(cno,i)/(TrainClassNum(cno)-1);
    end
    
    % recompute Covariance
    for i=1:TrainNum
        if(i~=test && TrainSample(i).label==cno)
            for u=1:FeatureVectorSize
                for v=1:FeatureVectorSize
                    du=TrainSample(i).fv(u)-mtest(cno,u);
                    dv=TrainSample(i).fv(v)-mtest(cno,v);
                    ctest(u,v,cno)=ctest(u,v,cno)+(du*dv);
                end
            end
        end
    end
    
    for i=1:FeatureVectorSize
        for j=1:FeatureVectorSize
            ctest(i,j,cno)= ctest(i,j,cno)/(TrainClassNum(cno)-2);
        end
    end
   
    % compute Discriminant Function
    for i=1:ClassNum
        for j=1:FeatureVectorSize
            x(j)=TrainSample(test).fv(j)-mtest(i,j);
        end
        
        tmp(1:FeatureVectorSize)=0;
        Ictest=inv(ctest(:,:,i));
        for j=1:FeatureVectorSize
            for k=1:FeatureVectorSize
                tmp(j)=tmp(j)+(x(k)*Ictest(j,k));
            end
        end
        
        dm=0;
        for j=1:FeatureVectorSize
            dm=dm+tmp(j)*x(j);
        end
        
        if(i~=cno)
            g(i)=-dm-log(abs(det(ctest(:,:,i))))+log(TrainClassNum(i)/TrainNum);
        else
            g(i)=-dm-log(abs(det(ctest(:,:,i))))+log((TrainClassNum(i)-1)/TrainNum);
        end
        %g(i)=(1/(det(ctest(:,:,i))^(1/2)))*exp(-dm);
    end
        
    max=1;
    maxg=g(1);
    for i=2:ClassNum
        if(g(i)>maxg)
            maxg=g(i);
            max=i;
        end
    end
    
    TrainSample(test).labeltest=max;
    
    disp(['(' num2str(test) ')' num2str(TrainSample(test).labeltest) ' -> ' num2str(TrainSample(test).labeltest)]);
end
clear test;
clear cno;
clear mtest;
clear ctest;
clear du;
clear dv;
clear x;
clear tmp;
clear dm;
clear g;
clear max;
clear maxg;


% Confusion Matrix
ConfusionMatrix(1:ClassNum,1:ClassNum)=0;
err=0;
for i=1:TrainNum
    if(TrainSample(i).label~=TrainSample(i).labeltest)
        err=err+1;
    end
    ConfusionMatrix(TrainSample(i).label,TrainSample(i).labeltest)=ConfusionMatrix(TrainSample(i).label,TrainSample(i).labeltest)+1;
end
clear i;
clear j;
clear k;
clear u;
clear v;

ConfusionMatrix
RecognitionRate=(TrainNum-err)/TrainNum

% load testing data
fin_test=fopen('test.txt','r');

TestNum=1;
while ~feof(fin_test) && TestNum<=200
    
    for i=1:FeatureVectorSize
        TestSample(TestNum).fv(i)=fscanf(fin_test,'%d',1);
    end
    
    % compute Discriminant Function
    for i=1:ClassNum
        for j=1:FeatureVectorSize
            x(j)=TestSample(TestNum).fv(j)-m(i,j);
        end
        
        tmp(1:FeatureVectorSize)=0;
        Ic=inv(c(:,:,i));
        for j=1:FeatureVectorSize
            for k=1:FeatureVectorSize
                tmp(j)=tmp(j)+(x(k)*Ic(j,k));
            end
        end
        
        dm=0;
        for j=1:FeatureVectorSize
            dm=dm+tmp(j)*x(j);
        end
        
        g(i)=-dm-log(abs(det(c(:,:,i))));
    end
        
    max=1;
    maxg=g(1);
    for i=2:ClassNum
        if(g(i)>maxg)
            maxg=g(i);
            max=i;
        end
    end
    
    TestSample(TestNum).labeltest=max;
    
    disp(['(' num2str(TestNum) ')' num2str(TestSample(TestNum).labeltest)]);
    TestClassNum(TestSample(TestNum).labeltest)=TestClassNum(TestSample(TestNum).labeltest)+1;
    TestNum=TestNum+1;
end
TestNum=TestNum-1;
clear fin_test;
clear x;
clear tmp;
clear dm;
clear g;
clear max;
clear maxg;
