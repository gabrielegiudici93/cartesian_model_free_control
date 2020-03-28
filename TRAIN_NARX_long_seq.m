%%in questo codice valuto avendo come input 3 valori di torque
%%shoulder_pitch;elbow,wrist_pitch 
clear all
close all

load('/home/gabriele/Scrivania/TESI GLOBAL/TESI MERGED/build-motor-babbling-Desktop-Debug/St_Ev_Wv_enlarged_473/workspace.mat')
train_cl=0 %0 train the net only in OL  1 make another training phase in CL

inp_0_3_5=reference;
xyz_position=xyz;

%%%NORMALIZATION INPUT
minTorque1=min(inp_0_3_5(:,1)); 
maxTorque1=max(inp_0_3_5(:,1));
minTorque2=min(inp_0_3_5(:,2));
maxTorque2=max(inp_0_3_5(:,2));
minTorque3=min(inp_0_3_5(:,3));
maxTorque3=max(inp_0_3_5(:,3));

inp_norm(:,1)=normalization(inp_0_3_5(:,1),minTorque1,maxTorque1);
inp_norm(:,2)=normalization(inp_0_3_5(:,2),minTorque2,maxTorque2);
inp_norm(:,3)=normalization(inp_0_3_5(:,3),minTorque3,maxTorque3);

%% NORMALIZATION OUTPUT
minPos1=min(xyz_position(:,1));
maxPos1=max(xyz_position(:,1));
minPos2=min(xyz_position(:,2));
maxPos2=max(xyz_position(:,2));
minPos3=min(xyz_position(:,3));
maxPos3=max(xyz_position(:,3));


%%CREATE DATASET
[X,T]=create_dataset(inp_norm,xyz_position,size(inp_0_3_5,1)/500,500);

trainFcn = 'trainlm';  % Levenberg-Marquardt backpropagation.

inputDelays =0;
feedbackDelays = 1:2;   

hiddenLayerSize = [41]; 

net = narxnet(inputDelays,feedbackDelays,hiddenLayerSize,'open',trainFcn);

net.inputs{1}.processFcns = {'removeconstantrows','mapminmax'};
net.inputs{2}.processFcns = {'removeconstantrows','mapminmax'};

[x,xi,ai,t] = preparets(net,X,{},T);

net.divideFcn = 'divideblock';  % Divide data randomly
net.divideMode = 'time';  % Divide up every sample


net.divideParam.trainRatio = 70/100;
net.divideParam.valRatio = 15/100;
net.divideParam.testRatio =15/100;

net.performFcn = 'mse';  % Mean Squared Error
net.plotFcns = {'plotperform','plottrainstate', 'ploterrhist', ...
'plotregression', 'plotresponse', 'ploterrcorr', 'plotinerrcorr'};


%% Train the Network OPEN LOOP

net.trainParam.epochs=600;
net.trainParam.max_fail = 6;

 [net,tr] = train(net,x,t,xi,ai);

%% Test the Network OPEN LOOP


out_openLoop= net(x,xi,ai); %%%pay attention, l'uscita è 6 valori ma a me interessa la performanca sul xyz_output quindi i primi 3 valori
out_test_open=cell2mat(out_openLoop);
out_test_open_xyz=out_test_open(1:3,:);

target_test_open=cell2mat(t);
target_test_open_xyz=target_test_open(1:3,:);


% errore_open_loop_xyz = gsubtract(target_test_open_xyz,out_test_open_xyz);


performance_open_loop_xyz = perform(net,target_test_open,out_test_open)
errore_metri_open_xyz=sqrt(performance_open_loop_xyz)


%% Recalculate Training, Validation and Test Performance
% trainTargets = gmultiply(t,tr.trainMask);
% valTargets = gmultiply(t,tr.valMask);
% testTargets = gmultiply(t,tr.testMask);
% trainPerformance = perform(net,trainTargets,y)
% valPerformance = perform(net,valTargets,y)
% testPerformance = perform(net,testTargets,y)

%% Closed Loop Network 
netc = closeloop(net);
netc.name = [net.name ' - Closed Loop']
[xc,xic,aic,tc] = preparets(netc,X,{},T);
out_closeLoop = netc(xc,xic,aic);
% closedLoopPerformance = perform(net,tc,out_close_loop)


if train_cl==1
    netc.trainParam.epochs=600;
    netc.trainParam.max_fail = 6;

    [netc,tr] = train(netc,xc,tc,xic,aic);%%###########################################

    %out_closeLoop= net(xc,xic,aic); %%%pay attention, l'uscita è 6 valori ma a me interessa la performanca sul xyz_output quindi i primi 3 valori
    out_test_close=cell2mat(out_closeLoop);
    out_test_close_xyz=out_test_close(1:3,:);

    target_test_close=cell2mat(tc);
    target_test_close_xyz=target_test_close(1:3,:);


    errore_close_loop_xyz = gsubtract(target_test_close_xyz,out_test_close_xyz);
    performance_close_loop_xyz = perform(net,target_test_close,out_test_close)

    errore_metri_close_xyz=sqrt(performance_close_loop_xyz)
end
    %test CLOSELOOP

    % torque_test=inp_0_3_5(9001:10000,:);
%     torque_test=inp_norm(test_min:test_max,:);

    % torque_test(length(torque_test),:)=[];




% 
%     position_test=xyz_position(test_min:test_max,:);
%     x_test=[torque_test]';
%     t_test=[position_test]';
% 
%     X_test=tonndata(x_test,true,false);
%     T_test=tonndata(t_test,true,false);
% 
%     [x_test,xi_test,ai_test,t_test]=preparets(netc,X_test,{},T_test);
% 
%     out_closeLoop = netc(x_test,xi_test,ai_test); %predizione rete
%     out_test_close=cell2mat(out_closeLoop);
%     out_test_close_xyz=out_test_close(1:3,:);
% 
%     target_test_close=cell2mat(t_test);
%     target_test_close_xyz=target_test_close(1:3,:);
% 
%     %%
%     %%euclidean distance
%     for i=1:(length(out_test_close_xyz))
%     err_x=out_test_close(1,i)-target_test_close(1,i); 
%     err_y=out_test_close(2,i)-target_test_close(2,i);
%     err_z=out_test_close(3,i)-target_test_close(3,i) ;
%     euclidean_distance(1,i)=sqrt( (err_x )^2+(err_y )^2+(err_z )^2);
%     end
% 
%     %% Test the Network CLOSE LOOP
% 
% 
%     % errore_open_loop_xyz = gsubtract(t_test_close_xyz,y_test_close_xyz);
%     performance_close_loop_xyz = perform(net,target_test_close,out_test_close)
%     errore_metri_close_xyz=sqrt(performance_close_loop_xyz)
% 
% 
%     % performance_nn=perform(net,t2_xyz(1:3,:),predizione_rete_close_xyz(1:3,:))
%     % metres_performance=sqrt(performance_nn)
% end
