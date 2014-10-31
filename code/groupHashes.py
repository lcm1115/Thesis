# File: groupHashes.py
# Author: Liam Morris
# Description: Given an input stream with the following format:
#              hash1
#              hash2
#              hash3
#
#              hash4
#              hash5
#
#              hash3
#              hash6
#              ...
#
#              determines which hashes are grouped together based on appearing
#              together in chunks at any point in the file. Each hash represents
#              a node in a graph, with two nodes being adjacent if they appear
#              adjacent to each other in the file. Performs a DFS on each
#              component in the graph after creating the graph to determine
#              which entities are present based on the input hashes.

import statistics
import sys

# Vertex class that stores its label, neighbors, and visited status.
class Vertex:
    def __init__(self, label):
        self.visited = False
        self.label = label
        self.neighbors = set()

    def addNeighbor(self, neighbor):
        self.neighbors.add(neighbor)
        neighbor.neighbors.add(self)

    def __str__(self):
        return self.label

# Performs DFS on a given vertex and returns a list of visited vertices.
def DFS(vertex):
    s = [vertex]
    vertices = []
    while len(s) > 0:
        v = s.pop()
        v.visited = True
        vertices.append(v)
        for n in v.neighbors:
            if not n.visited:
                s.append(n)
    return vertices

def groupHashes(filename):
    vertices = { }
    groups = { }
    f = open(filename)
    line = None
    while line != "":
        line = f.readline()
        hashes = []
        # Read in current group of hashes
        while line != "\n" and line != "":
            hashes.append(line.strip())
            line = f.readline()
        # Make vertices adjacent if hashes are adjacent
        for i in range(len(hashes) - 1):
            # If vertex doesn't yet exist, create it
            if hashes[i] not in groups.keys():
                groups[hashes[i]] = Vertex(hashes[i])
            if hashes[i + 1] not in groups.keys():
                groups[hashes[i + 1]] = Vertex(hashes[i + 1])
            # Create edge
            groups[hashes[i]].addNeighbor(groups[hashes[i + 1]])
    f.close()

    return groups

if __name__ == '__main__':
    groups = groupHashes(sys.argv[1])
    print("Number of addresses:", len(groups))
    maxsize = 0
    maxkey = list(groups.keys())[0]
    entities = []
    sizes = []
    for v in groups.values():
        if not v.visited:
            curr = [str(t) for t in DFS(v)]
            if len(curr) > 1:
                sizes.append(len(curr))
                entities.append('\n'.join(curr))
                if len(curr) > maxsize:
                    maxsize = len(curr)
                    maxkey = v.label
    # Sort entities by size
    pairs = list(zip(entities, sizes))
    pairs.sort(key=lambda p: p[1], reverse=True)
    entities = [pair[0] for pair in pairs]
    print("Number of entities:", len(entities))
    print("Average group size:", statistics.mean(sizes))
    print("Median group size:", statistics.median(sizes))
    print("Mode group size:", statistics.mode(sizes))
    print("Largest group:", maxsize)
    print("Key for group:", maxkey)
    print('\n\n'.join(entities))
