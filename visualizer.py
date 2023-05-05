#!/usr/bin/python
import matplotlib.pyplot as plt
import networkx as nx
from pyvis.network import Network
import matplotlib.cm as cm
import sys

def rgba_to_hex(rgba):
    r, g, b, a = rgba
    return '#{:02X}{:02X}{:02X}'.format(int(r * 255), int(g * 255), int(b * 255))

if __name__ == '__main__':
    
    # network = peopleNetwork()
    # network.readGraph("sample1.txt", "seqResultv3.txt")
    # network.drawGraph()

    f1 = open("sample1.txt", "r")
    f2 = open("seqResultv1.txt", "r")
    edge_lines = f1.readlines()
    eval_lines = f2.readlines()
    adjMatrix = []
    eval_array = []
    eval_edge_array = []
    count = 0

    for line in edge_lines: 
        values = line.split(",")
        currentLine = list(map(lambda x: float(x), values))
        adjMatrix.append(currentLine)
        count+=1

    for eval_line in eval_lines:
        eval_array.append(float(eval_line))
    print(count)
    
    Nodes = [str(node) for node in (range(0, count))]
    G = nx.Graph()
    G.add_nodes_from(Nodes)
    Edges = []
    for i in range(count):
        for j in range(count):
            if adjMatrix[i][j] != 0:
                eval_edge_array.append(adjMatrix[i][j])
                Edges.append((str(i),str(j)))
    # Edges_new = [str(e) for e in Edges]
    G.add_edges_from(Edges)
    evalmax = max(eval_array)
    evalmin = min(eval_array)
    eg_max = max(eval_edge_array)
    eg_min = min(eval_edge_array)
    # define the colormap to use
    colormap = cm.get_cmap('copper')
    colormap_edges = cm.get_cmap('winter')
    colors = [colormap((value - evalmin) / (evalmax - evalmin)) for value in eval_array]
    colors_edges = [colormap_edges((value - eg_min)/(eg_max - eg_min)) for value in eval_edge_array]
    # create a network visualization using pyvis
    net = Network(height='1000px', width='1500px', notebook=True)
    # add nodes to the visualization with their respective colors
    for node, color in zip(G.nodes, colors):
        color1 = rgba_to_hex(color)
        net.add_node(node, color=color1)
    for edge,color in zip(G.edges,colors_edges) :
        color1 = rgba_to_hex(color)
        net.add_edge(edge[0], edge[1], color = color1)
    # display the network visualization
    net.show('example.html')
    

