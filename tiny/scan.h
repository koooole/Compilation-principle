#ifndef SCAN_H
#define SCAN_H

#define MAXTOKENLEN 40

extern char tokenString[MAXTOKENLEN+1];

void getTokenInit(void);

TokenType getToken(void);

#endif
