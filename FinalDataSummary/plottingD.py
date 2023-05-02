import argparse
import numpy as np
import random
import matplotlib.pyplot as plt


def speedupcal(speedup, para, seq):
    for index in range(len(para)):
        speedup[index, :] = seq[index]/para[index, :]
    return speedup

if __name__ == "__main__":

    # Dense 
    # Sequential Value (v1)
    seqD = open("seqDense.csv", "r")
    lines = seqD.readlines()
    dense_seq = np.zeros(len(lines))
    nodes_dense = np.zeros(len(lines), dtype = int)
    count = 0 
    for line in lines: 
        values = line.split(',')
        nodes_dense[count] = values[0]
        dense_seq[count] = values[1]
        count += 1 

    # People Partition (v2)
    v1D = open("v2Dense.csv", "r")
    lines = v1D.readlines()
    dense_v1 = np.zeros((len(lines)-1,len(lines[1].split(','))-1), dtype = float)
    dense_v1_speedup = np.zeros((len(lines)-1,len(lines[1].split(','))-1), dtype = float)
    core_v1_D = np.zeros(len(lines[0]), dtype = int)
    count = 0 
    for line in lines: 
        values = line.split(',')
        if count != 0:
            dense_v1[count-1] = values[1:]
        else: 
            core_v1_D = values
        count += 1
    dense_v1_speedup = speedupcal(dense_v1_speedup, dense_v1, dense_seq)
    

    # 1D Partition (v3)
    v2D = open("v3Dense.csv", "r")
    lines = v2D.readlines()
    dense_v2 = np.zeros((len(lines)-1,len(lines[1].split(','))-1), dtype = float)
    dense_v2_speedup = np.zeros((len(lines)-1,len(lines[1].split(','))-1), dtype = float)
    core_v2_D = np.zeros(len(lines[0]), dtype = int)
    count = 0 
    for line in lines: 
        values = line.split(',')
        if count != 0:
            dense_v2[count-1] = values[1:]
        else: 
            core_v2_D = values
        count += 1
    dense_v2_speedup = speedupcal(dense_v2_speedup, dense_v2, dense_seq)

    # OpenMP (v4)
    v4D = open("v4Dense.csv", "r")
    lines = v4D.readlines()
    dense_v4 = np.zeros(len(lines), dtype = float)
    speedup_v4D = np.zeros(len(lines), dtype = float)
    core_v4_D = np.zeros(len(lines), dtype = int)
    count = 0
    for line in lines: 
        values = line.split(',')
        core_v4_D[count] = values[0]
        dense_v4[count] = values[1]
        speedup_v4D[count] = values[2]
        count += 1 
    
    fig1, axs1 = plt.subplots(3, 2)
    #Small Graph Runtime
    axs1[0, 0].plot(core_v1_D,dense_v1[1], label = "v1-50")
    axs1[0, 0].plot(core_v1_D,dense_v1[3], label = "v1-200")
    axs1[0, 0].plot(core_v1_D,dense_v1[4], label = "v1-500")
    axs1[0, 0].plot(core_v1_D,dense_v2[1], label = "v2-50")
    axs1[0, 0].plot(core_v1_D,dense_v2[3], label = "v2-200")
    axs1[0, 0].plot(core_v1_D,dense_v2[4], label = "v2-500")
    axs1[0, 0].set_title('Runtime for small graph')
    axs1[0, 0].set_xlabel('Numbe of Cores')
    axs1[0, 0].set_ylabel('Runtime(s)')
    #Small Graph Speedup 
    axs1[0, 1].plot(core_v1_D,dense_v1_speedup[1], label = "v1-50")
    axs1[0, 1].plot(core_v1_D,dense_v1_speedup[3], label = "v1-200")
    axs1[0, 1].plot(core_v1_D,dense_v1_speedup[4], label = "v1-500")
    axs1[0, 1].plot(core_v1_D,dense_v2_speedup[1], label = "v2-50")
    axs1[0, 1].plot(core_v1_D,dense_v2_speedup[3], label = "v2-200")
    axs1[0, 1].plot(core_v1_D,dense_v2_speedup[4], label = "v2-500")
    axs1[0, 1].set_title('Speedup for small graph')
    axs1[0, 1].set_xlabel('Numbe of Cores')
    axs1[0, 1].set_ylabel('Speedup(x)')

    #Small Graph Runtime
    axs1[1, 0].plot(core_v1_D,dense_v1[6], label = "v1-2000")
    axs1[1, 0].plot(core_v1_D,dense_v1[7], label = "v1-3000")
    axs1[1, 0].plot(core_v1_D,dense_v1[8], label = "v1-5000")
    axs1[1, 0].plot(core_v1_D,dense_v2[6], label = "v2-2000")
    axs1[1, 0].plot(core_v1_D,dense_v2[7], label = "v2-3000")
    axs1[1, 0].set_title('Runtime for large graph')
    axs1[1, 0].set_xlabel('Numbe of Cores')
    axs1[1, 0].set_ylabel('Runtime(s)')
    #Small Graph Speedup 
    axs1[1, 1].plot(core_v1_D,dense_v1_speedup[6], label = "v1-2000")
    axs1[1, 1].plot(core_v1_D,dense_v1_speedup[7], label = "v1-3000")
    axs1[1, 1].plot(core_v1_D,dense_v1_speedup[8], label = "v1-5000")
    axs1[1, 1].plot(core_v1_D,dense_v2_speedup[6], label = "v2-2000")
    axs1[1, 1].plot(core_v1_D,dense_v2_speedup[7], label = "v2-3000")
    axs1[1, 1].set_title('Speedup for large graph')
    axs1[1, 1].set_xlabel('Numbe of Cores')
    axs1[1, 1].set_ylabel('Speedup(x)')

    #OpenMP
    #runtime
    axs1[2, 0].plot(nodes_dense,dense_v4, label = "OpenMP-optimal")
    axs1[2, 0].plot(core_v4_D,dense_seq, label = "Sequential")
    axs1[2, 0].set_title('Runtime for OpenMP')
    axs1[2, 0].set_xlabel('Total Vertices')
    axs1[2, 0].set_ylabel('Runtime(s)')
    #speedup
    axs1[2, 1].plot(core_v4_D,speedup_v4D, label = "OpenMP-optimal")
    axs1[2, 1].set_title('Speedup for OpenMP')
    axs1[2, 1].set_xlabel('Total Vertices')
    axs1[2, 1].set_ylabel('Speedup(x)')



    axs1[0, 0].legend()
    axs1[0, 1].legend()
    axs1[1, 0].legend()
    axs1[1, 1].legend()
    axs1[2, 0].legend()
    axs1[2, 1].legend()

    fig1.suptitle('Figure 1: Dense Graph Runtime and Speedup Summary')

    # Show the plots
    plt.show()
    
    