import pandas as pd
import numpy as np 
import functools


'''

Utility functions to process NuMI MC samples 

'''



def extend_array(var_arr,weight_arr):
    out_var = []
    out_weight = []
    for i in range(len(var_arr)):
        #print(var_arr[i])
        temp_row = var_arr[i]
        if(len(temp_row) == 0):
            continue

        elif (len(temp_row) == 1):
            out_var.append(temp_row[0])
            out_weight.append(weight_arr[i])
        
        else:
            for j in temp_row:
                out_var.append(j)
                out_weight.append(weight_arr[i])

    return out_var, out_weight




def logit_transform(score):
    return np.log(score/(1-score))

def filter_df(df,score_label):
    df = df[df[score_label] >= 0.5]
    return df 


def make_unique_ev_id(df): #df must have 'run', 'sub' and 'evt' branches
    if pd.Series(['run_number', 'subrun_number', 'event_number']).isin(df.columns).all():
        rse_list = []
        for entry in df.index: #Looping over all events in the dataframe
            rse = str(df['run_number'][entry]) + "_" + str(df['subrun_number'][entry]) + "_" + str(df['event_number'][entry])
            rse_list.append(rse)
        df['rse_id'] = rse_list #Writing a new branch with the unique event id
        return df.copy()
    else:
        print("Dataframe needs \"run\", \"sub\" and \"evt\" columns.")
        return 0


def make_common_evs_df(df_list):
    overlapping_df = functools.reduce(lambda left,right: pd.merge(left, right, on=['rse_id'], how='inner'), df_list)
    print("Length is of common events list is " + str(len(overlapping_df)))
    return overlapping_df

def Edit_Weight_Tune(df_to_Tune):
     #This is taken from Aditya's code, Owen also has the same in his for overlay and dirt, there is the same block in PELEE code
    df_to_Tune.loc[ df_to_Tune['weightSplineTimesTune'] <= 0, 'weightSplineTimesTune' ] = 1.
    df_to_Tune.loc[ df_to_Tune['weightSplineTimesTune'] == np.inf, 'weightSplineTimesTune' ] = 1.
    df_to_Tune.loc[ df_to_Tune['weightSplineTimesTune'] > 50, 'weightSplineTimesTune' ] = 1.
    df_to_Tune.loc[ np.isnan(df_to_Tune['weightSplineTimesTune']) == True, 'weightSplineTimesTune' ] = 1.
    return df_to_Tune

def MC_weight_branch(df_MC): 
    #Writes a new branch called "weight" including, ppfx, weightSplineTimesTune AND if pi0 are present, scales by pi0 factor
    df_MC = Edit_Weight_Tune(df_MC)
    df_MC["weight"] = df_MC["ppfx_cv"]*df_MC["weightSplineTimesTune"] 

    #not sure if the next line is relevant for my analysis...
    #df_MC.loc[df_MC["npi0"]>0,"weight"] = df_MC["weight"][df_MC["npi0"]>0]*Constants.pi0_scaling_factor #If MC event contains pi0, need to scale down, derived from BNB data


10522703.0

'''
df = pd.read_pickle("/home/lmlepin/Desktop/dm_sets/dark_tridents_analysis/run1_samples/run1_nu_overlay_CNN.pkl")
flash_arr = df['reco_flash_time']
df = df.rename(columns={"ppfx_cv_good":"ppfx_cv", "spine_tune_good":"weightSplineTimesTune"})
MC_weight_branch(df)
weights = df['weight']



flash_ext, weight_ext = extend_array(flash_arr.to_list(),weights)
print(flash_ext[:100])
print(weight_ext[:100])
'''