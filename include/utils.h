#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_STR        64
#define MAX_ITEMS      128
#define MAX_RESULTS    10
#define INITIAL_STOCK  50

/*  DO NOT MODIFY */
typedef enum
{
    LIST_ITEMS,       // 0 - client requests full item list
    SEARCH_ITEM,      // 1 - client searches for an item by keyword
    GET_STOCK,        // 2 - client requests stock of a specific item
    BUY_ITEM,         // 3 - client requests to buy an item
    SELL_ITEM,        // 4 - client requests to sell (add stock) an item

    ITEM_LIST,        // 5 - server responds with item list
    SEARCH_RESULTS,   // 6 - server responds with search results
    STOCK_INFO,       // 7 - server responds with stock info
    BUY_OK,           // 8 - server confirms purchase
    SELL_OK,          // 9 - server confirms sale

    ERROR_MSG,        // 10 - server responds with error
    TERMINATE,        // 11 - shutdown signal

    ENC_SEARCH_ITEM,  // 12 - BONUS: client searches with a ciphered query
                      //       (Caesar shift-right-by-2 on the query bytes;
                      //       server decrypts, then runs a normal search
                      //       and replies with SEARCH_RESULTS as usual)
} msg_enum;

// an item in the marketplace
struct item
{
    char name[MAX_STR];
    int  stock;
    float price;
};

void bookeepingCode();

// --- BONUS: Caesar cipher (shift right by 2) ---
// Operate in place on a NUL-terminated string in a char[MAX_STR] buffer.
// Rules:
//   'a'..'z' shift +2 with wrap ('y'->'a', 'z'->'b')
//   'A'..'Z' shift +2 with wrap ('Y'->'A', 'Z'->'B')
//   everything else (digits, punctuation, spaces) is left unchanged
// decrypt_str is the inverse (shift -2 with wrap).
void encrypt_str(char *s);
void decrypt_str(char *s);

#endif
