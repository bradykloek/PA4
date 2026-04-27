#include "client.h"

void printSyntax(){
    printf("incorrect usage syntax!\n");
    printf("usage: $ ./client server_addr server_port\n");
    printf("       (commands are read from stdin; use < input.txt to redirect)\n");
}

// ============================================================
// You write everything below except main() and the fprintf format
// strings inside each function. Figure out what each parameter
// means from the protocol in instruction.md; fill in the reads,
// writes, and control flow so the fprintfs print the right values.
// ============================================================

int connect_to_server(char *server_addr, int server_port)
{
    // TODO: create a TCP socket, connect to (server_addr, server_port),
    //       return the socket fd.
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Failed to create server socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_addr, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        close(server_fd);
        return -1;
    }

    if (connect(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return server_fd;
}

void get_item_list(int sock_fd, FILE *log_fp)
{
    int count = 0;
    msg_enum request = LIST_ITEMS;

    // DONE: send the request, read the count, then loop reading items.
    send(sock_fd, &request, sizeof(msg_enum), 0);

    read(sock_fd, &count, sizeof(msg_enum));   // This is just to consume the enum sent back
    read(sock_fd, &count, sizeof(int));   // This actually reads the count


    fprintf(log_fp, "=== Item List (%d items) ===\n", count);
    // DONE: for each item received:
    //   struct item it = ...;
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    struct item it;
    for (int i = 0; i < count; i++){
        read(sock_fd, &it, sizeof(struct item));
        fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
            it.name, it.stock, it.price);
    }
    fprintf(log_fp, "\n");
}

void search_item(int sock_fd, char *query, FILE *log_fp)
{
    int count = 0;
    // DONE
    msg_enum request = SEARCH_ITEM;
    send(sock_fd, &request, sizeof(msg_enum), 0);
    send(sock_fd, query, sizeof(char) * MAX_STR, 0);

    read(sock_fd, &count, sizeof(msg_enum));   // This is just to consume the enum sent back
    read(sock_fd, &count, sizeof(int));   // This actually reads the count

    fprintf(log_fp, "=== Search results for \"%s\" (%d matches) ===\n", query, count);
    // DONE: for each match:
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    struct item it;
    for (int i = 0; i < count; i++){
        read(sock_fd, &it, sizeof(struct item));
        fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
            it.name, it.stock, it.price);
    }
    fprintf(log_fp, "\n");
}

// BONUS (+10): like search_item, but cipher the query on the wire.
// Use encrypt_str() from utils.h; the server will call decrypt_str().
void enc_search_item(int sock_fd, char *query, FILE *log_fp)
{
    int count = 0;
    // TODO (bonus)
    encrypt_str(query);
    msg_enum request = SEARCH_ITEM;

    send(sock_fd, &request, sizeof(msg_enum), 0);
    send(sock_fd, query, sizeof(char) * MAX_STR, 0);

    read(sock_fd, &count, sizeof(msg_enum));   // This is just to consume the enum sent back
    read(sock_fd, &count, sizeof(int));   // This actually reads the count

    fprintf(log_fp, "=== Search results for \"%s\" (%d matches) ===\n", query, count);
    fprintf(log_fp, "=== Search results for \"%s\" (%d matches) ===\n", query, count);
    // DONE (bonus): for each match:
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    struct item it;
    for (int i = 0; i < count; i++){
        read(sock_fd, &it, sizeof(struct item));
        fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
            it.name, it.stock, it.price);
    }
    fprintf(log_fp, "\n");
}

void get_stock(int sock_fd, char *item_name, FILE *log_fp)
{
    int stock = 0;
    float price = 0;
    char err[MAX_STR] = {0};
    msg_enum request = GET_STOCK;

    send(sock_fd, &request, sizeof(msg_enum), 0);
    send(sock_fd, item_name, sizeof(char) * MAX_STR, 0);

    msg_enum response;
    read(sock_fd, &response, sizeof(msg_enum));
    // DONE: on success:
    if (response == STOCK_INFO){
        read(sock_fd, &stock, sizeof(int));
        read(sock_fd, &price, sizeof(float));
        fprintf(log_fp, "Stock check: %s | stock: %d | price: $%.2f\n\n",
                item_name, stock, price);
    }
    // DONE: on error:
    //   fprintf(log_fp, "Stock check error for %s: %s\n\n", item_name, err);
    else{
        read(sock_fd, err, sizeof(char) * MAX_STR);
        fprintf(log_fp, "Stock check error for %s: %s\n\n", item_name, err);
        // (void)err;   I don't know what this is supposed to do but the file had it initially?
    }
}

void buy_item(int sock_fd, char *item_name, int amount, FILE *log_fp)
{
    int new_stock = 0;
    float total_cost = 0;
    char err[MAX_STR] = {0};

    msg_enum request = BUY_ITEM;

    send(sock_fd, &request, sizeof(msg_enum), 0);
    send(sock_fd, item_name, sizeof(char) * MAX_STR, 0);
    send(sock_fd, &amount, sizeof(int), 0);

    msg_enum response;
    read(sock_fd, &response, sizeof(msg_enum));
    // DONE: on success:
    if (response == BUY_OK){
        read(sock_fd, &new_stock, sizeof(int));
        read(sock_fd, &total_cost, sizeof(float));
        fprintf(log_fp, "Bought %d x %s for $%.2f (remaining stock: %d)\n\n",
            amount, item_name, total_cost, new_stock);
    }
    else{
        read(sock_fd, err, sizeof(char) * MAX_STR);
        fprintf(log_fp, "Buy error for %s: %s\n\n", item_name, err);
    }

    // DONE: on error:
    //   fprintf(log_fp, "Buy error for %s: %s\n\n", item_name, err);
    // (void)err;
}

void sell_item(int sock_fd, char *item_name, int amount, FILE *log_fp)
{
    int new_stock = 0;
    char err[MAX_STR] = {0};
    msg_enum request = SELL_ITEM;

    send(sock_fd, &request, sizeof(msg_enum), 0);
    send(sock_fd, item_name, sizeof(char) * MAX_STR, 0);
    send(sock_fd, &amount, sizeof(int), 0);

    msg_enum response;
    read(sock_fd, &response, sizeof(msg_enum));
    // DONE : on success:
    if (response == SELL_OK){
        read(sock_fd, &new_stock, sizeof(int));
        fprintf(log_fp, "Sold %d x %s (new stock: %d)\n\n",
            amount, item_name, new_stock);
    }
    else{
        read(sock_fd, err, sizeof(char) * MAX_STR);
        fprintf(log_fp, "Sell error for %s: %s\n\n", item_name, err);
    }

    // DONE: on error:
    //   fprintf(log_fp, "Sell error for %s: %s\n\n", item_name, err);
    (void)err;
}

// Read commands from stdin, dispatch to the functions above.
// Recognized commands: LIST | SEARCH <q> | ESEARCH <q> (bonus) |
//                      STOCK <name> | BUY <name> <n> | SELL <name> <n> | QUIT
// When isatty(0) is true, print a "> " prompt before each command.
void process_input(int sock_fd)
{
    // TODO
    while(1){
        if (isatty(0)){
            fprintf(stdout, "> ");
            fflush(stdout);
        }
        char input[MAX_STR];
        fgets(input, MAX_STR, stdin);
        char command[MAX_STR];
        char arg1[MAX_STR];
        int arg2;
        sscanf(input, "%s %s %d", command, arg1, &arg2);
        if (strcmp(command, "LIST") == 0){
            get_item_list(sock_fd, stdout);
        }
        else if (strcmp(command, "SEARCH") == 0){
            search_item(sock_fd, arg1, stdout);
        }
        else if (strcmp(command, "ESEARCH") == 0){
            enc_search_item(sock_fd, arg1, stdout);
        }
        else if (strcmp(command, "STOCK") == 0){
            get_stock(sock_fd, arg1, stdout);
        }
        else if (strcmp(command, "BUY") == 0){
            buy_item(sock_fd, arg1, arg2, stdout);
        }
        else if (strcmp(command, "SELL") == 0){
            sell_item(sock_fd, arg1, arg2, stdout);
        }
        if (strcmp(command, "QUIT") == 0){
            fprintf(stdout, "Quitting program. Goodbye!\n");
            return;
        }
        else {
            fprintf(stdout, "Unknown command\n");
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3){
        printSyntax();
        return 1;
    }

    int sock_fd = connect_to_server(argv[1], atoi(argv[2]));
    process_input(sock_fd);
    close(sock_fd);
    // TODO:
    //   1. check argc == 3, call printSyntax() on error
    //   2. parse server_addr and server_port
    //   3. sock_fd = connect_to_server(...)
    //   4. process_input(?)
    //   5. close(?)
    return 0;
}
