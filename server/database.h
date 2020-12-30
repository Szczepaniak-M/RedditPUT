#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H


#include <sqlite3.h>
#include <stdio.h>

#include "structures.h"


int createDatabase(ServerStatus *status);

int createTables(ServerStatus *status);

int insertUser(ServerStatus *status, User *user);

int insertChannel(ServerStatus *status, Channel *channel);

int insertPost(ServerStatus *status, Post *post);


#endif
