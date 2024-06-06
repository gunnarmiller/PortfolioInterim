import sys, os, gzip, re, uuid
from cassandra.cluster import Cluster
from datetime import datetime
from cassandra.query import BatchStatement
assert sys.version_info >= (3, 5) # make sure we have Python 3.5+

#NASA logs datetime looks like DD/mon/YYYY:HH:MM:SS
def convert_datetime(dt_string):
    nasa_format = '%d/%b/%Y:%H:%M:%S'
    return datetime.strptime(dt_string, nasa_format)
    

def main(inputs, keyspace, table_name):

    line_re = re.compile(r'^(\S+) - - \[(\S+) [+-]\d+\] \"[A-Z]+ (\S+) HTTP/\d\.\d\" \d+ (\d+)$')

    cluster = Cluster(['node1.local','node2.local'])
    session = cluster.connect(keyspace)

    insert_format = session.prepare('INSERT INTO nasalogs (host, id, bytes, datetime, path) VALUES (?,?,?,?,?)')
    batch = BatchStatement()
    
    counter = 0
    batch_size = 200
    

    for f in os.listdir(inputs):
        with gzip.open(os.path.join(inputs, f), 'rt', encoding='utf-8') as logfile:
            for line in logfile:
                split_line = line_re.split(line)
                if(len(split_line) == 6):
                    host = split_line[1]
                    id_key = uuid.uuid1()
                    req_size = int(split_line[4])
                    time = convert_datetime(split_line[2])
                    path = split_line[3]
                    batch.add(insert_format, (host, id_key, req_size, time, path))
                    counter += 1

                if (counter >= batch_size):
                    session.execute(batch)
                    batch.clear()
                    counter = 0
                
    session.execute(batch) #last batch probably won't execute in the loop, so do it manually

if __name__ == '__main__':
    inputs = sys.argv[1]
    keyspace = sys.argv[2]
    table_name = sys.argv[3]
    main(inputs, keyspace, table_name)
