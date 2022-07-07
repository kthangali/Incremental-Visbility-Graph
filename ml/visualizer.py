import numpy as np
import matplotlib.pyplot as plt
from torchvision.utils import save_image
import pdb

###############################################################
######### LH specific visualization functions below ############
def processData(data):
    """Input: data->tuple (***) """
    # https://stackoverflow.com/questions/12974474/how-to-unzip-a-list-of-tuples-into-individual-lists
    return list(zip(*data))

def plotCustomMulti(plotFunction, data, shape, filename):
    ncols,nrows = shape
    fig = plt.figure(figsize=(ncols*6+4,nrows*6))

    count = 1
    for x in range(nrows):
        for y in range(ncols):
            ax = plt.subplot(nrows, ncols, count)
            plotFunction(ax, *data[count-1])
            count += 1
    plt.savefig(filename)
    plt.close('all')

def plotLH(ax, obs, hvals, targetVal, predVal=None):
    """Note: obs should have 0 free, 1 invalid"""
    D = hvals.shape[0]

    plt_map = np.ma.masked_where(obs==0, obs)  # Mask out free space, just show obstacles
    ax.imshow(plt_map, cmap="binary", vmin=0, vmax=1,
            interpolation='none', origin='lower')
    
    hvals = np.ma.masked_where(obs==1, hvals)  # Mask out obstacles
    im = ax.imshow(hvals, alpha=1, interpolation='none', origin='lower')
    plt.colorbar(im, ax=ax)

    if predVal is not None:
        ax.set_title("True: {:.3}, Pred: {:.3}".format(targetVal, predVal))
    else:
        ax.set_title("True: {:.3}".format(targetVal))

###############################################################
######### Generic visualization functions below ################

# Input: images (W,H,3,D,D), pytorch tensors, values should be between 0 and 1
def create_image_from_subimages(images, file_name):
    cur_shape = images.shape
    images = images.permute(1, 0, 2, 3, 4).contiguous()  # (H,W,3,D,D)
    images = images.view(-1, *cur_shape[-3:])  # (H*W, 3, D, D)
    save_image(images, fp=file_name, nrow=cur_shape[0])
    #Note: Weird that it is cur_shape[0] but cur_shape[1] produces incorrect image


# Input: imgs (H,W,D,D,3) or (H,W,3,D,D), numpy arrays, values ints between 0-225
def plot_multi_image(imgs, save_dir, caption=None):
    if imgs.shape[-1] != 3:
        imgs = np.moveaxis(imgs, 2, -1)
    if imgs.dtype is not np.uint8:
        imgs = np.clip((imgs * 255), 0, 255).astype(np.uint8)

    rows, cols, imsize, imsize, _ = imgs.shape

    fig = plt.figure(figsize=(6*cols, 6*rows))
    ax = []
    count = 1
    for i in range(rows):
        for j in range(cols):
            ax.append(fig.add_subplot(rows, cols, count))
            ax[-1].set_yticklabels([])
            ax[-1].set_xticklabels([])
            if caption is not None:
                ax[-1].set_title("{}".format(caption[i][j]))
                # ax[-1].set_title('%0.4f' % caption[i][j])
            plt.imshow(imgs[i, j])
            count += 1

    plt.savefig(save_dir)
    plt.close('all')