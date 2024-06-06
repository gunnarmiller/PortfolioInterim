import sys
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

from pyspark.sql import SparkSession, functions, types
spark = SparkSession.builder.appName('colour prediction').getOrCreate()
spark.sparkContext.setLogLevel('WARN')
assert spark.version >= '2.4' # make sure we have Spark 2.4+

from pyspark.ml import Pipeline
from pyspark.ml.feature import StringIndexer, VectorAssembler, SQLTransformer
from pyspark.ml.classification import MultilayerPerceptronClassifier
from pyspark.ml.evaluation import MulticlassClassificationEvaluator

from colour_tools import colour_schema, rgb2lab_query, plot_predictions


def main(inputs):
    data = spark.read.csv(inputs, schema=colour_schema)
    train, validation = data.randomSplit([0.75, 0.25])
    train = train.cache()
    validation = validation.cache()
    
    
    rgb_assembler = VectorAssembler(outputCol = 'features')
    rgb_assembler.setInputCols(['R','G','B'])
    word_indexer = StringIndexer(outputCol = 'label', inputCol = 'word')
    classifier = MultilayerPerceptronClassifier(layers=[3, 30, 11])
    
    rgb_pipeline = Pipeline(stages=[rgb_assembler, word_indexer, classifier])
    rgb_model = rgb_pipeline.fit(train)
    predictions = rgb_model.transform(validation)
    
     
    color_eval = MulticlassClassificationEvaluator(predictionCol = 'prediction', labelCol = 'label')
    score = color_eval.evaluate(predictions)
    
    plot_predictions(rgb_model, 'RGB', labelCol='word')
    print('Validation score for RGB model: %g' % (score, ))
    
    rgb_to_lab_query = rgb2lab_query(passthrough_columns=['word'])
    
   
    sql_assembler = SQLTransformer(statement = rgb_to_lab_query)
    lab_assembler = VectorAssembler(outputCol = 'features')
    lab_assembler.setInputCols(['labL','labA','labB'])
    word_indexer2 = StringIndexer(outputCol = 'label', inputCol = 'word')
    classifier2 = MultilayerPerceptronClassifier(layers=[3, 30, 11])
    
    lab_pipeline = Pipeline(stages=[sql_assembler, lab_assembler, word_indexer2, classifier2])
    lab_model = lab_pipeline.fit(train)
    lab_predictions = lab_model.transform(validation)
    score2 = color_eval.evaluate(lab_predictions)
    
    plot_predictions(lab_model, 'LAB', labelCol='word')
    print('Validation score for LAB model: %g' % (score2, ))
    
if __name__ == '__main__':
    inputs = sys.argv[1]
    main(inputs)
