import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

# add more functions as necessary

def main(topic):
    
    

    #test_rdd = sc.parallelize(["mary had", "a little", "lamb it's"], numSlices=1)
    df = spark.createDataFrame([('foo 1', 'holy 2'), ('this 3', 'what 4'), ('fuck 5', 'bull 6')], ["int", "float"])
    split_df = functions.split(df['int'], ' ')
    df = df.withColumns({'name' : split_df.getItem(0), 'less' : split_df.getItem(1).cast("double")/4})
    
    df.show()
    
    
    

if __name__ == '__main__':
    topic = sys.argv, [1]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(topic)
