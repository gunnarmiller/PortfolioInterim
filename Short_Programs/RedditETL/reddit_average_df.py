import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, types
from pyspark.sql import functions as F
import re, string, json

# add more functions as necessary

def main(inputs, output):
    
    #schema is set to store only the fields we need: no reason to bother with the rest
    reddit_comment_schema = types.StructType([
        types.StructField('subreddit', types.StringType()),
        types.StructField('score', types.LongType()),
    ])
    
    comments = spark.read.json(inputs, schema = reddit_comment_schema)
    averages = comments.groupBy(comments["subreddit"]).agg(F.avg(comments["score"]))
    averages.write.csv(output, mode='overwrite')
    averages.explain()

if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('reddit average dataframe').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
