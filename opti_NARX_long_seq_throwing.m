clear all
close all 

CloseLoop=1;

if CloseLoop==1
    load('narx_473_CL.mat')
elseif CloseLoop==0
    load('narx_473_OL.mat')
end

numero_esperimento=10000;
step_n=50;%step di durata 0.1sec
max_iter=100;


target=[0.1172; 0.5698; 0.2724]%8001

    
numero_esperimento=numero_esperimento+step_n;
% target=[0.1300; 0.5822; 0.2893]%# posizione setup sinusoidi
%  target=[0.1110; 0.6919; 0.3090]
%target=[0.097246; 0.657279; 0.318852] %7111 150 30 0.005
inputa=[-2.5; 0.0; 0.0]%*ones(1,step);%50 scelto a caso per fare 50 passi
inputa2=[-2.5;0;0];

%
input_temp=[inputa,inputa*ones(1,step_n-1)];
 %input_temp=[inputa*ones(1,step_n)]
input(:,1)=normalization(input_temp(1,:),minTorque1,maxTorque1);
input(:,2)=normalization(input_temp(2,:),minTorque2,maxTorque2);
input(:,3)=normalization(input_temp(3,:),minTorque3,maxTorque3);%-1.8,0.8,0 sono i valori iniziali di torque

% vett1=mat2cell([-2.36*ones(1,step)],1);
% vett2=mat2cell([0*ones(1,step)],1);
% vett3=mat2cell([0*ones(1,step)],1);
% 
% input=[vett1;vett2;vett3];
para.basket=target;
para.net=netc;

lb =[0.1 ;0.1;0.1]*ones(1,step_n);%-5*ones(1,step);%[minTorque1 ;minTorque2; minTorque3 ]
ub=[0.9;0.9;0.9]*ones(1,step_n);%5*ones(1,step);%[maxTorque1; maxTorque2;maxTorque3 ]*ones(1,step);


para.aic=aic;
para.xic=xic;
para.netc=netc;

para.minPos1=minPos1;
para.maxPos1=maxPos1;
para.minPos2=minPos2;
para.maxPos2=maxPos2;
para.minPos3=minPos3;
para.maxPos3=maxPos3;

para.minTorque1=minTorque1;
para.maxTorque1=maxTorque1;
para.minTorque2=minTorque2;
para.maxTorque2=maxTorque2;
para.minTorque3=minTorque3;
para.maxTorque3=maxTorque3;

para.alpha=1;
para.beta=0.005;
% lb=zeros(3,step);
% ub=ones(3,step);
ai=[0.1141; 0.5582; 0.2639]*ones(1,step_n); %%posizione di setup

f = @(inp)evalu_throwing_473_OL(inp,ai,para);
    options =  optimoptions (@fmincon,'Display','iter','MaxIterations',max_iter,'algorithm','sqp','UseParallel',true)%,'OutputFcn',@outfun);%%SQP 'MaxIterations',30,

tic
[inp_opti,fval] = fmincon(f,input,[],[],[],[],lb,ub,[],options);
toc

[parabola,xy,release]=after_throwing_473_OL(inp_opti,ai,para);

inp_denorm(:,1)=denormalization(inp_opti(:,1),minTorque1,maxTorque1);
inp_denorm(:,2)=denormalization(inp_opti(:,2),minTorque2,maxTorque2);
inp_denorm(:,3)=denormalization(inp_opti(:,3),minTorque3,maxTorque3);


%decommenta per vedere differenza tra icub e rete reale
% DIFF=abs(REAL_OUT(3:end,:)-output_NN_pos');  %%REAL_OUT è il vettore in xyz preso dal robot dopo aver usato inp_denorm
% plot(REAL_OUT(3:end,:))
% hold on
% plot(output_NN_pos')

% inp_opt(1,:)=denormalization(inp(1,:),minTorque1,maxTorque1);
% inp_opt(2,:)=denormalization(inp(2,:),minTorque2,maxTorque2);
% inp_opt(3,:)=denormalization(inp(3,:),minTorque3,maxTorque3);
% inp_optimal=inp
% inp_optimal(3,:)=inp_optimal(3,:)
