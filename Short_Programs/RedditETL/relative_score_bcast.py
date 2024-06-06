from pyspark import SparkConf, SparkContext
import sys, re, string, json
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

def add_pairs(p1, p2):
    p_new = (p1[0] + p2[0], p1[1] + p2[1])
    return p_new
    
#saves just the portion of the reddit data that will be used later as a tuple of
#the form (subreddit, (author, score)). Apply before .cache() to save memory.
def create_user_tuple(dict_in):
    return (dict_in["subreddit"], (dict_in["author"], dict_in["score"]))
    
def create_sub_keyval(user_tuple):
    return (user_tuple[0], (1, user_tuple[1][1]))
    
def average(sub_score):
    return (sub_score[0], sub_score[1][1]/sub_score[1][0])
    

    

def main(inputs, output):
    text = sc.textFile(inputs)
    text = text.map(json.loads)
    user_tuples = text.map(create_user_tuple).cache()
    
    #average the scores for each subreddit and discard the subreddits where average < 0
    raw_scores = user_tuples.map(create_sub_keyval)
    sub_scores = raw_scores.reduceByKey(add_pairs)
    averages = sub_scores.map(average)
    averges = averages.filter(lambda x: x[1] > 0)
    
    #As directed, here is the comment accompanying the .collect() that bounds the size of the data. 
    #The number of elements of the RDD will be equal to the number of distinct subreddits among the comments
    #in the input data. For smaller data sets that might be a few thousand at most. However, Google says there
    #are 2.8 million subreddits on reddit currently, so the HARD upper bound for the size of the RDD is ~3 million elements.
    #Under reasonable assumptions, even in the extreme cast this should keep the total size in memory at well under 1 GB.
    avg_dict = dict(averages.collect())
    bcast_dict = sc.broadcast(avg_dict) #should broadcast a dict of the form {subreddit : avg_score}
    
    
    quality_data = user_tuples.map(lambda x:(x[1][1]/bcast_dict.value[x[0]], x[1][0]))
    sorted_quality = quality_data.sortByKey(False)
    
    outdata = sorted_quality.map(json.dumps)
    outdata.saveAsTextFile(output)

if __name__ == '__main__':
    conf = SparkConf().setAppName('reddit relative score broadcast')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
