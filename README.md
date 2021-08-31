# save

<pre>Install instructions for libpqxx
$ brew install libpq postresql
$ git clone https://github.com/jtv/libpqxx.git
$ cd libpqxx/
$ CXX=clang++ ./configure --enable-shared
$ make -j8
$ PGHOST=localhost PGPORT=5432 PGDATABASE=save PGUSER=save PGPASSWORD=secret make check
$ sudo make install

import data instructions
$ ./fix_input.sh
and follow instructions from schema.sql


build instructions
$ clang++ -std=c++17 -I/usr/local/include -o save -lpqxx -lpq save.cxx

run instructions
$ ./save postgresql://save:secret@localhost/save

export instructions:
COPY save.account_info TO &apos;&lt;full_path&gt;/account_info_export.csv&apos; DELIMITER &apos;,&apos; CSV HEADER;
COPY save.transactions TO &apos;&lt;full_path&gt;/transactions_export.csv&apos; DELIMITER &apos;,&apos; CSV HEADER;</pre>
