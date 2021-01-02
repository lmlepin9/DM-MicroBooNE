# IMPORTANT: USER LARCV1 CONTAINER TO RUN THIS SCRIPT 

import os, sys, ROOT                                                    
import numpy as np
import pandas as pd


import torch
import torch.nn as nn
import torch.optim as optim
import torch.utils.data.dataloader as dataloader


from mpid_net import mpid_net, mpid_func
from larcv import larcv
larcv.LArbysLoader()
import cv2
from larcv_projection_helper import utility as u

training_number = 20006

# Open relevant info
print('Openning csv info...')
df = pd.read_csv("dm_vertex_all.csv")
di = pd.read_csv("dm_info_all.csv")
print('\n')


print('Initializing IO manager for LArCV...')
iom = larcv.IOManager(larcv.IOManager.kREAD)
iom.add_in_file("../dm_all_larcv.root")
iom.initialize()

io_output = larcv.IOManager(larcv.IOManager.kWRITE)
io_output.set_out_file('../dm_training_set.root')
io_output.initialize()

io_output_b = larcv.IOManager(larcv.IOManager.kWRITE)
io_output_b.set_out_file('../dm_test_set.root')
io_output_b.initialize()
print('\n')

plane = 2
event_out = 1 

print('Iterating over whole DM sample...')
for entry in range(iom.get_n_entries()):
    
    if(entry%1000 == 0):
      print('Entry: ', entry)
      print('\n')
    
    iom.read_entry(entry)
    ev_img = iom.get_data(larcv.kProductImage2D,"wire")
    run_vertex = ev_img.run()
    subrun_vertex = ev_img.subrun()
    event_vertex = ev_img.event()   
    index_array = di.query('run == {:2d} & subrun == {:2d} & event == {:2d}'.format(run_vertex,subrun_vertex,event_vertex)).index.values
    
    x_2d = ROOT.Double()
    y_2d = ROOT.Double()
    vertex_index = index_array[0]
    vx , vy ,vz = df['v_x'][vertex_index], df['v_y'][vertex_index], df['v_z'][vertex_index]
    
    whole_img = ev_img.at(plane)
    whole_image=larcv.as_ndarray(whole_img)
    whole_img.reset_origin(0, 8448)
    larcv.Project3D(whole_img.meta(), vx, vy, vz, 0,  plane, x_2d, y_2d)
    
            
    meta_crop = larcv.ImageMeta(512,512*6,512,512,0,0,plane)   
    meta_origin_x, meta_origin_y = u.Meta_origin_helper(x_2d, y_2d, verbose=0)
    meta_crop.reset_origin(meta_origin_x, meta_origin_y)
    
    image_vtx = ev_img.at(plane).crop(meta_crop)
    img_vtx = larcv.as_ndarray(image_vtx)
    img_vtx = np.where(img_vtx<10 ,0  ,img_vtx)
    img_vtx = np.where(img_vtx>500,500,img_vtx)
    
    if(entry <= training_number):
        image2d_array_output = io_output.get_data(larcv.kProductImage2D, 'image2d_binary')
        image_as_2d = larcv.as_image2d(img_vtx)
        image2d_array_output.Append(image_as_2d)
        io_output.set_id(100,1,event_out)
        io_output.save_entry()
        event_out += 1
    else:
        image2d_array_output = io_output_b.get_data(larcv.kProductImage2D, 'image2d_binary')
        image_as_2d = larcv.as_image2d(img_vtx)
        image2d_array_output.Append(image_as_2d)
        io_output_b.set_id(100,1,event_out)
        io_output_b.save_entry()
        event_out += 1

print('\n')
print('Cropped finished, closing LArCV outputs')

io_output.finalize()
io_output_b.finalize()