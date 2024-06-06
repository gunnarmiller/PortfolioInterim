from pyspark import SparkConf, SparkContext
import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

# add more functions as necessary

def main(inputs, output):

    MAX_ITER = 20
    MAX_DIST = 999

    first = 3

    node_strings = sc.textFile(inputs)
    node_string_list = node_strings.collect()
    
    #node_keys = []
    node_edges = []
    node_distances = []
    node_paths = []
    
    
    for entry in node_string_list:
        split = entry.split(':')
        edge_strings = split[1].split(' ')
        edge_ints = []
        for edge in edge_strings:
            if (edge != ''):
                edge_ints.append(int(edge))
        edge_tuple = tuple(edge_ints)
        node_edges.append(edge_tuple)
        node_distances.append(MAX_DIST)
        node_paths.append(-1)
        
    node_distances[first - 1] = 0
        
    for i in range(0, MAX_ITER):
        done = True
        for j in range(0, len(node_distances)):
            if (node_distances[j] == i):
                done = False
                for k in node_edges[j]:
                    if (node_distances[k - 1] > node_distances[j] + 1):
                        node_distances[k - 1] = node_distances[j] + 1
                        node_paths[k - 1] = j
        if(done):
            break
            
            
    print(node_distances[:20])
    print(node_paths[:20])
        
        
     

if __name__ == '__main__':
    conf = SparkConf().setAppName('example code')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
