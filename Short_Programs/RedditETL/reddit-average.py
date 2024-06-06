from pyspark import SparkConf, SparkContext
import sys, re, string, json
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

def add_pairs(p1, p2):
    p_new = (p1[0] + p2[0], p1[1] + p2[1])
    return p_new
    
def create_keyval(dict_in):
    keyval = (dict_in["subreddit"], (1, dict_in["score"]))
    return keyval
    
def average(sub_score):
    return (sub_score[0], sub_score[1][1]/sub_score[1][0])

def main(inputs, output):
    text = sc.textFile(inputs)
    text = text.map(json.loads)
    comments = text.map(create_keyval)
    sub_scores = comments.reduceByKey(add_pairs)
    averages = sub_scores.map(average)
    averages = averages.coalesce(1)
    outdata = averages.map(json.dumps)
    outdata.saveAsTextFile(output)

if __name__ == '__main__':
    conf = SparkConf().setAppName('reddit average')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
