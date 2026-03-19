#include <stdio.h>
#include <time.h>
#include <getopt.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include "argon.h"
#include "config.h"

static MYSQL mysql;

int init_database(void) {
    // init database client
    if (!mysql_init(&mysql)) return 0;

    // try to login to database using config data
    if (!mysql_real_connect(&mysql, config_data.dbhost, config_data.dbuser, config_data.dbpass, config_data.dbname, 0, NULL, 0)) {
        fprintf(stderr, "MySQL error: %s (%d)\n", mysql_error(&mysql), mysql_errno(&mysql));
        return 0;
    }

    return 1;
}

void exit_database(void) {
    mysql_close(&mysql);
}

void help(char *prog) {
    fprintf(stderr, "Usage: %s [-s name=value] [-f filename] [-e] <email> <password>\n\n-s Set config name to value (e.g. dbhost=localhost).\n-f Read config file <filename>.\n-e Read configuration from environment variables.\n", prog);
}

int main(int argc, char **args) {
    char hash[512];
    int c;

    config_init();
    while (1) {
        c = getopt(argc, args, "s:f:e");
        if (c == -1) break;
        switch (c) {
        case 'h':
            help(args[0]);
            exit(0);
        case 's':
            config_string(optarg);
            break;
        case 'f':
            config_file(optarg);
            break;
        case 'e':
            config_getenv();
            break;
        }
    }

    if (argc - optind != 2) {
        help(args[0]);
        return 1;
    }

    if (!init_database()) {
        fprintf(stderr, "Cannot connect to database.\n");
        return 3;
    }

    if (argon2id_hash_password(hash, sizeof(hash), args[optind + 1], NULL)) {
        fprintf(stderr, "Argon failed. Call Dad!\n");
        return 2;
    }

    {
        const char *sql = "INSERT INTO subscriber (email,password,creation_time,locked,banned,vendor) VALUES (?, ?, ?, 'N', 'I', ?)";
        MYSQL_STMT *stmt = mysql_stmt_init(&mysql);
        if (!stmt) {
            fprintf(stderr, "Failed to init statement: %s\n", mysql_error(&mysql));
            return 2;
        }

        if (mysql_stmt_prepare(stmt, sql, (unsigned long)strlen(sql)) != 0) {
            fprintf(stderr, "Failed to prepare statement: %s (%d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
            mysql_stmt_close(stmt);
            return 2;
        }

        MYSQL_BIND bind[4];
        memset(bind, 0, sizeof(bind));

        unsigned long email_len = (unsigned long)strlen(args[optind]);
        unsigned long hash_len = (unsigned long)strlen(hash);
        int creation_time = (int)time(NULL);
        int vendor = 0;

        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = args[optind];
        bind[0].buffer_length = email_len;
        bind[0].length = &email_len;

        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = hash;
        bind[1].buffer_length = hash_len;
        bind[1].length = &hash_len;

        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = &creation_time;

        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = &vendor;

        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            fprintf(stderr, "Failed to bind params: %s (%d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
            mysql_stmt_close(stmt);
            return 2;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            fprintf(stderr, "Failed to create subscriber: Error: %s (%d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
            mysql_stmt_close(stmt);
            return 2;
        }

        unsigned long long inserted_id = mysql_insert_id(&mysql);
        printf("Success. Account ID is %llu.\n", inserted_id);

        mysql_stmt_close(stmt);
    }

    exit_database();

    return 0;
}
