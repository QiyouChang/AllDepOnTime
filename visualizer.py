#!/usr/bin/python
import networkx as nx 
import matplotlib.pyplot as plt
class peopleNetwork(object):
    def __init__(self):
        self.adjMatrix = []
        self.size = 0
        self.G = nx.Graph()
    
    def readGraph(self,inputFile):
        f1 = open(inputFile, "r")
        lines = f1.readlines()
        count = 0

        for line in lines:
            values = line.split(",")
            currentLine = list(map(lambda x: float(x), values))
            self.adjMatrix.append(currentLine)
            count += 1
        self.size = count
    
    def drawGraph(self):
        for i in range(self.size):
            for j in range(self.size):
                w = self.adjMatrix[i][j]
                if w!= 0:
                    self.G.add_edge(i, j, weight=w)
        labels = nx.get_edge_attributes(self.G,'weight')
        #layout = nx.circular_layout(self.G)
        #layout = nx.random_layout(self.G)
        nx.draw_networkx(self.G, pos = layout, arrows=True)
        nx.draw_networkx_edge_labels(self.G, pos = layout, edge_labels=labels)
        plt.savefig("graph.png")

if __name__ == '__main__':
    network = peopleNetwork()
    network.readGraph("sample1.txt")
    network.drawGraph()
