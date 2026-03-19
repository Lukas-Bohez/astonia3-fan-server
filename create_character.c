#include <stdio.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#define EXTERNAL_PROGRAM
#include "server.h"
#undef EXTERNAL_PROGRAM
#include "drdata.h"
#include "config.h"

static MYSQL mysql;

int init_database(void) {
    // init database client
    if (!mysql_init(&mysql)) return 0;

    // try to login as root with our password
    if (!mysql_real_connect(&mysql, config_data.dbhost, config_data.dbuser, config_data.dbpass, config_data.dbname, 0, NULL, 0)) {
        fprintf(stderr, "MySQL error: %s (%d)\n", mysql_error(&mysql), mysql_errno(&mysql));
        return 0;
    }

    return 1;
}

void exit_database(void) {
    mysql_close(&mysql);
}

#define WANTSIZE 512
#define CHARINFO

int create_char(int user_ID, char *new_user, char *class) {
    char data[80];
    unsigned long long flag = 0;
    int size, expandto, add, mirror;
    char buf[WANTSIZE * 2 + 256], dbuf[WANTSIZE * 2], ddata[WANTSIZE];

    if (class[2] == 'G') flag |= CF_GOD;

    if (class[1] == 'W') flag |= CF_WARRIOR;
    else flag |= CF_MAGE;

    if (class[0] == 'M') flag |= CF_MALE;
    else flag |= CF_FEMALE;

    unsigned long data_len = mysql_real_escape_string(&mysql, data, (char *)&flag, sizeof(flag));
    data[data_len] = '\0';

    size = 8 + 8 + strlen(new_user) + 15 * 4 + 1 + 3 * 2 + 1 + 6 - 20;
    expandto = ((size + (WANTSIZE - 1) + 9) / WANTSIZE) * WANTSIZE;
    add = expandto - size;

    *(unsigned int *)(ddata + 0) = DRD_JUNK_PPD;
    *(unsigned int *)(ddata + 4) = add - 8;
    memset(ddata + 8, 0, add - 8);

    unsigned long dbuf_len = mysql_real_escape_string(&mysql, dbuf, ddata, add);
    dbuf[dbuf_len] = '\0';

    mirror = RANDOM(26) + 1;

    {
        const char *sql = "INSERT INTO chars VALUES (0, ?, ?, 0, 0, 0, 0, 0, 0, 1, ?, 1, 1, 'N', ?, ?, ?, ?, ?, 0, 1)";
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

        MYSQL_BIND bind[8];
        memset(bind, 0, sizeof(bind));

        unsigned long name_len = (unsigned long)strlen(new_user);
        unsigned long creation_time = (unsigned long)time(NULL);
        unsigned long class_flag = (unsigned long)(flag & 0xffffffff);

        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = new_user;
        bind[0].buffer_length = name_len;
        bind[0].length = &name_len;

        bind[1].buffer_type = MYSQL_TYPE_LONG;
        bind[1].buffer = (char *)&class_flag;

        bind[2].buffer_type = MYSQL_TYPE_LONG;
        bind[2].buffer = (char *)&creation_time;

        bind[3].buffer_type = MYSQL_TYPE_LONG;
        bind[3].buffer = (char *)&user_ID;

        bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
        bind[4].buffer = data;
        bind[4].buffer_length = data_len;
        bind[4].length = &data_len;

        bind[5].buffer_type = MYSQL_TYPE_VAR_STRING;
        bind[5].buffer = data;
        bind[5].buffer_length = data_len;
        bind[5].length = &data_len;

        bind[6].buffer_type = MYSQL_TYPE_VAR_STRING;
        bind[6].buffer = dbuf;
        bind[6].buffer_length = dbuf_len;
        bind[6].length = &dbuf_len;

        bind[7].buffer_type = MYSQL_TYPE_LONG;
        bind[7].buffer = (char *)&mirror;

        if (mysql_stmt_bind_param(stmt, bind) != 0) {
            fprintf(stderr, "Failed to bind chars params: %s (%d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
            mysql_stmt_close(stmt);
            return 2;
        }

        if (mysql_stmt_execute(stmt) != 0) {
            unsigned int errnum = mysql_stmt_errno(stmt);
            if (errnum == ER_DUP_ENTRY) {
                fprintf(stderr, "Sorry, the name %s is already taken.\n", new_user);
                mysql_stmt_close(stmt);
                return 1;
            }
            fprintf(stderr, "Failed to create account %s: Error: %s (%d)\n", new_user, mysql_stmt_error(stmt), errnum);
            mysql_stmt_close(stmt);
            return 2;
        }

        mysql_stmt_close(stmt);
    }
    if (mysql_errno(&mysql) == ER_DUP_ENTRY) {
        fprintf(stderr, "Sorry, the name %s is already taken.\n", new_user);
        return 1;
    } else {
        fprintf(stderr, "Failed to create account %s: Error: %s (%d)\n", new_user, mysql_error(&mysql), mysql_errno(&mysql));
        return 2;
    }
}
else {
#ifdef CHARINFO
    {
        const char *sql2 = "INSERT INTO charinfo VALUES (?, ?, ?, 0, 0, 0, 0, 0, 0, ?, 1, 1, 1, 'N', ?)";
        MYSQL_STMT *stmt2 = mysql_stmt_init(&mysql);
        if (!stmt2) {
            fprintf(stderr, "Failed to init charinfo statement: %s\n", mysql_error(&mysql));
            return 2;
        }

        if (mysql_stmt_prepare(stmt2, sql2, (unsigned long)strlen(sql2)) != 0) {
            fprintf(stderr, "Failed to prepare charinfo statement: %s (%d)\n", mysql_stmt_error(stmt2), mysql_stmt_errno(stmt2));
            mysql_stmt_close(stmt2);
            return 2;
        }

        MYSQL_BIND bind2[5];
        memset(bind2, 0, sizeof(bind2));

        int insertedId = (int)mysql_insert_id(&mysql);
        unsigned long name_len = (unsigned long)strlen(new_user);
        unsigned long class_flag = (unsigned long)(flag & 0xffffffff);
        unsigned long creation_time = (unsigned long)time(NULL);
        int userID = user_ID;

        bind2[0].buffer_type = MYSQL_TYPE_LONG;
        bind2[0].buffer = &insertedId;

        bind2[1].buffer_type = MYSQL_TYPE_STRING;
        bind2[1].buffer = new_user;
        bind2[1].buffer_length = name_len;
        bind2[1].length = &name_len;

        bind2[2].buffer_type = MYSQL_TYPE_LONG;
        bind2[2].buffer = (char *)&class_flag;

        bind2[3].buffer_type = MYSQL_TYPE_LONG;
        bind2[3].buffer = (char *)&creation_time;

        bind2[4].buffer_type = MYSQL_TYPE_LONG;
        bind2[4].buffer = &userID;

        if (mysql_stmt_bind_param(stmt2, bind2) != 0) {
            fprintf(stderr, "Failed to bind charinfo params: %s (%d)\n", mysql_stmt_error(stmt2), mysql_stmt_errno(stmt2));
            mysql_stmt_close(stmt2);
            return 2;
        }

        if (mysql_stmt_execute(stmt2) != 0) {
            fprintf(stderr, "Failed to create charinfo %s: Error: %s (%d)\n", new_user, mysql_stmt_error(stmt2), mysql_stmt_errno(stmt2));
            mysql_stmt_close(stmt2);
            // continue; // failure is nonfatal for charinfo
        }

        mysql_stmt_close(stmt2);
    }
#endif
}

return 0;
}

void help(char *prog) {
    fprintf(stderr, "Usage: %s [-s name=value] [-f filename] [-e] <accountID> <name> <genderandclassandgod>\ngenderandclassandgod: MW = male warrior, FM = female mage, FWG = female warrior god\n-s Set config name to value (e.g. dbhost=localhost).\n-f Read config file <filename>.\n-e Read configuration from environment variables.\n", prog);
}

int main(int argc, char **args) {
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

    if (argc - optind != 3) {
        help(args[0]);
        return 1;
    }

    if (!init_database()) {
        fprintf(stderr, "Cannot connect to database.\n");
        return 3;
    }

    if (!create_char(atoi(args[optind]), args[optind + 1], args[optind + 2])) {
        printf("Success.\n");
    }

    exit_database();

    return 0;
}
