from pyspark import SparkConf, SparkContext
import sys, re, string, json
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

def create_keyval(comment):
    subreddit = comment["subreddit"]
    score = comment["score"]
    author = comment["author"]
    
    return (subreddit, (score, author))

def has_an_e(comment):
    if "e" in comment[0]:
        return True
    else:
        return False
        

def main(inputs, output):
    text = sc.textFile(inputs)
    text = text.map(json.loads)
    comments = text.map(create_keyval)
    comments = comments.filter(has_an_e)
    comments = comments.cache()
    positives = comments.filter(lambda x: x[1][0] > 0)
    positives.map(json.dumps).saveAsTextFile(output + '/positive')
    negatives = comments.filter(lambda x: x[1][0] <= 0)
    negatives.map(json.dumps).saveAsTextFile(output + '/negative')

if __name__ == '__main__':
    conf = SparkConf().setAppName('example code')
    sc = SparkContext(conf=conf)
    sc.setLogLevel('WARN')
    assert sc.version >= '3.0'  # make sure we have Spark 3.0+
    inputs = sys.argv[1]
    output = sys.argv[2]
    main(inputs, output)
