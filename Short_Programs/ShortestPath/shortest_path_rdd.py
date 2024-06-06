from pyspark import SparkConf, SparkContext
import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

MAX_DIST = 10
NO_PATH = -1

def create_keyval(input_string):
    keyvals_string = input_string.split(':')
    key = int(keyvals_string[0])
    vals_string = keyvals_string[1].split()
    adj_list = [int(x) for x in vals_string]
    value = (adj_list, MAX_DIST, NO_PATH)
    return (key, value)
 
#Takes in the key-val pair for a node a returns a list of key-val pairs of all the nodes that it points to.
#The key, distance and path should all be correct for destination nodes but they will NOT have any values
#To use for their own destinations: this must be fixed by combining them with the entry in the original RDD    
def get_destinations(source):
    destination_list = []
    
    for key in source[1][0]:
        destination = (key, ((), source[1][1] + 1, source[0]))
        destination_list.append(destination)
        
    return destination_list
 
def update_node(node1, node2):
    
    if(len(node1[0]) > len(node2[0])):
        adj_list = node1[0]
    else:
        adj_list = node2[0]
        
    if(node1[1] < node2[1]):
        dist = node1[1]
        path = node1[2]
    else:
        dist = node2[1]
        path = node2[2]
  
    return (adj_list, dist, path)

def main(inputs, output):

    START_VAL = 3

    node_strings = sc.textFile(inputs)
    nodes = node_strings.map(create_keyval)
    
    #we have to set the distance to our first node 'by hand' to kick off the algorithm
    first_node = nodes.filter(lambda x: x[0] == START_VAL)
    first_node = first_node.map(lambda x: (x[0],(x[1][0],0,NO_PATH)))
    nodes = nodes.union(first_node) #technically this will create a redundant copy of our first node entry briefly. The first time we reduceByKey while iterating, it will get removed
    
    for i in range(0,MAX_DIST):
        sources = nodes.filter(lambda x: x[1][1] == i)
        print("Iteration ", i, ": Number of Source Nodes: ", sources.count())
        if (sources.isEmpty()):
            break
            
        destinations = sources.flatMap(get_destinations)
        nodes = nodes.union(destinations).reduceByKey(update_node)
    
    sample = nodes.take(20)
    print(sample) 
    
if __name__ == '__main__':
    conf = SparkConf().setAppName('shortest path')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
