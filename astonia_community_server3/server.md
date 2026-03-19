# server

Example command line:
```
./server -a 3 -m 13 -s dbhost=database.astonia.com -s dbuser=as3 -s dbpass="top secret" -s dbname=merc_a -s chathost=chat.astonia.com
```

Example config file. Invocation:
```
./server -a 3 -m 13 -f server.cnf
```
server.cnf:
```
dbhost=database.astonia.com
dbuser=as3
dbpass="top secret"
dbname=merc_a
chathost=chat.astonia.com
```

Example environment variables:
```
export AS3_DBHOST=database.astonia.com
export AS3_DBUSER=as3
export AS3_DBPASS="top secret"
export AS3_DBNAME=merc_a
export AS3_CHATHOST=chat.astonia.com

./server -a 3 -m 13 -e # -e reads environment variables.
```

All options have sensible defaults (localhost, the password from MYSQLPASS, merc and localhost, respectively.)
Command line option order matters. Later settings overwrite earlier ones.
