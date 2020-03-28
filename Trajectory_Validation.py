# 1 prendi torque di opti e mettili in test_torque_reaching_one_point.csv
# 2 apri gazebo e motorgui
# 3 runna questo codice per avere test2_position_reaching_one_point.csv
# 4 metti i valori qui test2_position_reaching_one_point.txt (ricorda ctrl+h e sostituisci '\n' con ' \n' e cancella ultima riga
# 5 fai la conversione chiamando \fkin

##########################################i file di reference presi da MATLAB o PYTHON  devono essere 3*20
import time
import random
import yarp
import pandas as pd
import numpy as np
from icub_gazebo_sync import iCubGazeboRunner
from setup_fun import setup_LSTM,setup_NARX, setup_gazebo_LSTM,setup_gazebo_NARX, setup_LSTM_open

icub='on' #icub=on /off
gazebo='off' #on per simulare su gazebo,  off usare icub
setup='NARX'
wrist_offset= +12.6
step=50
exp=8050
#numero esperimento
task='REACHING' #REACHING THROWING


if (icub!='off' and icub!='on'):
    print('icub config err.')
    quit()
if (gazebo!='off' and gazebo!='on'):
    print('gazebo config err.')
    quit()
if ((gazebo=='off'and icub=='off') or (gazebo=='on' and icub=='on')):
    print('icub/gazebo config err.')
    quit()
if (task!='REACHING' and task!='THROWING'):
    print('task confg err.')
    quit()







if (setup!='LSTM' and setup!='NARX'):
    print('SETUP confg err.')
    quit()

control_variable=3 #2 solo shoulder ed elbow , 3 anche wrist

if (control_variable!=2 and control_variable!=3):
    print('Control Variable not correct')
    quit()




yarp.Network.init()

if gazebo=='on':
    print(setup+' GAZEBO TEST '+task)
    #df = np.loadtxt(''), delimiter=',')
    runner = iCubGazeboRunner()

else:
    print(setup + ' iCub TEST ' + task)


# prepare a property object
props = yarp.Property()
props.put("device", "remote_controlboard")
props.put("local", "/client/left_arm")
if gazebo=='on' and icub=='off':
    props.put("remote", "/icubSim/left_arm")
elif gazebo=='off' and icub=='on':
    props.put("remote", "/icub/left_arm")

else:
    print('Not existing Robot')
    quit()
#yarp.Network.init();

# create remote driver
armDriver = yarp.PolyDriver(props)
time.sleep(0.1)

# query motor control interfaces
iPos = armDriver.viewIPositionControl()
iVel = armDriver.viewIVelocityControl()
iEnc = armDriver.viewIEncoders()
iTrq = armDriver.viewITorqueControl()
iCtrl = armDriver.viewIControlMode()

# retrieve number of joints
jnts = iPos.getAxes()

# read encoders
encs = yarp.Vector(jnts)
torques = yarp.Vector(jnts)
iEnc.getEncoders(encs.data())

# Switch to Torque Control Mode for each Joint
iCtrl.setPositionMode(0)  # r_shoulder_pitch
iCtrl.setPositionMode(1)
iCtrl.setPositionMode(2)
iCtrl.setPositionMode(3)  # elbow
iCtrl.setPositionMode(4)
iCtrl.setPositionMode(5)  # wrist_pitch
iCtrl.setPositionMode(6)

def enco_initial_pos():
    enco_pos = []
    for j in range(7):
        enco_pos.append(iEnc.getEncoder(j))
    print('ENCODER POS', enco_pos)


if gazebo=='off':
    if setup=='LSTM':
        print('LSTM ICUB TEST'+task)
        setup_LSTM(iCtrl,iPos, iEnc)
        enco_initial_pos()
#       reference_values0= [-2.5, 0.7, 0]#stessi valori del optimization
        reference_values = [-2.5, 0.0, 0.0]
        reference_values0 = [-2.5, 0.0, 0.0]

    elif setup=='NARX':
        print('NARX ICUB TEST'+task)
        setup_LSTM(iCtrl,iPos, iEnc)
        enco_initial_pos()
        reference_values = [-2.5, 0.0, 0.0]
        reference_values0 = [-2.5, 0.0, 0.0]
    else:
        print('SETUP SCONOSCIUTO')

elif gazebo=='on':
    if setup=='LSTM':
        print('LSTM GAZEBO TEST'+task)
        setup_gazebo_LSTM(iCtrl,iPos,runner,iEnc)
        enco_initial_pos()
        reference_values0 = [-2.5, 0, 0]#stessi valori del optimization
        reference_values = [-2.5, 0, 0]

    elif setup=='NARX':
        print('NARX GAZEBO TEST_'+task)
        setup_gazebo_NARX(iCtrl,iPos, runner,iEnc)
        enco_initial_pos()
        reference_values0 = [-2.5, 0, 0]#stessi valori del optimization
        reference_values = [-2.5, 0, 0]

else:
    print('SETUP GAZEBO SBAGLIATO 1')
    quit()

task_fcn = task

iterations = int(step)

df_position = pd.DataFrame(index=(np.arange(0, iterations)),columns=['theta0_pos', 'theta1_pos', 'theta2_pos', 'theta3_pos', 'theta4_pos', 'theta5_pos','theta6_pos'])

###iCUB CODE
if ( icub=='on' and gazebo=='off' ):
    print('ICUB')
# sarah del tipo NARX_icub_reference_1_REACHING.csv
#il file reference deve essere assolutamente (2 o 3) x step

    df_reference = np.loadtxt('OPTIMIZATION/icub_reference/'+task+'_ref/'+setup+'_icub_reference_'+str(exp)+'_'+task+'_denorm.csv', delimiter=',')############

    if(df_reference.shape[1]!=3 and df_reference.shape[1]!=2):
        print('DIMENSIONE INPUT ERRATA 1 icub')
        quit()
    if (df_reference.shape[0] != step):
        print('DIMENSIONE INPUT ERRATA 2 icub')
        quit()

    iCtrl.setTorqueMode(0)  # r_shoulder_pitch


    if control_variable == 3:
        iCtrl.setVelocityMode(3)  # elbow
        iCtrl.setVelocityMode(5)  # wrist_pitch

    time.sleep(0.001)


    start_time = time.time()

    iPos.positionMove(9, 0.0)  # su gazebo 24.75
    iPos.positionMove(10, 0.0)  # su gazebo 24.75

    for i in range(iterations):

        ######i file di reference presi da MATLAB o PYTHON  devono essere 3*20
        reference_values[0] = df_reference[i][0]#torque

        if control_variable==3:
            reference_values[1] = df_reference[i][1]  # torque
            reference_values[2] = df_reference[i][2]#speed
        #torque_values[2] = df[i][2]

        print ('iterazione', i)
        print ('reference_values', reference_values)

        ###SET TORQUE

        # Set Desired values in Torque Controller
        joint_index = [0, 3, 5]
        #if i==1:


        iTrq.setRefTorque(0, reference_values[0])#torque
        if control_variable == 3:
            iVel.velocityMove(3, reference_values[1])  # speed
            iVel.velocityMove(5, reference_values[2])#speed

        tempo_effettivo = time.time() - start_time
        time.sleep(0.01 - tempo_effettivo)  ##########################dovrebbe essere 0.01 per nuova rete?? e 0.1 per la vecchia
        start_time = time.time()

        encoder_pos = []
        for j in range(7):
            encoder_pos.append(iEnc.getEncoder(j))
            # print 'encoder pos',encoder_pos
        df_position.loc[i]['theta0_pos'] = encoder_pos[0]
        df_position.loc[i]['theta1_pos'] = encoder_pos[1]
        df_position.loc[i]['theta2_pos'] = encoder_pos[2]
        df_position.loc[i]['theta3_pos'] = encoder_pos[3]
        df_position.loc[i]['theta4_pos'] = encoder_pos[4]
        df_position.loc[i]['theta5_pos'] = encoder_pos[5]
        df_position.loc[i]['theta6_pos'] = encoder_pos[6]+wrist_offset

        print('encoder_pos', encoder_pos)

        ###END for

    df_position.to_csv('OPTIMIZATION/icub_result/'+setup+'_icub_validation' + str(exp) + '_' + str(task) + '.csv', sep=' ',decimal=',', index=False) ##############


# SETUP SECURITY CONFIGURATION

    if setup=='LSTM':
            setup_LSTM(iCtrl, iPos,iEnc)
            enco_initial_pos()
    elif setup=='NARX':
            setup_LSTM(iCtrl, iPos,iEnc)
            enco_initial_pos()
    else:
        print('SETUP SCONOSCIUTO')
        setup_LSTM(iCtrl,iPos,iEnc)#per sicurezza
        enco_initial_pos()

### END  ICUB   CODE




### START   GAZEBO    CODE
elif (gazebo=='on' and icub=='off'):
    print('GAZEBO')
    # sarah del tipo NARX_gazebo_reference_1_REACHING.csv
    print('pippo')
    df_reference = np.loadtxt('OPTIMIZATION/gazebo_reference/' + task + '_ref/' + setup + '_gazebo_reference_' + str(exp)+'_' + task + '_denorm.csv', delimiter=',')
    df_torques_lette = pd.DataFrame(index=(np.arange(0,iterations)), columns=['theta0','theta3']) #,'theta5'] )
    print(df_reference)

    # il file reference deve essere assolutamente (2 o 3) x step
    if(df_reference.shape[1]!=3 and df_reference.shape[1]!=2):
        print('DIMENSIONE GAZEBO ERRATA 1 icub')
        quit()
    if (df_reference.shape[0] != step):
        print('DIMENSIONE GAZEBO ERRATA 0 icub')
        quit()

    iCtrl.setTorqueMode(0)  # r_shoulder_pitch
    #iCtrl.setTorqueMode(3)  # elbow

    if control_variable == 3:
        iCtrl.setVelocityMode(3)  # wrist_pitch
        iCtrl.setVelocityMode(5)  # wrist_pitch

    time.sleep(0.001)


    for i in range(iterations):

        ######i file di reference presi da MATLAB o PYTHON  devono essere 3*20
        reference_values[0] = df_reference[i][0]  # torque
        #reference_values[1] = df_reference[i][1]  # torque
        #print('reference1', df_reference[0][i])
        #print('reference2', df_reference[1][i])

        if control_variable == 3:
            reference_values[1] = df_reference[i][1]  # speed
            reference_values[2] = df_reference[i][2]  # speed
            #print('reference3', df_reference[2][i])

        print('iterazione', i)
        print('torque_values', reference_values)

        ###SET TORQUE

        # Set Desired values in Torque Controller
        joint_index = [0, 3, 5]

        iTrq.setRefTorque(0, reference_values[0])#torque
        #iTrq.setRefTorque(3, reference_values[1])#torque
        if control_variable == 3:
            iVel.velocityMove(3, reference_values[1])  # speed
            iVel.velocityMove(5, reference_values[2])#speed



        for j in [0]:
            df_torques_lette.loc[i]['theta' + str(j)] = iTrq.getTorque(j)
        #df_torques_lette.loc[i]['theta5']=iVel.getSpeed(5) #metodo non implementato

        # time step msecond
        runner.advance_simulation(10)  # must be 100 = 0.1sec

        encoder_pos = []
        for j in range(7):
            encoder_pos.append(iEnc.getEncoder(j))
        # print 'encoder pos',encoder_pos
        df_position.loc[i]['theta0_pos'] = encoder_pos[0]
        df_position.loc[i]['theta1_pos'] = encoder_pos[1]
        df_position.loc[i]['theta2_pos'] = encoder_pos[2]
        df_position.loc[i]['theta3_pos'] = encoder_pos[3]
        df_position.loc[i]['theta4_pos'] = encoder_pos[4]
        df_position.loc[i]['theta5_pos'] = encoder_pos[5]
        df_position.loc[i]['theta6_pos'] = encoder_pos[6]#+15.3

        print('encoder_pos', encoder_pos)
    #end for
    #exp=4
    df_torques_lette.to_csv('verifiche_torque_lette.csv', sep=';', decimal=',', index=False)
    df_position.to_csv('OPTIMIZATION/gazebo_result/' + setup + '_gazebo_validation' + str(exp) + '_' + str(task) + '.csv', sep=' ', decimal=',', index=False)

    print('FINE')
    time.sleep(1)
# SETUP SECURITY CONFIGURATION
    if setup=='LSTM':
        print('SETUP LSTM')
        setup_gazebo_LSTM(iCtrl, iPos, runner, iEnc)
        enco_initial_pos()
    elif setup=='NARX':
        print('SETUP NARX')
        setup_gazebo_NARX(iCtrl, iPos,runner,iEnc)
        enco_initial_pos()
    else:
        print('SETUP SCONOSCIUTO')
        setup_gazebo_NARX(iCtrl,iPos,runner,iEnc)#per sicurezza
        enco_initial_pos()
### END      GAZEBO     CODE


else:
    print('SETUP GAZEBO SBAGLIATO FINE')
    quit()
