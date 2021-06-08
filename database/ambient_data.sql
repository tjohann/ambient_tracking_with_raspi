-- SQL for the ambient tracker table

BEGIN TRANSACTION;
DROP TABLE IF EXISTS AmbientValues;

CREATE TABLE AmbientValues(
       DATE INTEGER PRIMARY KEY,
       EXT_TEMP INTEGER,
       BARO_TEMP INTEGER,
       ONBOARD_TEMP INTEGER,
       PRESSURE INTEGER,
       BRIGHTNESS INTEGER,
       HUMINITY INTEGER);

INSERT INTO AmbientValues VALUES(
       1623141708,
       20,
       21,
       22,
       123456,
       12,
       34);
COMMIT;
