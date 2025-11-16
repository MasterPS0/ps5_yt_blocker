#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <sqlite3.h>

// ====================== Definitions ======================
#define DB_PATH              "/system_data/priv/mms/appinfo.db"
#define TITLE_ID             "PPSA01650"
#define NEW_CONTENT_VERSION  "99.999.999"
#define NEW_VERSION_FILE_URI "http://127.0.0.2"

#define APP_ID               "YTBLK0001"
#define LOG_PATH             "/user/app/" APP_ID "/log.txt"
#define BACKUP_DIR           "/user/app/" APP_ID "/backup"
#define BACKUP_PATH          BACKUP_DIR "/appinfo.org.db"

//====================== PS5 Notices ======================
typedef struct {
    char useless1[45];
    char message[3075];
} notify_request_t;

extern int sceKernelSendNotificationRequest(int device, notify_request_t* req, size_t size, int flags);

// ====================== Helper Functions ======================
int file_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0);
}

void mkdir_recursive(const char *path) {
    char tmp[512];
    char *p = NULL;
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

void write_log(const char *fmt, ...) {
    FILE *f = fopen(LOG_PATH, "a");
    if (!f) return;
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    timestr[strlen(timestr)-1] = '\0';
    fprintf(f, "[%s] ", timestr);
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fclose(f);
}

void send_notification(const char *fmt, ...) {
    notify_request_t req = {0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(req.message, sizeof(req.message), fmt, ap);
    va_end(ap);
    sceKernelSendNotificationRequest(0, &req, sizeof(req), 0);
    va_list ap2;
    va_start(ap2, fmt);
    write_log(fmt, ap2);
    va_end(ap2);
}

// ====================== Backup ======================
int create_backup() {
    if (!file_exists(DB_PATH)) {
        send_notification("Error: appinfo.db not found!\n%s", DB_PATH);
        return -1;
    }
    mkdir_recursive(BACKUP_DIR);
    if (file_exists(BACKUP_PATH)) {
        send_notification("Backup exists:\n%s", BACKUP_PATH);
        return 0;
    }
    FILE *src = fopen(DB_PATH, "rb");
    if (!src) {
        send_notification("Open DB failed: %s", strerror(errno));
        return -1;
    }
    FILE *dst = fopen(BACKUP_PATH, "wb");
    if (!dst) {
        fclose(src);
        send_notification("Create backup failed: %s", strerror(errno));
        return -1;
    }
    char buf[8192];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, n, dst);
    }
    fclose(src); fclose(dst);
    send_notification("Backup created:\n%s", BACKUP_PATH);
    return 0;
}

// ====================== Block Updates ======================
int block_update() {
    sqlite3 *db;
    if (sqlite3_open(DB_PATH, &db) != SQLITE_OK) {
        send_notification("DB open failed:\n%s", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) FROM tbl_appinfo WHERE titleId = ? AND key IN ('CONTENT_VERSION', 'VERSION_FILE_URI');";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        send_notification("SQL error: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    sqlite3_bind_text(stmt, 1, TITLE_ID, -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count != 2) {
        send_notification("Error: Found %d entries (need 2)", count);
        sqlite3_close(db);
        return -1;
    }

    const char *updates[] = {
        "UPDATE tbl_appinfo SET val = ? WHERE titleId = ? AND key = 'CONTENT_VERSION';",
        "UPDATE tbl_appinfo SET val = ? WHERE titleId = ? AND key = 'VERSION_FILE_URI';"
    };
    const char *values[] = { NEW_CONTENT_VERSION, NEW_VERSION_FILE_URI };

    for (int i = 0; i < 2; i++) {
        if (sqlite3_prepare_v2(db, updates[i], -1, &stmt, NULL) != SQLITE_OK) continue;
        sqlite3_bind_text(stmt, 1, values[i], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, TITLE_ID, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    const char *verify = "SELECT key, val FROM tbl_appinfo WHERE titleId = ? AND key IN ('CONTENT_VERSION', 'VERSION_FILE_URI');";
    sqlite3_prepare_v2(db, verify, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, TITLE_ID, -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        send_notification("%s = %s",
            (const char*)sqlite3_column_text(stmt, 0),
            (const char*)sqlite3_column_text(stmt, 1));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    send_notification("YT Update Blocked Successfully!");
    return 0;
}

// ====================== Main ======================
int main() {
    sceSysmoduleLoadModule(ORBIS_SYSMODULE_SAVE_DATA);
    mkdir_recursive("/user/app/" APP_ID);
    write_log("=== YT Update Blocker Started ===");
    send_notification("YT Update Blocker v1.0");

    if (create_backup() != 0) return -1;
    if (block_update() != 0) return -1;

    write_log("=== Success ===");
    return 0;
}