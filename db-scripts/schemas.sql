\c :dbname

CREATE TYPE RequestType AS ENUM ('listening for connections', 'request of connection', 
                                 'sending ID', 'listen for process request', 'my sleep request', 
                                 'alert Blocking call', 'exited Blocking call', 'syn sleep request',
                                 'disconnection request', 'sync process', 'waiting for process ID',
                                 'waiting for sync');

CREATE DOMAIN StringS AS VARCHAR(50);

CREATE DOMAIN IntGEZ AS integer CHECK (VALUE >= 0);
CREATE DOMAIN RealGZ AS DOUBLE PRECISION CHECK (VALUE > 0);

CREATE TABLE IF NOT EXISTS RedisLog (
        request_instant timestamp NOT NULL,
        stream StringS NOT NULL,
        request_value RealGZ
);