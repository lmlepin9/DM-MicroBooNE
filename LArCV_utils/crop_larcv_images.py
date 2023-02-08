# IMPORTANT: IF NOT IN UBOONE-BUILD, USE LARCV1 CONTAINER TO RUN THIS SCRIPT 

import os, sys, ROOT                                                    
import numpy as np
import pandas as pd
import glob  
from larcv import larcv
larcv.LArbysLoader()
import cv2
from larcv_projection_helper import utility as u


def CropFunction(input_csv, input_root, output_root):
    # Open relevant info
    print('Openning csv info...')
    df = pd.read_csv(input_csv)
    print('\n')
    print('Initializing IO manager for LArCV...')
    iom = larcv.IOManager(larcv.IOManager.kREAD)
    iom.add_in_file(input_root)
    iom.initialize()

    io_output_b = larcv.IOManager(larcv.IOManager.kWRITE)
    io_output_b.set_out_file(output_root)
    io_output_b.initialize()
    print('\n')

    projection_plane =2
    plane = 0
    event_out = 1 

    print('Iterating over the sample')
    for entry in range(iom.get_n_entries()):
        
        if(entry%1000 == 0):
            print('Entry: ', entry)
            print('\n')
        
        iom.read_entry(entry)
        ev_img = iom.get_data(larcv.kProductImage2D,"wire")
        run_vertex = ev_img.run()
        subrun_vertex = ev_img.subrun()
        event_vertex = ev_img.event()   
        index_array = df.query('run_number == {:2d} & subrun_number == {:2d} & event_number == {:2d} '.format(run_vertex,subrun_vertex,event_vertex)).index.values
        if(len(index_array) == 0):
            continue
        else: 
            x_2d = ROOT.Double()
            y_2d = ROOT.Double()
            vertex_index = index_array[0]
            vx , vy ,vz = df['reco_vertex_x'][vertex_index], df['reco_vertex_y'][vertex_index], df['reco_vertex_z'][vertex_index]
            whole_img = ev_img.at(plane)
            whole_image=larcv.as_ndarray(whole_img)
            whole_img.reset_origin(0, 8448)
            larcv.Project3D(whole_img.meta(), vx, vy, vz, 0,  projection_plane, x_2d, y_2d)      
            meta_crop = larcv.ImageMeta(512,512*6,512,512,0,0,plane)   
            meta_origin_x, meta_origin_y = u.Meta_origin_helper(x_2d, y_2d, verbose=0)
            meta_crop.reset_origin(meta_origin_x, meta_origin_y)
            image_vtx = ev_img.at(plane).crop(meta_crop)
            img_vtx = larcv.as_ndarray(image_vtx)
            img_vtx = np.where(img_vtx<10 ,0  ,img_vtx)
            img_vtx = np.where(img_vtx>500,500,img_vtx)
            
        
            image2d_array_output = io_output_b.get_data(larcv.kProductImage2D, 'image2d_binary')
            image_as_2d = larcv.as_image2d(img_vtx)
            image2d_array_output.Append(image_as_2d)
            
            # Run 100 for signal
            io_output_b.set_id(run_vertex,subrun_vertex,event_vertex)
            io_output_b.save_entry()
            event_out += 1

    print('\n')
    print('Cropped finished, closing LArCV outputs')

    io_output_b.finalize()




def CropDetVar(run):
    input_dir = "/pnfs/uboone/persistent/users/lmoralep/larcv_files/" + run + "_det_variations/"
    csv_dir = "/uboone/data/users/lmoralep/csv_files/"
    output_dir = "/pnfs/uboone/persistent/users/lmoralep/larcv_files/" + run + "_det_variations/"

    det_vars=[
        'CV',
        'LYAttenuation',
        'LYDown',
        'LYRayleigh',
        'Recomb',
        'SCE',
        'WireModdEdX',
        'WireModThetaXZ',
        'WireModThetaYZ_withouts',
        'WireModThetaYZ_withs',
        'WireModX',
        'WireModYZ'
    ]

    print("Processing detector variations of : " + run)
    for var in det_vars: 
        print("Current variation: " + var)
        input_csv = csv_dir + run + "_" + var + "_" + "CNN.csv"
        input_larcv =  input_dir + run + "_" + var + "_" + "larcv.root"
        output_cropped = output_dir + run + "_" + var + "_" + "larcv_cropped.root"


        CropFunction(input_csv, input_larcv , output_cropped)



def CropSample(run):
    input_dir = "/pnfs/uboone/persistent/users/lmoralep/larcv_files/" + run + "_samples/"
    csv_dir = "/uboone/data/users/lmoralep/csv_files/"
    output_dir = "/pnfs/uboone/persistent/users/lmoralep/larcv_files/" + run + "_samples/"

    samples = [
        "nu_overlay",
        "dirt",
        "offbeam"
    ]

    for sample in samples:
        print("Processing sample:" + sample)
        input_csv = csv_dir + run + "_" + sample + "_" + "CNN.csv"
        subsample_index = 0 
        for subsample in glob.glob(input_dir + run + "_NuMI_" + sample + "_larcv_set*"):
            output_cropped = output_dir + run + "_" + sample + "_set" + str(subsample_index) + "_larcv_cropped.root"
            CropFunction(input_csv, subsample, output_cropped)
            subsample_index+=1 



if __name__ == "__main__":
    CropSample("run3")