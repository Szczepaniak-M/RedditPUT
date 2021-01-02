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

int createStatementSelectNoticesByUserId(ServerStatus *status, int userId, sqlite3_stmt **stmt);

int createStatementSelectUsersByChannelId(ServerStatus *status, int channelId, sqlite3_stmt **stmt);


#endif
