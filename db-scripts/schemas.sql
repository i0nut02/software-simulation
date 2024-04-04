\c :dbname

CREATE TYPE RequestType AS ENUM ('listening for connections', 'request of connection', 
                                 'sending ID', 'listen for process request', 'my sleep request', 
                                 'alert Blocking call', 'exited Blocking call', 'syn sleep request',
                                 'disconnection request', 'sync process', 'waiting for process ID',
                                 'waiting for sync');

CREATE DOMAIN IntGEZ AS integer CHECK (VALUE >= 0);
CREATE DOMAIN RealGZ AS DOUBLE PRECISION CHECK (VALUE > 0);

CREATE TABLE IF NOT EXISTS RedisLog (
        request_instant timestamp NOT NULL,
        request_type RequestType NOT NULL,
        from_user IntGEZ,
        to_user IntGEZ,
        request_value RealGZ,

        CONSTRAINT from_constraint CHECK ((from_user = 0 AND (request_type = 'listening for connections' OR 
                                                                request_type = 'sending ID' OR 
                                                                request_type = 'listen for process request' OR 
                                                                request_type = 'sync process'))
                                        OR (from_user <> 0 AND (request_type <> 'listening for connections' AND 
                                                                request_type <> 'sending ID' AND 
                                                                request_type <> 'listen for process request' AND 
                                                                request_type <> 'sync process'))
                                        ),

        CONSTRAINT to_constraint CHECK ((to_user IS NULL AND (request_type = 'sending ID' OR
                                                              request_type = 'listening for connections'))
                                        OR (NOT (to_user IS NULL) AND to_user <> 0 AND (request_type = 'listen for process request' OR 
                                                                                        request_type = 'sync process'))
                                        OR (NOT (to_user IS NULL) AND to_user = 0 AND (
                                                request_type <> 'listen for process request' AND
                                                request_type <> 'sending ID' AND
                                                request_type <> 'listening for connections' AND
                                                request_type <> 'sync process'   ))
                                        ),
        
        CONSTRAINT req_value_constraint CHECK ((request_value IS NULL AND (request_type <> 'syn sleep request' AND
                                                                           request_type <> 'my sleep request' AND
                                                                           request_type <> 'sending ID'))
                                               OR (request_value IS NOT NULL AND (request_type = 'syn sleep request' OR
                                                                                  request_type = 'my sleep request' OR
                                                                                  request_type = 'sending ID')))
);