import sys, datetime
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

from pyspark.ml import Pipeline
from pyspark.ml import PipelineModel

def main(model_file):

    tmax_schema = types.StructType([
    types.StructField('station', types.StringType()),
    types.StructField('date', types.DateType()),
    types.StructField('latitude', types.FloatType()),
    types.StructField('longitude', types.FloatType()),
    types.StructField('elevation', types.FloatType()),
    types.StructField('tmax', types.FloatType()),
    ])
    
    tomorrow = datetime.datetime(2023, 11, 18)
    today = datetime.datetime(2023, 11, 17)
    today_weather = spark.createDataFrame([('ComputerLab', tomorrow , 49.2771, -122.9146, 330.0, -999.9), ('ComputerLab', today , 49.2771, -122.9146, 330.0, 12.0)], schema = tmax_schema)
    
    model = PipelineModel.load(model_file)
    prediction = model.transform(today_weather)
    
    print('Predicted tmax tomorrow: ', prediction.head(1)[-1]['prediction'])
    

if __name__ == '__main__':
    model_file = sys.argv[1]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(model_file)
