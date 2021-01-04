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
    const char *operationName = "insertUser";
    const char *sqlStatement = "INSERT INTO USER(NAME, PASSWORD) VALUES (?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "NAME", user->name, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 2, user->password, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "PASSWORD", user->password, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertChannel(ServerStatus *status, Channel *channel) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "insertChannel";
    const char *sqlStatement = "INSERT INTO CHANNEL(NAME) VALUES (?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, channel->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "NAME", channel->name, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertPost(ServerStatus *status, Post *post) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "insertPost";
    const char *sqlStatement = "INSERT INTO POST(USER_ID, CHANNEL_ID, CONTENT) VALUES (?, ?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, post->userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", post->userId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, post->channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", post->channelId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 3, post->content, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "CONTENT", post->content, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertSubscription(ServerStatus *status, int userId, int channelId) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "insertSubscription";
    const char *sqlStatement = "INSERT INTO USER_CHANNEL(USER_ID, CHANNEL_ID) VALUES (?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int insertNotice(ServerStatus *status, int userId, int channelId, int postId) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "insertNotice";
    const char *sqlStatement = "INSERT INTO NOTICE(USER_ID, CHANNEL_ID, POST_ID) VALUES (?, ?, ?);";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 3, postId);
    if (error != SQLITE_OK) {
        bindIntError(status, "POST_ID", postId, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int deleteSubscription(ServerStatus *status, int userId, int channelId) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "deleteSubscription";
    const char *sqlStatement = "DELETE FROM USER_CHANNEL WHERE USER_ID = ? AND CHANNEL_ID = ?";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int deleteNotice(ServerStatus *status, int userId, int channelId) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "deleteNotice";
    const char *sqlStatement = "DELETE FROM NOTICE WHERE USER_ID = ? AND CHANNEL_ID = ?";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 2, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error != SQLITE_DONE) {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectUserByName(ServerStatus *status, User *user) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "selectUserByName";
    const char *sqlStatement = "SELECT ID, NAME, PASSWORD FROM USER WHERE NAME = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, user->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "USER_NAME", user->name, operationName, stmt);
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
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectNewPostIdByUserId(ServerStatus *status, int userId, int *postId) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "selectNewPostIdByUserId";
    const char *sqlStatement = "SELECT ID FROM POST WHERE USER_ID = ? ORDER BY ID DESC LIMIT 1;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, stmt);
        return error;
    }

    error = sqlite3_step(stmt);
    if (error == SQLITE_ROW) {
        *postId = sqlite3_column_int(stmt, 0);
    } else {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectNoticesByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt) {
    int error;
    const char *operationName = "selectNoticesByUserId";
    const char *sqlStatement = "SELECT CHANNEL_ID FROM NOTICE WHERE USER_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, *stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, *stmt);
        return error;
    }
    error = sqlite3_step(*stmt);
    return error;
}

int selectUsersByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt) {
    int error;
    const char *operationName = "selectUsersByChannelId";
    const char *sqlStatement = "SELECT USER_ID FROM USER_CHANNEL WHERE CHANNEL_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, *stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, *stmt);
        return error;
    }
    error = sqlite3_step(*stmt);
    return error;
}

int selectChannelsByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt) {
    int error;
    const char *operationName = "selectChannelsByUserId";
    const char *sqlStatement = "SELECT C.ID, C.NAME FROM USER_CHANNEL U " \
                               "INNER JOIN CHANNEL C on C.ID = U.CHANNEL_ID " \
                               "WHERE USER_ID = ?;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, *stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, userId);
    if (error != SQLITE_OK) {
        bindIntError(status, "USER_ID", userId, operationName, *stmt);
        return error;
    }
    error = sqlite3_step(*stmt);
    return error;
}

int selectChannelIdByName(ServerStatus *status, Channel *channel) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "selectChannelIdByName";
    const char *sqlStatement = "SELECT ID FROM CHANNEL " \
                               "WHERE NAME = ? " \
                               "LIMIT 1;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_text(stmt, 1, channel->name, -1, SQLITE_TRANSIENT);
    if (error != SQLITE_OK) {
        bindTextError(status, "CHANNEL_NAME", channel->name, operationName, stmt);
        return error;
    }
    error = sqlite3_step(stmt);
    if (error == SQLITE_ROW) {
        channel->id = sqlite3_column_int(stmt, 0);
    } else if (error == SQLITE_DONE) {
        channel->id = -1;
    } else {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectChannelNameById(ServerStatus *status, Channel *channel) {
    int error;
    sqlite3_stmt *stmt;
    const char *operationName = "selectChannelNameById";
    const char *sqlStatement = "SELECT NAME FROM CHANNEL " \
                               "WHERE ID = ? " \
                               "LIMIT 1;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, &stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, stmt);
        return error;
    }

    error = sqlite3_bind_int(stmt, 1, channel->id);
    if (error != SQLITE_OK) {
        bindTextError(status, "CHANNEL_ID", channel->name, operationName, stmt);
        return error;
    }
    error = sqlite3_step(stmt);
    if (error == SQLITE_ROW) {
        char *text = (char *) sqlite3_column_text(stmt, 0);
        channel->name = (char *) malloc(sizeof(char) * (strlen(text) + 1));
        strcpy(channel->name, text);
    } else {
        executeError(status, operationName, stmt);
        return error;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int selectAllChannels(ServerStatus *status, sqlite3_stmt **stmt) {
    int error;
    const char *operationName = "selectAllChannels";
    const char *sqlStatement = "SELECT ID, NAME FROM CHANNEL;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, *stmt);
        return error;
    }

    error = sqlite3_step(*stmt);
    return error;
}

int selectPostByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt) {
    int error;
    const char *operationName = "selectPostByChannelId";
    const char *sqlStatement = "SELECT p.ID, u.NAME, p.CONTENT " \
                               "FROM POST p " \
                               "INNER JOIN USER U on U.ID = p.USER_ID " \
                               "WHERE CHANNEL_ID = ? "
                               "ORDER BY p.ID ASC;";

    error = sqlite3_prepare_v2(status->db, sqlStatement, -1, stmt, NULL);
    if (error != SQLITE_OK) {
        prepareError(status, operationName, *stmt);
        return error;
    }

    error = sqlite3_bind_int(*stmt, 1, channelId);
    if (error != SQLITE_OK) {
        bindIntError(status, "CHANNEL_ID", channelId, operationName, *stmt);
        return error;
    }
    error = sqlite3_step(*stmt);
    return error;
}

void prepareError(ServerStatus *status, const char *operationName, sqlite3_stmt *stmt) {
    fprintf(stderr, "%s: SQL error during preparing statement %s: %s\n",
            status->programName, operationName, sqlite3_errmsg(status->db));
    sqlite3_finalize(stmt);
}

void bindIntError(ServerStatus *status, char *parameterName, int value, const char *operationName, sqlite3_stmt *stmt) {
    fprintf(stderr, "%s: SQL error during binding parameter %s with value %d in %s: %s\n",
            status->programName, parameterName, value, operationName, sqlite3_errmsg(status->db));
    sqlite3_finalize(stmt);
}

void bindTextError(ServerStatus *status, char *parameterName, char *value, const char *operationName, sqlite3_stmt *stmt) {
    fprintf(stderr, "%s: SQL error during binding parameter %s with value %s in %s: %s\n",
            status->programName, parameterName, value, operationName, sqlite3_errmsg(status->db));
    sqlite3_finalize(stmt);
}

void executeError(ServerStatus *status, const char *operationName, sqlite3_stmt *stmt) {
    fprintf(stderr, "%s: SQL error during executing %s: %s\n",
            status->programName, operationName, sqlite3_errmsg(status->db));
    sqlite3_finalize(stmt);
}
