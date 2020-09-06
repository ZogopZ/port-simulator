#ifndef _CLI_UTILS_H_
#define _CLI_UTILS_H_

void ParseCliArgs(int argc, char** argv, int caller);
char* GetConfigFile(void);
char* GetStrTime(void);
char* GetStrType(void);
char* GetStrPosType(void);
char* GetStrParkPeriod(void);
char* GetStrManTime(void);
char* GetStrShmid(void);

#endif /* _CLI_UTILS_H_ */
