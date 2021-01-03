#include "database.h"

int createDatabase(ServerStatus *status) {
    int error = sqlite3_open("server.db", &status->db);
    if (error != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(status->db));
        sqlite3_close(status->db);
    }
    return error;
}

int createTableCheck(ServerStatus *status, char *sqlStatement, char *tableName) {
    int error;
    char *errMsg = 0;

    error = sqlite3_exec(status->db, sqlStatement, NULL, 0, &errMsg);

    if (error != SQLITE_OK) {
        fprintf(stderr, "SQL error during creating table %s: %s\n", tableName, errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(status->db);
    }
    return error;
}

int createTables(ServerStatus *status) {

    char *sqlStatement;
    int error;
    // Creating table USER
    sqlStatement = "CREATE TABLE IF NOT EXISTS USER("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT        UNIQUE  NOT NULL," \
        "PASSWORD       TEXT        NOT NULL" \
        ");";

    error = createTableCheck(status, sqlStatement, "USER");
    if (error != SQLITE_OK) {
        return error;
    }


    // Creating table CHANNEL
    sqlStatement = "CREATE TABLE IF NOT EXISTS CHANNEL("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "NAME           TEXT    NOT NULL" \
        ");";

    error = createTableCheck(status, sqlStatement, "CHANNEL");
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating table USER_CHANNEL
    sqlStatement = "CREATE TABLE IF NOT EXISTS USER_CHANNEL("  \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "PRIMARY KEY(USER_ID, CHANNEL_ID)," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";

    error = createTableCheck(status, sqlStatement, "USER_CHANNEL");
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating table POST
    sqlStatement = "CREATE TABLE IF NOT EXISTS POST("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "CONTENT        TEXT        NOT NULL," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)" \
        ");";

    error = createTableCheck(status, sqlStatement, "POST");
    if (error != SQLITE_OK) {
        return error;
    }

    // Creating table NOTICE
    sqlStatement = "CREATE TABLE IF NOT EXISTS NOTICE("  \
        "ID             INTEGER     PRIMARY KEY     AUTOINCREMENT   NOT NULL," \
        "USER_ID        INTEGER     NOT NULL," \
        "CHANNEL_ID     INTEGER     NOT NULL," \
        "POST_ID        INTEGER     NOT NULL," \
        "FOREIGN KEY(USER_ID) REFERENCES USER(ID)," \
        "FOREIGN KEY(CHANNEL_ID) REFERENCES CHANNEL(ID)," \
        "FOREIGN KEY(POST_ID) REFERENCES POST(ID)" \
        ");";

    error = createTableCheck(status, sqlStatement, "NOTICE");
    return error;
}

int insertUser(ServerStatus *status, User *user) {

    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "INSERT INTO USER(NAME, PASSWORD) VALUES (?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement INSERT USER: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter NAME with value %s in INSERT USER: %s\n",
                status->programName, user->name, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 2, user->password, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter PASSWORD with value %s in INSERT USER: %s\n",
                status->programName, user->password, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during inserting USER: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertChannel(ServerStatus *status, Channel *channel) {

    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "INSERT INTO CHANNEL(NAME) VALUES (?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement INSERT CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, channel->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter NAME with value %s in INSERT CHANNEL: %s\n",
                status->programName, channel->name, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during inserting CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertPost(ServerStatus *status, Post *post) {

    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "INSERT INTO POST(USER_ID, CHANNEL_ID, CONTENT) VALUES (?, ?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement INSERT CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, post->userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in INSERT POST: %s\n",
                status->programName, post->userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, post->channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CHANNEL_ID with value %d in INSERT POST: %s\n",
                status->programName, post->channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 3, post->content, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CONTENT with value %s in INSERT POST: %s\n",
                status->programName, post->content, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during inserting POST: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertSubscription(ServerStatus *status, int userId, int channelId) {

    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "INSERT INTO USER_CHANNEL(USER_ID, CHANNEL_ID) VALUES (?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement INSERT USER_CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in INSERT USER_CHANNEL: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CHANNEL_ID with value %d in INSERT USER_CHANNEL: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during inserting USER_CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertNotice(ServerStatus *status, int userId, int channelId, int postId) {
    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "INSERT INTO NOTICE(USER_ID, CHANNEL_ID, POST_ID) VALUES (?, ?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement INSERT NOTICE: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in INSERT NOTICE: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CHANNEL_ID with value %d in INSERT NOTICE: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 3, postId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter POST_ID with value %d in INSERT NOTICE: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during inserting NOTICE: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int deleteSubscription(ServerStatus *status, int userId, int channelId) {
    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "DELETE FROM USER_CHANNEL WHERE USER_ID = ? AND CHANNEL_ID = ?";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement DELETE USER_CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in DELETE USER_CHANNEL: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CHANNEL_ID with value %d in DELETE USER_CHANNEL: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during deleting USER_CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int deleteNotice(ServerStatus *status, int userId, int channelId) {
    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "DELETE FROM NOTICE WHERE USER_ID = ? AND CHANNEL_ID = ?";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement DELETE NOTICE: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in DELETE NOTICE: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter CHANNEL_ID with value %d in DELETE NOTICE: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        fprintf(stderr, "%s: SQL error during deleting NOTICE: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectUserByName(ServerStatus *status, User *user) {
    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "SELECT ID, NAME, PASSWORD FROM USER WHERE NAME = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT USER: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %s in SELECT USER: %s\n",
                status->programName, user->name, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error == SQLITE_ROW) {
        user->id = sqlite3_column_int(stmt, 0);
        char *text = (char *) sqlite3_column_text(stmt, 1);
        user->name = (char *) malloc(sizeof(char) * (strlen(text) + 1));
        strcpy(user->name, text);
        text = (char *) sqlite3_column_text(stmt, 2);
        user->password = (char *) malloc(sizeof(char) * (strlen(text) + 1));
        strcpy(user->password, text);
    } else if (error == SQLITE_DONE) {
        user->name = NULL;
    } else {
        fprintf(stderr, "%s: SQL error during selecting  USER: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectNewPostIdByUserId(ServerStatus *status, int userId, int *postId) {
    int error;
    sqlite3_stmt *stmt;
    const char *sqlStatement = "SELECT ID FROM POST WHERE USER_ID = ? ORDER BY ID DESC;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT USER: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in SELECT POST: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error == SQLITE_ROW) {
        *postId = sqlite3_column_int(stmt, 0);
    } else if (error == SQLITE_DONE) {
        postId = NULL;
    } else {
        fprintf(stderr, "%s: SQL error during selecting POST: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectNoticesByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt) {
    int error;
    const char *sqlStatement = "SELECT CHANNEL_ID FROM NOTICE WHERE USER_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT NOTICE: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in SELECT NOTICE: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_step(*stmt);

    return error;
}

int selectUsersByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt) {
    int error;
    const char *sqlStatement = "SELECT USER_ID FROM USER_CHANNEL WHERE CHANNEL_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT USER_CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in SELECT USER_CHANNEL: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }
    error = sqlite3_step(*stmt);
    return error;
}

int selectChannelsByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt) {
    int error;
    const char *sqlStatement = "SELECT c.ID, c.NAME FROM USER_CHANNEL " \
                               "INNER JOIN CHANNEL C on C.ID = USER_CHANNEL.CHANNEL_ID" \
                               "WHERE USER_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, userId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in SELECT CHANNEL: %s\n",
                status->programName, userId, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_step(*stmt);
    return error;
}

int selectPostByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt) {
    int error;
    const char *sqlStatement = "SELECT p.ID, u.NAME, p.CONTENT " \
                               "FROM POST p " \
                               "INNER JOIN USER U on U.ID = p.USER_ID " \
                               "WHERE CHANNEL_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during preparing statement SELECT CHANNEL: %s\n",
                status->programName, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, channelId);
    if (error != SQLITE_OK) {
        fprintf(stderr, "%s: SQL error during binding parameter USER_ID with value %d in SELECT CHANNEL: %s\n",
                status->programName, channelId, sqlite3_errmsg(status->db));
        sqlite3_finalize(*stmt);
        return error;
    }

    error = sqlite3_step(*stmt);
    return error;
}


