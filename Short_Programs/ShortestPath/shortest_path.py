from pyspark import SparkConf, SparkContext
import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+



def create_keyval(input_string):
    keyvals_string = input_string.split(':')
    key = int(keyvals_string[0])
    vals_string = keyvals_string[1].split()
    value = [int(x) for x in vals_string]
    return (key, value)
 
#take is a source node and returns (key, (path, distance)) for all the nodes it points to.
#The path/distance returned might not be the shortest one for that node: once
#the resulting RDD is unioned to paths, prune() used to remove the longer paths
def get_new_paths(source):
    destination_list = []
    
    for key in source[1][1]:
        destination = (key, (source[0], source[1][0][1] + 1))
        destination_list.append(destination)
        
    return destination_list
 
#takes to path entries of the form (key, (path, distance)) and returns
#whichever one has the longer distance.   
def prune(path1, path2):
    if (path1[1] < path2[1]):
        return path1
    else:
        return path2


def main(inputs, output, source_str, dest_str):

    MAX_ITER = 5
    NO_PATH = -1
    source = int(source_str)
    end = int(end_str)
    
    inputs = inputs + "/links-simple-sorted.txt"

    node_strings = sc.textFile(inputs)
    nodes = node_strings.map(create_keyval)
    nodes.cache() #elements of nodes have the form (key, [edge list])
    
    #elements of paths have the form (key, (path, distance))
    #need to create the first entry of paths 'by hand' by finding our starting node setting it's values
    paths = nodes.filter(lambda x: x[0] == source).map(lambda x: (x[0], (NO_PATH,0))) 
    
    for i in range(0,MAX_ITER):
        destinations = paths.filter(lambda x: x[1][1] == i).join(nodes) #elements of targets will have the form (key, ((path, dist), (dest list))
        if(destinations.count() == 0):  #no need to continue looping if we've found all reachable nodes
            break
        new_paths = destinations.flatMap(get_new_paths)
        new_paths.sortByKey(numPartitions = 1).saveAsTextFile(output + '/iter-' + str(i))
        paths = paths.union(new_paths).reduceByKey(prune).cache()
    
    
    path = []
        
    #need to make sure we actually found our destination before retracing the path  
    if(len(paths.lookup(end)) == 0):
        path.append("Path not found")
    else:       
        prev_node = end
        
        while(prev_node != source):
            path.append(prev_node)
            prev_list = paths.lookup(prev_node)
            prev_node = prev_list[0][0]
            
        path.append(source)
        path.reverse() 
     
    sc.parallelize(path, numSlices = 1).saveAsTextFile(output + '/path')
            
    
    
    
if __name__ == '__main__':
    conf = SparkConf().setAppName('shortest path')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    source_str = sys.argv[3]
    end_str = sys.argv[4]
    main(inputs, output, source_str, end_str)
