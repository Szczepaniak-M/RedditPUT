#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H


#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structures.h"


int createDatabase(ServerStatus *status);

int createTables(ServerStatus *status);

int insertUser(ServerStatus *status, User *user);

int insertChannel(ServerStatus *status, Channel *channel);

int insertPost(ServerStatus *status, Post *post);

int insertSubscription(ServerStatus *status, int userId, int channelId);

int insertNotice(ServerStatus *status, int userId, int channelId, int postId);

int deleteSubscription(ServerStatus *status, int userId, int channelId);

int deleteNotice(ServerStatus *status, int userId, int channelId);

int selectUserByName(ServerStatus *status, User *user);

int selectNewPostIdByUserId(ServerStatus *status, int userId, int *postId);

int selectNoticesByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt);

int selectUsersByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt);

int selectChannelsByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt);

int selectPostByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt);

int selectAllChannels(ServerStatus *status, sqlite3_stmt **stmt);

void prepareError(ServerStatus *status, const char *operationName, sqlite3_stmt *stmt);

void bindIntError(ServerStatus *status, char *parameterName, int value, const char *operationName, sqlite3_stmt *stmt);

void bindTextError(ServerStatus *status, char *parameterName, char *value, const char *operationName, sqlite3_stmt *stmt);

void executeError(ServerStatus *status, const char *operationName, sqlite3_stmt *stmt);


#endif
