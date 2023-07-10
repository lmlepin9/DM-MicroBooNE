'''

Functions to create LaTex tables


'''

import pandas as pd
import numpy as np 
from math import floor


def ParseNumber(number):
    exponent = floor(np.log10(number))
    base = number*10**(-1*exponent)
    return [base, exponent] 


def CompareParse(input_a,input_b):
    number_a = ParseNumber(input_a)
    number_b = ParseNumber(input_b)

    base_a = number_a[0]
    exp_a = number_a[1]
    base_b = number_b[0]
    exp_b = number_b[1]

    if(base_b < base_a):
        base_a*=(10**(exp_a - exp_b))
        exp_a = exp_b
    else:
        pass
    return [base_a, base_b, exp_a]


def LimitsTable(input_df):

    masses = input_df["mass"].to_numpy()
    observed = input_df["observed"].to_numpy()
    expected = input_df["epsilon_squared"].to_numpy()
    two_up = input_df["two_sig_up"].to_numpy()
    one_up = input_df["one_sig_up"].to_numpy()
    one_down = input_df["one_sig_down"].to_numpy()
    two_down = input_df["two_sig_down"].to_numpy()


    # Initialize buffer with:
    buffer = "$ \\boldsymbol{M_{A^\prime}} \\, \\boldsymbol{\\left[ \\textrm{GeV} \\right]}$ & \\textbf{Observed} & \\textbf{Expected} & \\textbf{Exp.} $\\boldsymbol{1\sigma}$ & \\textbf{Exp.} $\\boldsymbol{2\sigma}$ \\\\ \n"

    # Add horizontal lines:
    buffer+= "\\hline \n"
    buffer+= "\\hline \n"


    for i in range(len(observed)):
 
        obs_temp = ParseNumber(observed[i])
        exp_temp = ParseNumber(expected[i])
        one_band = CompareParse(one_down[i],one_up[i])
        two_band = CompareParse(two_down[i],two_up[i])


        obs_string = r"{:.2f}$\times 10^{{{exp}}}$".format(obs_temp[0], exp=str(obs_temp[1]))
        exp_string = r"{:.2f}$\times 10^{{{exp}}}$".format(exp_temp[0], exp=str(exp_temp[1]))
        two_sig_string = r"({:.2f} - {:.2f})$\times 10^{{{exp}}}$ ".format(two_band[0],two_band[1],exp=str(two_band[2]))
        one_sig_string = r"({:.2f} - {:.2f})$\times 10^{{{exp}}}$".format(one_band[0],one_band[1],exp=str(one_band[2]))

        buffer+= "{:.2f} & {} & {} & {} & {} \\\\ \n".format(float(masses[i]), obs_string, exp_string, one_sig_string, two_sig_string)

    # Add horizontal lines:
    buffer+= "\\hline \n"
    buffer+= "\\hline \n"

    return buffer




