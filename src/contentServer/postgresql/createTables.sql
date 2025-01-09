CREATE TABLE producer (
    producerId serial PRIMARY KEY,
    name VARCHAR (40) NOT NULL,
    title VARCHAR (50) NOT NULL,
    description VARCHAR (100) NOT NULL,
    flags INTEGER NOT NULL, 
    sourceId INTEGER NOT NULL,
    status SMALLINT NOT NULL
);

CREATE TABLE generation (
    generationId serial PRIMARY KEY,
    generationType INTEGER NOT NULL,
    producerId INTEGER NOT NULL,
    name VARCHAR (50) UNIQUE NOT NULL,
    description VARCHAR (100) NOT NULL,
    analysisTime TIMESTAMP NOT NULL,
    flags INTEGER NOT NULL,
    sourceId INTEGER NOT NULL,
    modificationTime TIMESTAMP,
    deletionTime TIMESTAMP,
    status SMALLINT NOT NULL,
    FOREIGN KEY (producerId) REFERENCES producer (producerId)
);

CREATE TABLE file (
    fileId serial PRIMARY KEY,
    producerId INTEGER NOT NULL,
    generationId INTEGER NOT NULL,
    protocol SMALLINT NOT NULL,
    serverType SMALLINT NOT NULL,
    server VARCHAR (50) NOT NULL,
    fileType SMALLINT NOT NULL,
    fileName VARCHAR (200) UNIQUE NOT NULL,
    flags INTEGER NOT NULL,
    sourceId INTEGER NOT NULL,
    modificationTime TIMESTAMP,
    deletionTime TIMESTAMP,
    fileSize BIGINT NOT NULL,
    status SMALLINT NOT NULL,
    FOREIGN KEY (producerId) REFERENCES producer (producerId),
    FOREIGN KEY (generationId) REFERENCES generation (generationId)
);

CREATE TABLE generationGeometry (
    producerId INTEGER NOT NULL,
    generationId INTEGER NOT NULL,
    geometryId INTEGER NOT NULL,
    levelId SMALLINT NOT NULL,
    flags INTEGER NOT NULL,
    sourceId INTEGER NOT NULL,
    modificationTime TIMESTAMP,
    deletionTime TIMESTAMP,
    status SMALLINT NOT NULL,
    PRIMARY KEY (generationId,geometryId,levelId),
    FOREIGN KEY (producerId) REFERENCES producer (producerId),
    FOREIGN KEY (generationId) REFERENCES generation (generationId)
);

    
CREATE TABLE content (
    fileId INTEGER NOT NULL,
    messageIndex INTEGER NOT NULL,
    fileType SMALLINT NOT NULL,
    filePosition BIGINT NOT NULL,
    messageSize INTEGER NOT NULL,
    producerId INTEGER NOT NULL,
    generationId INTEGER NOT NULL,
    geometryId INTEGER NOT NULL,
    parameterId INTEGER NOT NULL,
    parameterName VARCHAR (50) NOT NULL,
    forecastTime TIMESTAMP NOT NULL,
    levelId SMALLINT NOT NULL,
    level INTEGER NOT NULL,
    forecastType SMALLINT,
    foracastNumber INTEGER,
    aggregationId INTEGER,
    aggregationPeriod INTEGER,
    processingTypeId INTEGER,
    processingTypeValue1 NUMERIC,
    processingTypeValue2 NUMERIC,
    flags INTEGER NOT NULL,
    sourceId INTEGER NOT NULL,
    modificationTime TIMESTAMP,
    deletionTime TIMESTAMP,
    
    PRIMARY KEY (fileId, messageIndex),
    FOREIGN KEY (fileId) REFERENCES file (fileId),
    FOREIGN KEY (producerId) REFERENCES producer (producerId),
    FOREIGN KEY (generationId) REFERENCES generation (generationId)
);


CREATE TABLE event (
    eventId serial PRIMARY KEY,
    eventTime TIMESTAMP,
    eventType SMALLINT NOT NULL,
    id1 INTEGER,
    id2 INTEGER,
    id3 INTEGER,
    flags INTEGER,
    eventData VARCHAR (300)    
);

