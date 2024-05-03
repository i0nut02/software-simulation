\c :dbname


CREATE DOMAIN StringS AS VARCHAR(50);

CREATE DOMAIN IntGEZ AS integer CHECK (VALUE >= 0);
CREATE DOMAIN RealGZ AS DOUBLE PRECISION CHECK (VALUE > 0);

CREATE TABLE IF NOT EXISTS Logs (
        clientId IntGEZ NOT NULL,
        timeRequest RealGZ NOT NULL,
        timeResponse RealGZ,

        PRiMARY KEY (clientId, timeRequest)
);