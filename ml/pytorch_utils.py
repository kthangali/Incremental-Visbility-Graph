# import torch
# import numpy as np
# import pdb


# ############################# Start generic helpful functions #########################
# # Input: vals (***), num_classes N (int)
# # Ouput: (***,N) one hot
# def custom_one_hot(vals, num_classes):
#     the_shape = vals.shape  # (***)
#     tmp = torch.eye(num_classes).index_select(0, vals.flatten())  # (***)
#     tmp = tmp.view([*the_shape] + [num_classes])  # (***,N)
#     return tmp

# ############################# End generic helpful functions #########################

# ############################# Start other functions #########################
# def check_nan(list_of_tensors, folder=None):
#     there_is_a_nan = False
#     for tensor in list_of_tensors:
#         if tensor is not None and torch.isnan(tensor).any():
#             there_is_a_nan = True
#             break

#     if there_is_a_nan:
#         if folder: #Create a txt that informs users checking log folder that a nan has occured
#             f = open(folder + "/NAN_ALERT.txt", "w")
#             f.close()
#         pdb.set_trace()


# ############################# End other functions #########################



# ############################# Start pytorch gpu functions #############################
# device = None

# def set_device(gpu_id=-1):
#     global device
#     device = torch.device("cuda:" + str(gpu_id) if gpu_id > -1 else "cpu")

# # noinspection PyPep8Naming
# def FloatTensor(*args, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.FloatTensor(*args, **kwargs, device=torch_device)


# def from_numpy(*args, **kwargs):
#     return torch.from_numpy(*args, **kwargs).float().to(device)


# def get_numpy(tensor):
#     return tensor.to('cpu').detach().numpy()


# def zeros(*sizes, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.zeros(*sizes, **kwargs, device=torch_device)


# def ones(*sizes, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.ones(*sizes, **kwargs, device=torch_device)


# def ones_like(*args, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.ones_like(*args, **kwargs, device=torch_device)


# def randn(*args, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.randn(*args, **kwargs, device=torch_device)


# def zeros_like(*args, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.zeros_like(*args, **kwargs, device=torch_device)


# def tensor(*args, torch_device=None, **kwargs):
#     if torch_device is None:
#         torch_device = device
#     return torch.tensor(*args, **kwargs, device=torch_device)


# def normal(*args, **kwargs):
#     return torch.normal(*args, **kwargs).to(device)
# #############################End pytorch gpu functions#############################