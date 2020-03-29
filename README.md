# Cartesian Model-Free Control

This repository hosts the code used to train and use a cartesian model-free controller for robotic arms. The model, similar to (Thuruthel et al., 2018), learns the forward dynamic model via a neural network which training set is collected via a motor babbling process. Then, an optimization procedure uses the learned forward model to find the optimal actuation sequences to perform tasks such as reaching and throwing of objects. Two different implementations are providede, one that uses LSTM (in PyTorch) and another that uses NARX (in MATLAB).

## Dependencies

The C++ code for the motor babbling process depends on [this](https://bitbucket.org/lore_ucci/robolibs) library.