-- SQL for the ambient tracker table

BEGIN TRANSACTION;
DROP TABLE IF EXISTS AmbientValues;

CREATE TABLE AmbientValues(
       Id INTEGER PRIMARY KEY,
       Date TEXT,
       EXT_TEMP INTEGER,
       BARO_TEMP INTEGER,
       ONBOARD_TEMP INTEGER,
       PRESSURE INTEGER,
       BRIGHTNESS INTEGER,
       HUMINITY INTEGER);

INSERT INTO AmbientValues VALUES(
       1,
       'So 6. Jun 12:00:00 CEST 2021',
       20,
       21,
       22,
       123456,
       12,
       34);
COMMIT;
