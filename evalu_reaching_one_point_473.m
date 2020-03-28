function rew= evalu_reaching_one_point_473_OL(inp,ai,para)
%  load('motoreNormalizzatoWorkspace.mat');


% load('narx_473_OL.mat')
% inputa=[-2.5; 0.0; 0]%*ones(1,step);%50 scelto a caso per fare 50 passi

target=para.basket;%
% target=[0.1110; 0.6919; 0.3090]%# posizione setup sinusoidi

paraNN.minPos1=para.minPos1;
paraNN.maxPos1=para.maxPos1;

paraNN.minPos2=para.minPos2;
paraNN.maxPos2=para.maxPos2;

paraNN.minPos3=para.minPos3;
paraNN.maxPos3=para.maxPos3;


paraNN.minTorque1=para.minTorque1;
paraNN.maxTorque1=para.maxTorque1;

paraNN.minTorque2=para.minTorque2;
paraNN.maxTorque2=para.maxTorque2;

paraNN.minTorque3=para.minTorque3;
paraNN.maxTorque3=para.maxTorque3;

paraNN.xic=para.xic;
paraNN.aic=para.aic;

netc=para.net;

input_rete=inp';

% [X]=create_dataset_opti(input_rete,1,50);

ai=ai;
Y=tonndata(ai,true,false);
X = tonndata(input_rete,true,false);
[xc,xic,aic,tc] = preparets(netc,X,{},Y); 
% X = tonndata(torque,true,false);

% xic=para.xic;
% aic=para.aic;
% xc=(X(:,2:end));


yc = netc(xc,xic,aic);

output_NN_pos=cell2mat(yc);


%%var 
% var1=var(output_NN_joint(1,:));
% var2=var(output_NN_joint(2,:));
% var3=var(output_NN_joint(3,:));
%  costo=(1/3*(var1)+1/3*(var2)+1/3*(var3)); %%forse ha piu senso usare la varianza
%%nic dice di fare diff.^2 del vettore e poi SUM del vettore dif
% diff_quadr1=diff(output_NN_joint(1,:)).^2;
% diff_quad_1=sum(diff_quadr1,'all');
% 
% diff_quadr2=diff(output_NN_joint(2,:)).^2;
% diff_quad_2=sum(diff_quadr2,'all');
% 
% diff_quadr3=diff(output_NN_joint(3,:)).^2;
% diff_quad_3=sum(diff_quadr3,'all');

% costo=diff_quad_1+diff_quad_2+diff_quad_3;
%%nic2 dice che potrei pesare la derivata della torque input

%% calcorare velocità 

% velocita=diff(denormalized_Pos_evalu_basket')/0.01;

%%altrimenti posso disegnare la curva generata da tutto il movimento del
%%end effector e guardare la distanza minima

% velocita=diff(output_NN_pos')/0.1;

end_effector(1,1)=output_NN_pos(1,end);
end_effector(2,1)=output_NN_pos(2,end);
end_effector(3,1)=output_NN_pos(3,end);

end_effe_trasp=end_effector';
target=para.basket;
basket_trasp=target';
regularization=norm(inp(1:end-1,1:3)-inp(2:end,1:3));
% regularization_wrist=norm(inp(3,1:end-1)-inp(3,2:end));
%         funzione_di_costo=distance+costo*3;
%   
%         rew=funzione_di_costo; %%come concetto a simile a LQ
        %%minimum jerk
alpha=para.alpha;
beta=para.beta;%0.003;
% gamma=0.003;%0.005;
distanza_dal_target=norm(end_effector(1:3,1)-target(1:3,1));
err=alpha*distanza_dal_target+beta*regularization;%+gamma*regularization_wrist

rew=err;
end
