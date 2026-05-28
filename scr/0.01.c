#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define REG_PATH "Software\\Classes\\.ck"
#define REG_COMMAND "Software\\Classes\\clonke\\shell\\open\\command"

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintUse(const char* code, const char* line) {
    SetColor(14);
    printf("use: %s{%s}000\n", code, line);
    SetColor(7);
}

void PrintWarn(const char* code, const char* line) {
    SetColor(6);
    printf("warn: %s{%s}000\n", code, line);
    SetColor(7);
}

void PrintErr(const char* code, const char* line) {
    SetColor(12);
    printf("err: %s{%s}000\n", code, line);
    SetColor(7);
}

void WriteRegistry(void) {
    char exePath[MAX_PATH];
    char command[MAX_PATH + 20];
    
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    snprintf(command, sizeof(command), "\"%s\" \"%%1\"", exePath);
    
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, REG_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)"clonke", 9);
        RegCloseKey(hKey);
    }
    
    if (RegCreateKeyExA(HKEY_CURRENT_USER, REG_COMMAND, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)command, strlen(command) + 1);
        RegCloseKey(hKey);
    }
}

void EnsureDirectoriesAndLog(void) {
    char logPath[MAX_PATH];
    char logFile[MAX_PATH];
    FILE* f;
    
    CreateDirectoryA("C:\\clonke", NULL);
    snprintf(logPath, sizeof(logPath), "C:\\clonke\\logs");
    CreateDirectoryA(logPath, NULL);
    
    snprintf(logFile, sizeof(logFile), "C:\\clonke\\logs\\entercode.txt");
    f = fopen(logFile, "r");
    if (!f) {
        f = fopen(logFile, "w");
        if (f) {
            fprintf(f, "1");
            fclose(f);
        }
        WriteRegistry();
    } else {
        fclose(f);
    }
}

void parseAndExecute(const char* line, int lineNum) {
    char content[1024];
    int num;
    int len = strlen(line);
    
    if (len == 0) {
        PrintUse("1000.0", "");
        PrintErr("unspecified command", "");
        return;
    }
    
    if (strstr(line, "t.text(") == line) {
        if (sscanf(line, "t.text(\"%[^\"]\")", content) == 1) {
            printf("%s\n", content);
        } else if (sscanf(line, "t.text(%d)", &num) == 1) {
            PrintUse("100.0.", line);
            PrintWarn("use double input", line);
            PrintErr("enter the correct syntax for text()", line);
        } else {
            PrintUse("100.0.", line);
            PrintErr("enter the correct syntax for text()", line);
        }
    }
    else if (strstr(line, "n.text(") == line) {
        if (sscanf(line, "n.text(%d)", &num) == 1) {
            printf("%d\n", num);
        } else if (strstr(line, "\"")) {
            PrintUse("1110.", line);
            PrintErr("enter the correct syntax for text()", line);
        } else {
            PrintUse("1110.", line);
            PrintErr("enter the correct syntax for text()", line);
        }
    }
    else if (strstr(line, "nt.text(") == line) {
        if (sscanf(line, "nt.text(\"%[^\"]\")", content) == 1) {
            printf("%s\n", content);
        } else if (sscanf(line, "nt.text(%d)", &num) == 1) {
            PrintUse("010.0", line);
            PrintWarn("use double input", line);
            PrintErr("enter the correct syntax for text()", line);
        } else {
            PrintUse("010.0", line);
            PrintErr("enter the correct syntax for text()", line);
        }
    }
    else if (strstr(line, "text(") == line) {
        PrintUse("0000.", line);
        PrintErr("flag n, t or nt not defined", line);
    }
    else {
        PrintUse("1110.", line);
        PrintErr("incorrect arrangement of characters", line);
    }
}

void interpretFile(const char* filename) {
    FILE* f = fopen(filename, "r");
    char line[1024];
    int lineNum = 0;
    
    if (!f) {
        printf("Cannot open file: %s\n", filename);
        return;
    }
    
    while (fgets(line, sizeof(line), f)) {
        lineNum++;
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0 || lineNum > 0) {
            parseAndExecute(line, lineNum);
        }
    }
    
    fclose(f);
}

void interactiveMode(void) {
    char input[1024];
    int lineNum = 0;
    
    SetColor(7);
    printf("clonke 0.01\n");
    printf("type 'exit' to quit\n\n");
    
    while (1) {
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0) {
            break;
        }
        
        lineNum++;
        if (strlen(input) > 0) {
            parseAndExecute(input, lineNum);
        } else {
            PrintUse("1000.0", "");
            PrintErr("unspecified command", "");
        }
    }
}

int main(int argc, char* argv[]) {
    EnsureDirectoriesAndLog();
    
    if (argc > 1) {
        interpretFile(argv[1]);
    } else {
        interactiveMode();
    }
    
    return 0;
}
