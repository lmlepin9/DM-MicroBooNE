#!/usr/bin/env python

##########################################################
#	Event display for larcv-root files 
#       This script should be run with Larcv1 or in uboone-build 
##########################################################



import os, sys, ROOT                                                    
import numpy as np
import pandas as pd

import matplotlib as mpl 
# Backend that can't display a figure Agg 
mpl.use('Agg')

import matplotlib.pyplot as plt
from larcv import larcv
import gc




def EvDisp(input_file, tag, entry, producer="wire"):
    print("Your input file is :", input_file)
    print("Displaying entry: ", entry)
    fig_dir = "/uboone/data/users/lmoralep/CNN_ev_disp/"
    plane = 0 # Hard-coded to collection plane, the one used in the main CNN analysis 
    iom = larcv.IOManager(larcv.IOManager.kREAD)
    iom.add_in_file(input_file)
    iom.initialize()
    print("Number of entries in this file: " + str(iom.get_n_entries()))
    # Access entry
    img_plane= plane 
    iom.read_entry(int(entry))
    # Access a product instance (type,label) = (image2d,data)
    image2d_data = iom.get_data(larcv.kProductImage2D,producer)
    print("Selected entry: " + str(entry))
    print("Run: " + str(image2d_data.run()))
    print("Subrun: " + str(image2d_data.subrun()))
    print("Event: " + str(image2d_data.event()))
    whole_img = image2d_data.at(img_plane)
    whole_image=larcv.as_ndarray(whole_img)
    plt.figure(figsize=(15,6),dpi=300)
    plt.imshow(whole_image.transpose(), cmap="jet",origin="lower")
    plt.axis("off")
    plt.savefig(fig_dir + tag + "_entry_" + str(entry) + "_.png", bbox_inches='tight', pad_inches=0.)
    iom.finalize()
    gc.collect()



def DisplayLoop():
    for i in range(100):
        EvDisp("/pnfs/uboone/persistent/users/lmoralep/cnn_training/run1_numi_nu_MC_larcv_cnn_training.root", "run1_numi_cnn_training", i)


if __name__ == "__main__":
    #EvDisp(sys.argv[1], sys.argv[2], sys.argv[3]) 
    DisplayLoop()
