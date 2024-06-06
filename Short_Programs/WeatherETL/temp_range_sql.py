import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

def main(inputs, output):

    observation_schema = types.StructType([
        types.StructField('station', types.StringType()),
        types.StructField('date', types.StringType()),
        types.StructField('observation', types.StringType()),
        types.StructField('value', types.IntegerType()),
        types.StructField('qflag', types.StringType()),
    ])
    
    
    spark.read.csv(inputs, schema = observation_schema).createOrReplaceTempView("Weather")
    spark.sql("SELECT * FROM Weather WHERE qflag IS NULL").createOrReplaceTempView("Weather")
    spark.sql("SELECT station, date, observation, value FROM Weather").createOrReplaceTempView("Weather")
    
    spark.sql("SELECT * FROM Weather WHERE observation = 'TMAX'").createOrReplaceTempView("Highs")
    spark.sql("SELECT station, date, value AS max FROM Highs").createOrReplaceTempView("Highs")
    spark.sql("SELECT * FROM Weather WHERE observation = 'TMIN'").createOrReplaceTempView("Lows")
    spark.sql("SELECT station, date, value AS min FROM Lows").createOrReplaceTempView("Lows")
    
    spark.sql("SELECT Highs.date, Highs.station, max, min FROM Highs JOIN Lows ON (Highs.station = Lows.station) AND (Highs.date = Lows.date)").createOrReplaceTempView("Ranges")
    spark.sql("SELECT date, station, (max - min)/10 AS range FROM Ranges").createOrReplaceTempView("Ranges")
    spark.sql("SELECT date, MAX(range) as diff FROM Ranges GROUP BY date ORDER BY date").createOrReplaceTempView("MaxRanges")
    spark.sql("SELECT MaxRanges.date, station, diff FROM MaxRanges JOIN Ranges ON (MaxRanges.date = Ranges.date) AND (MaxRanges.diff = Ranges.range)").createOrReplaceTempView("MaxRanges")
    max_ranges = spark.sql("SELECT * FROM MaxRanges ORDER BY date, station")
    max_ranges.write.csv(output, mode = 'overwrite')
    
    
    

if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('weather ETL').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
