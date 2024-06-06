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
    
    #read in the data and processes it to get a list of temperature ranges
    weather = spark.read.csv(inputs, schema = observation_schema)
    weather = weather.filter(weather.qflag.isNull())
    weather = weather.drop('qflag').cache() #drop what we can before caching to save memory
    high_temps = weather.filter(weather.observation == 'TMAX').withColumnRenamed('value','max')
    high_temps = high_temps.drop('observation') #drop this now-reduntant column from both dfs to make join more efficient
    low_temps = weather.filter(weather.observation == 'TMIN').withColumnRenamed('value','min')
    low_temps = low_temps.drop('observation') 
    temp_pairs = high_temps.join(low_temps, ['station', 'date'])
    temp_ranges = temp_pairs.withColumn('range', (temp_pairs.max - temp_pairs.min)/10) #doing the /10 calc here instead of earlier is more efficient
    temp_ranges = temp_ranges.drop('max', 'min').cache()
    
    #find the largest range for each date
    biggest_ranges = temp_ranges.groupBy('date').max('range').hint('broadcast')
    biggest_station = temp_ranges.join(biggest_ranges, 'date')
    biggest_station = biggest_station.filter(biggest_station['range'] == biggest_station['max(range)']).sort('date')
    biggest_station = biggest_station.drop('max(range)').sort('date', 'station')
    biggest_station.write.csv(output, mode = 'overwrite')
    
    
    

if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('weather ETL').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
