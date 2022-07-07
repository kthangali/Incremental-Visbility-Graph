import torch
from torch import nn as nn
import torch.nn.functional as F

import numpy as np
import pdb

import pytorch_utils as ptu

def identity(x):
    return x

class simple_CNN(nn.Module):
    def __init__(
            self,
            input_dim,
            input_channels,
            kernel_sizes,
            n_channels,
            strides,
            paddings,
            max_pool_sizes,  # Max pool should be in the form [int, ...], use 1 to indicate no max pool
            hidden_init=nn.init.xavier_uniform_,
            hidden_activation=nn.ReLU(),
            output_activation=identity,
            output_flatten=False,
            mlp_sizes=None):

        assert len(kernel_sizes) == len(n_channels) == len(strides) == len(paddings) == len(max_pool_sizes)
        super().__init__()

        self.input_dim = input_dim
        self.input_channels = input_channels
        self.kernel_sizes = kernel_sizes
        self.n_channels = n_channels
        self.strides = strides
        self.paddings = paddings
        self.max_pool_sizes = max_pool_sizes
        self.max_pools = []
        self.mlp_sizes = mlp_sizes
        for a_size in max_pool_sizes:
            if a_size is None:
                self.max_pools.append(identity)
            else:
                self.max_pools.append(nn.MaxPool2d(a_size))

        self.output_activation = output_activation
        self.hidden_activation = hidden_activation
        self.output_flatten = output_flatten

        self.conv_layers = nn.ModuleList()

        for out_channels, kernel_size, stride, padding in \
                zip(n_channels, kernel_sizes, strides, paddings):
            conv = nn.Conv2d(input_channels, out_channels, kernel_size, stride=stride, padding=padding)
            hidden_init(conv.weight)
            conv.bias.data.fill_(0)

            self.conv_layers.append(conv)
            input_channels = out_channels

        if mlp_sizes is not None:  # Add mlp modules
            assert output_flatten  # output_flatten needs to be true for mlp to work afterwards
            self.mlp_layers = nn.ModuleList()
            input_size = self.get_output_dim(include_mlp=False)[-1]

            for a_size in mlp_sizes:
                mlp = nn.Linear(input_size, a_size)
                hidden_init(mlp.weight)

                self.mlp_layers.append(mlp)
                input_size = a_size
        else:
            self.mlp_layers = None

        print("CNN dims: {}".format(self.get_output_dim()))

    # Input: input is (B,C,D,D)
    def forward(self, input):
        for i in range(len(self.conv_layers)):
            input = self.hidden_activation(self.conv_layers[i](input))
            input = self.max_pools[i](input)

        if self.output_flatten:
            input = input.reshape((input.shape[0], -1))  # (B,C*D*D)
        if self.mlp_layers is not None:
            for layer in self.mlp_layers:
                input = self.hidden_activation(layer(input))
        input = self.output_activation(input)
        return input

    def get_output_dim(self, include_mlp=True):
        cur_dim = self.input_dim
        all_dims = [cur_dim]
        for i in range(len(self.conv_layers)):
            cur_dim = (cur_dim + 2*self.paddings[i] - (self.kernel_sizes[i]-1))//self.strides[i]
            cur_dim = cur_dim // self.max_pool_sizes[i]
            all_dims.append(cur_dim)
        if self.output_flatten:
            all_dims.append("Flatten")
            all_dims.append(self.n_channels[-1]*cur_dim*cur_dim)
        if include_mlp and self.mlp_sizes is not None:
            all_dims.extend(self.mlp_sizes)
        return all_dims


class simple_MLP(nn.Module):
    def __init__(
            self,
            input_size,
            sizes,
            hidden_init=nn.init.xavier_uniform_,
            hidden_activation=nn.ReLU(),
            output_activation=identity):
        super().__init__()

        self.input_size = input_size
        self.sizes = sizes
        self.hidden_init = hidden_init
        self.hidden_activation = hidden_activation
        self.output_activation = output_activation

        self.mlp_layers = nn.ModuleList()
        for a_size in sizes:
            mlp = nn.Linear(input_size, a_size)
            hidden_init(mlp.weight)

            self.mlp_layers.append(mlp)
            input_size = a_size

    def forward(self, input):
        for layer in self.mlp_layers:
            input = self.hidden_activation(layer(input))
        return self.output_activation(input)

