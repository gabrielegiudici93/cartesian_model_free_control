import scipy
from scipy.spatial import distance_matrix
import torch
import numpy as np
import models
import sys
import pandas as pd
from scipy.optimize import Bounds, minimize

#nome_file, nome_rete_neurale, step, numero_esperimento, task, alpha, beta = sys.argv
control_var=3
setup='icub'
nome_rete_neurale='icub_St_Ev_Wv_473_testmodel.pt' #icub_St_Ev_Wv_473_testmodel=41			icub_shoulderwrist_testmodel=41 #icub_St_Ev_Wv_testmodel=37

layer_size=41
step=100

for ttt in range(0,10):
	print (ttt)

	numero_esperimento=5000+ttt

	task='REACHING' # 'BASKET_SHOT' REACHING
	makeTest=1
	alpha=1
	beta=0.0005
	task_fcn=task
	print('LSTM TRAJECTORY OPTIMIZATION')
	step=int(step)
	# torque_bounds = Bounds([-5, 5]*(np.ones((step, 1))), [-5, 5]*(np.ones((step, 1))), [-5, 5]*(np.ones((step, 1))))




	g=9.81

	if control_var==3:
		#bound1 = (-5.4, 0.4)
		#bound2 = (-0.2, 1.6)
		#bound3 = (-40, 40)
		bound1 = (0.1, 0.9)
		bound2 = (0.1, 0.9)
		bound3= (0.1, 0.9)
		torque_bounds = [bound1, bound2, bound3]*step
		torque0 = np.array([[0.5, 0.5, 0.5]] * step)
	#	torque0=np.array([[-2.5, 0.7, 0]]*step)

	if control_var==2:
		bound1 = (0, 1)
		bound2 = (0, 1)
		torque_bounds=[bound1, bound2]*step
		torque0 = np.array([[0.49, 0.5]] * step)


	device = torch.device('cpu')

	#eng = matlab.engine.start_matlab()
	#posizione_iniziale=[-0.0871409,0.580162,0.258022]


	target0=[0.1141,	0.5582,	0.2639]# posizione setup sinusoidi

	if numero_esperimento==5000:
		target=[0.1110, 0.6919, 0.3090]#5000  ***
	elif numero_esperimento==5001:
		target=[0.1172, 0.5698, 0.2724]#5001
	elif numero_esperimento == 5002:
		target=[0.1155, 0.5792, 0.2783]#5002
	elif numero_esperimento == 5003:
		target=[0.1212, 0.5846, 0.2806]#5003
	elif numero_esperimento == 5004:
		target=[0.102429, 0.613089, 0.299167]#5004
	elif numero_esperimento == 5005:
		target=[0.130023, 0.632226, 0.289366]#5005
	elif numero_esperimento == 5006:
		target=[0.097246, 0.657279, 0.318852]#5006
	elif numero_esperimento == 5007:
		target=[0.108033, 0.675294, 0.304130]#5007
	elif numero_esperimento == 5008:
		target=[0.128457, 0.700326, 0.297203]#5008
		#target = [ 0.700326, 0.297203]  # 5008kkk
	elif numero_esperimento == 5009:
		target=[0.1141, 0.5582, 0.2639]#5009

	print('numero_esperimento',numero_esperimento)

	numero_esperimento=numero_esperimento+step+1000


	basket=[0.114528, 1.300402, 0.26101]

	network_lstm=models.MyLSTM(control_var, layer_size, 3).double() #input-layersize-output
	network_lstm.load_state_dict(torch.load(nome_rete_neurale, map_location=torch.device('cpu'))) #lstm_modello_finalemodel.pt


	input= torch.from_numpy(torque0[:, None, :]).float()
	hidden_intial = network_lstm.init_hidden(input.size(1), device=device, dtype=torch.double) #restituisce lo stato inizi dei neuroni

	# output, hidden = network_lstm(input, hidden_intial) #in questo caso hidden  lo stato della rete al'ultimo passo
	print('target', target)



	def evalu_reaching(torque_input, with_point=False):
		torque=torque_input.reshape(step,control_var)
		torch_torques=torch.from_numpy(torque[:, None, :])
		output, hidden = network_lstm(torch_torques, hidden_intial)#output rete  un tensore (step,1,3)
		final_value=output.data[-1, 0].numpy()
		yyy=output.data[-1, 0, 1:].numpy()
		distanza_euclidea=np.sqrt(np.sum((output.data[-1, 0].numpy()-target)**2))
		#print('output.data[-1,0]',output.data[-1,0])

		print('distanza_euclidea',distanza_euclidea)
		output_final=output.detach().numpy().reshape(step, 3)
		regularization=sum((np.linalg.norm(torque[:-1, :]-torque[1:, :],2,axis=1) ))

		costo = float(alpha) * distanza_euclidea + float(beta) * regularization

		if with_point==True:
			print(output_final)
			print('regularization',regularization)
			print(distanza_euclidea)
			return costo, output_final
		else:
			#if (costo<0.009):
			#	print(output_final)
			return costo




	def basket_shot(torque_input, with_point=False):
		torque = torque_input.reshape(step, 3)
		#print(torque_input)
		torch_torques = torch.from_numpy(torque[:, None, :])
		output, hidden = network_lstm(torch_torques, hidden_intial)  # output rete  un tensore (step,1,3)
		#output_final=output['x'].reshape(step, 3)
		output_final=output.detach().numpy().reshape(step, 3)

		velocita=np.diff(output_final,axis=0)/0.1
		dt=0.1 #fixed 100ms
		time_vect=[j for j in np.arange(0,step*dt,dt)]

		#for i=1 fino alla fine del vettore velocita
		costo = np.inf
		punto_di_lancio=None
		for i in range(len(velocita)):
			x=np.multiply(velocita[i][0],time_vect)+output_final[i][0]
			y=np.multiply(velocita[i][1],time_vect)+output_final[i][1]
			z=np.multiply(velocita[i][2],time_vect)+output_final[i][2]
			parabola=np.ndarray((step,3))
			parabola[:,0]=x.T
			parabola[:,1]=y.T
			parabola[:,2]=z.T

			#distance=calcola distanza tra parabola e basket
			print('parabola',parabola)
			basket_vect = np.array([basket] * step)
			#distance=scipy.spatial.distance_matrix(parabola,basket) #reuturn Mx1 parabola MxK basket 1xK k=3 dimension_xyz
			distance = np.sqrt(sum((parabola - basket_vect)**2)) #  un valore
			print('distance',distance)
			distanza_minima=min(distance)
			print('distanza_minima',distanza_minima)
			#punto_di_lancio=output_final[i]#parabola[values.index(min(distance))]##questo lo devo salvare in memoria..insieme all'indice perch mi dice il momento in cui lascare

			if distanza_minima < costo:
				costo = distanza_minima
				punto_di_lancio = output_final[i]
			#print('indice punto di lancio',values.index(min(distance)))# stampa l'istante di lancio
			print('punto_di_lancio',punto_di_lancio)

		#print('basket_shot')
		regularization=sum((np.linalg.norm(torque[:-1, :]-torque[1:, :],2,axis=1) ))
		#alpha = 1
		#beta = 1 / 1000 * 3
		# for ogni valore di alpha POTREI METTERE UNA OPTI(per alphabeta) DENTRO LA OPTI; MA A LIVELLO COMPUTAZIONALE LENTO CREDO
		rev = float(alpha) * costo + float(beta)*regularization
		if with_point==True:
			return costo, punto_di_lancio
		else:
			return rev

	#def evalu_test(inp):
	#    return np.sqrt(sum((inp[-3:]-target)**2))


	torque0_reshape=torque0.reshape(step*control_var)

	torque_final = None
	if task == "REACHING":
		print('TASK REACHING')
		torque_final=scipy.optimize.minimize(evalu_reaching, torque0_reshape, jac='2-point', method='SLSQP', bounds=torque_bounds, options={'ftol': 0.000001,'maxiter':60000})
		costo, output_final=evalu_reaching(torque_final['x'], with_point=True)

		print('output final',output_final)
		df_out_nn = pd.DataFrame(index=(np.arange(0, step)), columns=['x','y','z'])
		for i in range(len(output_final)):
			df_out_nn.loc[i]['x'] = output_final[i][0]
			df_out_nn.loc[i]['y'] = output_final[i][1]
			df_out_nn.loc[i]['z'] = output_final[i][2]
			##save the position xyz
			df_out_nn.to_csv('icub_reference/LSTM_out_NN_xyz' + str(numero_esperimento) + '_' + str(task) + '.csv', sep=' ', decimal=',', index=False)

	elif task == "BASKET_SHOT":
		print('TASK BASKET SHOT')
		torque_final=scipy.optimize.minimize(basket_shot, torque0_reshape, jac='2-point', method='SLSQP', bounds=torque_bounds, options={'ftol': 0.000001,'maxiter':10})
		costo, output_final=basket_shot(torque_final['x'], with_point=True)

	else:
		print('TASK SCONOSCIUTO')

	print('torque_final =', torque_final)

	print('numero_esperimento',numero_esperimento)

	###save the torque ref
	print(torque_final['x'].reshape(step, control_var))
	if makeTest==1:
		np.savetxt("gazebo_reference/REACHING_ref/"'LSTM_gazebo_reference_'+str(numero_esperimento)+"_"+str(task)+".csv", torque_final['x'].reshape(step, control_var), delimiter=",")
	elif setup=='icub':
		np.savetxt("icub_reference/"+str(setup)+"_"+"icub_reference"+str(numero_esperimento)+"_"+str(task)+".csv", torque_final['x'].reshape(step, control_var), delimiter=",")
	elif setup=='gazebo':
		np.savetxt("gazebo_reference/"+str(setup)+"_"+"gazebo_reference"+str(numero_esperimento)+"_"+str(task)+".csv", torque_final['x'].reshape(step, 3), delimiter=",")
	else:
		print('setup errato')
		quit()
	#scipy.optimize.minimize()

