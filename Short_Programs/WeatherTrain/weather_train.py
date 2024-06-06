import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types

from pyspark.ml import Pipeline
from pyspark.ml.feature import StringIndexer, VectorAssembler, SQLTransformer
from pyspark.ml.regression import RandomForestRegressor
from pyspark.ml.evaluation import RegressionEvaluator

def main(inputs, output):

    tmax_schema = types.StructType([
    types.StructField('station', types.StringType()),
    types.StructField('date', types.DateType()),
    types.StructField('latitude', types.FloatType()),
    types.StructField('longitude', types.FloatType()),
    types.StructField('elevation', types.FloatType()),
    types.StructField('tmax', types.FloatType()),
    ])
    
    data = spark.read.csv(inputs, schema=tmax_schema)
    train, validation = data.randomSplit([0.80, 0.20])
    train = train.cache()
    validation = validation.cache()
    
    
    date_assembler = SQLTransformer(statement="""SELECT today.latitude AS lat, today.longitude AS long, today.elevation AS elev, today.tmax AS tmax, 
                                                    dayOfYear(today.date) AS day, yesterday.tmax AS yesterday_tmax
                                                    FROM __THIS__ as today
                                                    INNER JOIN __THIS__ as yesterday
                                                        ON date_sub(today.date, 1) = yesterday.date
                                                        AND today.station = yesterday.station """   )
    
    
    weather_assembler = VectorAssembler(outputCol = 'features')
    weather_assembler.setInputCols(['lat','long','elev', 'day', 'yesterday_tmax'])
    
    regressor = RandomForestRegressor(featuresCol = 'features', labelCol = 'tmax', numTrees = 25, maxDepth = 10)
    weather_pipeline = Pipeline(stages=[date_assembler, weather_assembler, regressor])
    weather_model = weather_pipeline.fit(train)
    predictions = weather_model.transform(validation)
    
    weather_eval = RegressionEvaluator(predictionCol = 'prediction', labelCol = 'tmax')
    rmse = weather_eval.evaluate(predictions)
    r_squared = weather_eval.setMetricName('r2').evaluate(predictions)

    print('RMSE: ', rmse, '  |  r^2 value: ', r_squared)
    weather_model.write().overwrite().save(output)

if __name__ == '__main__':
    inputs = sys.argv[1]
    output = sys.argv[2]
    spark = SparkSession.builder.appName('example code').getOrCreate()
    assert spark.version >= '3.0' # make sure we have Spark 3.0+
    spark.sparkContext.setLogLevel('WARN')
    sc = spark.sparkContext
    main(inputs, output)
