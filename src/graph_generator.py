# -*- coding: utf-8 -*-
"""
Created on Thu Jun 22 07:56:13 2017

@author: gualandi
"""

import networkx as nx
import sys

n = int(sys.argv[1])
d = float(sys.argv[2])

G = nx.gnp_random_graph(n, d)


with open("rnd-"+str(n)+"-"+str(d)+".col", "w") as text_file:
    
    print("p edges",nx.number_of_nodes(G), nx.number_of_edges(G), file=text_file)

    for a,b in G.edges():
        print("e", a+1, b+1, file=text_file)
    
