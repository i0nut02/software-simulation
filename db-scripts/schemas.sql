\c :dbname


CREATE DOMAIN StringS AS VARCHAR(50);

CREATE DOMAIN IntGEZ AS integer CHECK (VALUE >= 0);
CREATE DOMAIN RealGEZ AS DOUBLE PRECISION CHECK (VALUE >= 0);

CREATE TABLE IF NOT EXISTS Logs (
        clientId IntGEZ NOT NULL,
        timeRequest RealGEZ NOT NULL,
        timeResponse RealGEZ NOT NULL,

        PRiMARY KEY (clientId, timeRequest)
);

CREATE TABLE IF NOT EXISTS ResponseTimeMonitors (
        fromTime RealGEZ NOT NULL,
        toTime RealGEZ NOT NULL,
        numRequests IntGEZ NOT NULL,
        minTime RealGEZ NOT NULL,
        maxTime RealGEZ NOT NULL,
        meanTime RealGEZ NOT NULL
);
