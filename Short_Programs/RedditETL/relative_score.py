from pyspark import SparkConf, SparkContext
import sys, re, string, json
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

def add_pairs(p1, p2):
    p_new = (p1[0] + p2[0], p1[1] + p2[1])
    return p_new
    
#saves just the portion of the reddit data that will be used later as a tuple of
#the form (subreddit, author, score). Apply before .cache() to save memory.
def create_user_tuple(dict_in):
    return (dict_in["subreddit"], (dict_in["author"], dict_in["score"]))
    
def create_sub_keyval(user_tuple):
    return (user_tuple[0], (1, user_tuple[1][1]))
    
def average(sub_score):
    return (sub_score[0], sub_score[1][1]/sub_score[1][0])
   
#even broken up across multiple lines this code is awful and unreadable, which is a direct result
#of the horrible data structure created during the join. Using the broadcast join mostly fixes this 
def quality_calc(join_tuple):
    score = join_tuple[1][0][1]
    sub_average = join_tuple[1][1]
    quality = score/sub_average
    author = join_tuple[1][0][0]
    return (quality, author)
    

def main(inputs, output):
    text = sc.textFile(inputs)
    text = text.map(json.loads)
    user_tuples = text.map(create_user_tuple).cache()
    
    #average the scores for each subreddit and discard the subreddits where average < 0
    raw_scores = user_tuples.map(create_sub_keyval)
    sub_scores = raw_scores.reduceByKey(add_pairs)
    averages = sub_scores.map(average)
    averges = averages.filter(lambda x: x[1] > 0)
    
    #should create tuple of form (subreddit, ((author, score), subreddit_average)) which is terrible and shouldn't be allowed to exist
    user_avg_tuples = user_tuples.join(averages) 
    
    quality_data = user_avg_tuples.map(quality_calc)
    sorted_quality = quality_data.sortByKey(False)
    
    outdata = sorted_quality.map(json.dumps)
    outdata.saveAsTextFile(output)

if __name__ == '__main__':
    conf = SparkConf().setAppName('reddit relative score')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
