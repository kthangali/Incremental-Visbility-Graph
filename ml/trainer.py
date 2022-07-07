import numpy as np
from argparse import ArgumentParser
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

import os
import pdb
from collections import defaultdict

import torch
import torchvision
from torch import nn as nn

## Importing ML stuff
from model_utils import identity, simple_CNN, simple_MLP
import pytorch_utils as ptu
import visualizer
from createDataset import LocalHeuristicDataset

def get_dataset(filepath, isTrain, size):
    # "/home/rishi/Desktop/CMU/Research/search-zoo/data.npz"
    dataset = LocalHeuristicDataset(filepath, size, "improvement")
    return dataset


class TrainingLoop:
    def __init__(self, train_data_loader, test_data_loader, training_dataset, test_dataset, my_model, loss_function):
        self.train_data_loader = train_data_loader
        self.test_data_loader = test_data_loader
        self.training_dataset = training_dataset
        self.test_dataset = test_dataset
        self.my_model = my_model
        self.loss_function = loss_function
        self.optimizer = torch.optim.Adam(self.my_model.parameters(), lr=1e-3)

        self.logDict = defaultdict(list)


    def prepare_inputs(self, sampled_batch, isNumpy=False):
        obs, hvals, newHVal = sampled_batch
        if isNumpy:
            obs, hvals, newHVal = ptu.from_numpy(obs), ptu.from_numpy(hvals), ptu.from_numpy(newHVal)
        inputs = torch.stack([obs, hvals], dim=1).reshape((obs.shape[0], -1)) # (B,2,D,D)
        newHVal = newHVal.unsqueeze(1) # (B)->(B,1)
        return inputs.to(ptu.device).float(), newHVal.to(ptu.device).float()

    def run_training_epoch(self):
        self.my_model.train()
        all_losses = []
        total_accuracy = []

        for i_batch, sampled_batch in enumerate(self.train_data_loader):
            inputs, targets = self.prepare_inputs(sampled_batch)  #(B,2,D,D), (B)

            self.optimizer.zero_grad()
            preds = self.my_model.forward(inputs)  # preds (float)
            loss = self.loss_function(preds,targets)

            loss.backward()
            self.optimizer.step()
            all_losses.append(loss.detach().item())  # Regular number

        stats = dict(
            type = "Training",
            loss = np.mean(all_losses),
        )
        self.logDict["TrainLoss"].append(np.mean(all_losses))
        return stats

    def run_testing_epoch(self):
        self.my_model.eval()
        all_losses = []
        total_accuracy = []

        for i_batch, sampled_batch in enumerate(self.test_data_loader):
            # t1 = time.time()
            inputs, targets = self.prepare_inputs(sampled_batch)  # (B,1,D,D), (B)
            loss, reconstruction_images = self.my_model.compute(inputs)  # loss (float), input, (B,1,D,D)
            all_losses.append(loss.detach().item())
            # inputs, targets = self.prepare_inputs(sampled_batch)  #(B,1,D,D), (B)
            # predictions = self.my_model(inputs) # (B,10)
            # loss = self.loss_function(predictions, targets)
            #
            # accuracy = torch.where(torch.argmax(predictions, dim=1) == targets, ptu.ones_like(targets),
            #                        ptu.zeros_like(targets)).sum()/targets.shape[0]
            # total_accuracy.append(accuracy.item())
            #
            # all_losses.append(loss.item())

        stats = dict(
            type = "Testing",
            loss = np.mean(all_losses),
            # total_accuracy=np.mean(total_accuracy),
            # last_inputs = inputs.detach(),
            # last_targets = targets.detach(),
            # last_preds = predictions.detach()
        )
        return stats

    def visualize_values(self, outputFolder, iter):
        # pdb.set_trace()
        K = 10
        # D = self.training_dataset.D
        rand_indices = np.random.randint(0, len(self.training_dataset), K)
        sampled_batch = [self.training_dataset[i] for i in rand_indices]  # np, ((),(),())xK
        sampled_batch = list(map(np.stack, zip(*sampled_batch)))  # np
        inputs, targets = self.prepare_inputs(sampled_batch, isNumpy=True)  # torch
        preds = self.my_model.forward(inputs)  # torch

        # captions = []
        # for pred, target in zip(preds, targets):
        #     captions.append("Pred: {}, True:{}".format(pred.item(), target.item()))
        # captions = np.array(captions).reshape((2, K//2))  # np

        # targets = targets.view(2, K//2)  # (2, 5)
        # preds = preds.view(2, K//2)  # (2, 5)
        # images = ptu.from_numpy(sampled_batch[0]).repeat(1, 3, 1, 1)  # (B=10,1*3,D,D)
        # images = images.view(2, K//2, 3, D, D)

        # visualizer.plot_multi_image(ptu.get_numpy(images), save_dir="guess.png", caption=captions)

        sampled_batch.append(ptu.get_numpy(preds.flatten()))
        data = list(zip(*sampled_batch))
        visualizer.plotCustomMulti(visualizer.plotLH, data, (2, K//2), "{}/values_{}.png".format(outputFolder, iter))

        # pdb.set_trace()
        # visualizer.plot_multi_image(ptu.get_numpy(images), save_dir="true.png", caption=ptu.get_numpy(targets))
        # visualizer.plot_multi_image(ptu.get_numpy(images), save_dir="guess.png", caption=ptu.get_numpy(preds))
        # pdb.set_trace()
        plt.figure()
        plt.plot(self.logDict["TrainLoss"])
        plt.savefig("{}/trainLossCurve.png".format(outputFolder))
        plt.close('all')

    def visualize(self, stats):
        print("Type: {}, Loss: {}, Accuracy: {}".format(stats["type"], stats["loss"], 5)) # stats["total_accuracy"]))

    def saveModel(self, outputFolder):
        ### Generate fake data
        # sampled_batch = [self.training_dataset[i] for i in [0,1]]  # np, ((),(),())x2
        # sampled_batch = list(map(np.stack, zip(*sampled_batch)))  # np
        # inputs, targets = self.prepare_inputs(sampled_batch, isNumpy=True)  # torch
        # traced_model = torch.jit.trace(self.my_model, inputs)
        sm = torch.jit.script(self.my_model)
        sm.save("{}/my_module_model.pt".format(outputFolder))
        

def practice_classifier(args):
    np.random.seed(args.seed)
    torch.manual_seed(args.seed)

    outputFolder = "/home/rishi/Desktop/CMU/Research/search-zoo/logs/{}".format(args.outputFolder)
    if not os.path.exists(outputFolder):
        os.makedirs(outputFolder)

    using_gpu = args.which_gpu >= 0
    batch_size = 32
    windowHalfSize = 5
    ### Create MLP model
    mlp_info = dict(
        input_size = pow(windowHalfSize*2+1,2)*2,
        sizes=[100, 1],
        hidden_activation=nn.ELU(),
        output_activation=[identity, nn.ReLU()][0]
    )
    my_model = simple_MLP(**mlp_info)
    my_model.to(ptu.device)

    ### Create CNN model
    # cnn_info = dict(
    #     input_dim = windowHalfSize*2+1, # 11->
    #     input_channels = 2,
    #     kernel_sizes = [5, 5],
    #     strides = [2, 2],
    #     paddings = [2, 2],
    #     max_poolsizes = [],
    #     hidden_activation=[nn.ReLU(), nn.ELU()][0],
    #     output_activation=[identity, nn.ReLU()][0]
    # )
    # my_model = simple_CNN(**cnn_info)

    train_dataset = get_dataset(args.datapath, True, args.size)
    test_dataset = get_dataset(args.datapath, False, args.size)
    train_data_loader = torch.utils.data.DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=0,
                                                    pin_memory=using_gpu)
    test_data_loader = torch.utils.data.DataLoader(test_dataset, batch_size=batch_size, shuffle=True, num_workers=0,
                                                   pin_memory=using_gpu)

    # Supervised training loop: num_epochs, objective
    my_trainer = TrainingLoop(train_data_loader, test_data_loader, train_dataset, test_dataset, my_model, nn.MSELoss())
    for i in range(51):
        training_stats = my_trainer.run_training_epoch()
        print(training_stats)
        if i%10 == 0:
            my_trainer.visualize_values(outputFolder, i)
            # pdb.set_trace()
        # my_trainer.visualize(training_stats)

        # my_model.visualize(mnist_train_dataset)

        # testing_stats = my_trainer.run_testing_epoch()
        # my_trainer.visualize(testing_stats)
        # my_trainer.visualize_autoencoder()
        # my_trainer.visualize_values()
        my_trainer.saveModel(outputFolder)

    # Visualizations


####### Main interface #######
## Example run: CUDA_VISIBLE_DEVICES=1 python mnist_fun.py -gpu 0
if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("datapath", help=".npz filepath", type=str)
    parser.add_argument("outputFolder", help="output folder", type=str)
    parser.add_argument('-gpu', '--which_gpu', type=int, default=-1, required=False)
    parser.add_argument('-s', '--size', type=int, default=-1, required=False)
    parser.add_argument('-seed', '--seed', type=int, default=1, required=False)
    args = parser.parse_args()

    ptu.set_device(args.which_gpu)  # Set gpu or cpu device

    practice_classifier(args)