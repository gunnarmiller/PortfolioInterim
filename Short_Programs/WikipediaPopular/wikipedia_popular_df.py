import sys, re
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, types
from pyspark.sql import functions as F

@F.udf(returnType = types.StringType())
def filename_to_time(filename):
    parsed_path = filename.split("/")
    parsed_name = parsed_path[-1].split(".", maxsplit = 1)[0]
    date_and_hour = parsed_name.split("-", maxsplit=1)[1]
    return date_and_hour[0:11]

def main(inputs, output):
    
    wiki_page_schema = types.StructType([
        types.StructField('language', types.StringType()),
        types.StructField('title', types.StringType()),
        types.StructField('views', types.LongType())
    ])

    #loading, filtering and processing the data so we can work with it
    pages = spark.read.csv(inputs, schema = wiki_page_schema, sep=" ").withColumn('time', F.input_file_name())
    pages = pages.filter(pages.language == "en").filter(pages.title != "Main_Page").filter(pages.title.startswith("Special:") == False)
    pages = pages.drop(pages.language)
    pages = pages.withColumn("time", filename_to_time(pages.time)).cache()
    
    #finding the most-viewed page for each hour
    max_pageviews = pages.groupBy("time").max("views").hint("broadcast")
    popular_pages = pages.join(max_pageviews, "time")
    popular_pages = popular_pages.filter(popular_pages["views"] == popular_pages["max(views)"]).drop(popular_pages["max(views)"])
    popular_pages = popular_pages.sort(popular_pages.time, popular_pages.title)
    popular_pages.write.json(output, mode = 'overwrite')
    popular_pages.explain()
    
    
    
if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
