-- create the database :dbname
DROP DATABASE IF EXISTS :dbname ;
CREATE DATABASE :dbname ;

-- create the user :admin
\c :dbname postgres

REASSIGN OWNED BY :admin TO postgres ;

REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :admin ;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :admin ;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public FROM :admin ;

DROP OWNED BY :admin;
DROP USER IF EXISTS :admin ;

CREATE USER :admin WITH ENCRYPTED PASSWORD 'admin' ;

-- create the user :puseradmin
\c :dbname postgres

REASSIGN OWNED BY :puser TO postgres ;

REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :puser ;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :puser ;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public FROM :puser ;

DROP OWNED BY :puser;
DROP USER IF EXISTS :puser ;

CREATE USER :puser WITH ENCRYPTED PASSWORD 'puser' ;
