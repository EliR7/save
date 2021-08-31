-- connect to database postgres
CREATE DATABASE save;
-- now connect to database save
CREATE SCHEMA save;
CREATE USER save PASSWORD 'secret';

REVOKE ALL ON SCHEMA public FROM save;
GRANT USAGE ON SCHEMA save TO save;

CREATE TABLE save.account_info (
    last_name VARCHAR(10) NOT NULL,
    first_name VARCHAR(15) NOT NULL,
    street_address VARCHAR(30) NOT NULL,
    unit VARCHAR(5) NULL,
    city VARCHAR(20) NOT NULL,
    state CHAR(2) NOT NULL,
    zip CHAR(5) NOT NULL,
    dob DATE NOT NULL,
    ssn CHAR(11) NOT NULL,
    email_address VARCHAR(40) NOT NULL,
    mobile_number CHAR(10) NOT NULL,
    account_number INT NOT NULL,
    PRIMARY KEY(account_number)
);

COPY save.account_info(last_name, 
                       first_name, 
                       street_address, 
                       unit, 
                       city, 
                       state, 
                       zip, 
                       dob, 
                       ssn, 
                       email_address, 
                       mobile_number, 
                       account_number) 
     FROM '/Users/iliar00t/python_tests/Save/account_info_fixed.csv' 
     DELIMITER ',' 
     CSV HEADER;

CREATE TABLE save.transactions (
    account_number INT NOT NULL,
    transaction_datetime TIMESTAMP NOT NULL,
    transaction_amount DOUBLE PRECISION NOT NULL,
    post_date DATE NOT NULL,
    merchant_number VARCHAR(15) NOT NULL,
    merchant_description VARCHAR(60) NOT NULL,
    merchant_category_code INT NOT NULL,
    transaction_number INT NOT NULL,
    PRIMARY KEY(account_number, transaction_number),
    FOREIGN KEY(account_number) REFERENCES save.account_info(account_number)
);

COPY save.transactions(account_number, 
                       transaction_datetime,
                       transaction_amount,
                       post_date,
                       merchant_number,
                       merchant_description,
                       merchant_category_code,
                       transaction_number)
     FROM '/Users/iliar00t/python_tests/Save/transactions_fixed.csv'
     DELIMITER ','
     CSV HEADER;

GRANT CONNECT ON DATABASE save to save;
GRANT SELECT ON save.transactions, save.account_info TO save;

-- for testing
GRANT CREATE on schema save to save;

