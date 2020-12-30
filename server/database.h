#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stdio.h>
#include "structures.h"

int createDatabase(ServerStatus *status);

int createTables(ServerStatus *status);

#endif
