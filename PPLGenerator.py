import math 
import numpy as np
import sys
import random

def Generation(total, graph_type):
    popMat = np.zeros((total,total), dtype = float)
    # for dense graph (m = O(n^2))
    if graph_type == "D":
        total_connections = random.randint(total*(total-1), total*total+1)
        for round in range(total_connections):
            u, v = random.randint(0, total-1), random.randint(0,total-1)
            uv_weight = random.gauss(mu=0.0, sigma=0.3)
            vu_weight = random.gauss(mu=0.0, sigma=0.3)
            popMat[u,v] = uv_weight
            popMat[v,u] = vu_weight
    elif graph_type == "S":
        # for sparse graph (m = O(n))
        total_connections = random.randint(total/2, total+1)
        for round in range(total_connections):
            u = 0
            v = 0
            while u == v:
                u, v = random.randint(0, total-1), random.randint(0,total-1)
            uv_weight = random.gauss(mu=0.0, sigma=0.3)
            vu_weight = random.gauss(mu=0.0, sigma=0.3)
            popMat[u,v] = uv_weight
            popMat[v,u] = vu_weight
    return popMat

if __name__ == '__main__':
    data_output = sys.argv[1]
    total_population = sys.argv[2]
    graph_type = sys.argv[3]
    print(graph_type)
    Result_G = Generation(int(total_population),graph_type)
    np.savetxt(data_output, Result_G, delimiter=",")
    