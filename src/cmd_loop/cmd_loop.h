#ifndef __CMD_LOOP_H__
#define __CMD_LOOP_H__

typedef struct {
    char* cmdName;
    uint8_t (*handler)(const char* inCmd);
} SCmdEntry;

void
mainLoop();

extern SCmdEntry cmdEntries[];

//errors
#define CMD_OK                   0
#define CMD_ERR_NOT_ALLOWED      1
#define CMD_ERR_INVALID_ARG      2
#define CMD_ERR_PARSE            3
#define CMD_ERR_INVALID_CMD      4
#define CMD_ERR_INVALID_STATE    5

#endif
