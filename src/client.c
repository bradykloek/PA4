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
    return -1;
}

void get_item_list(int sock_fd, FILE *log_fp)
{
    int count = 0;
    // TODO: send the request, read the count, then loop reading items.
    fprintf(log_fp, "=== Item List (%d items) ===\n", count);
    // TODO: for each item received:
    //   struct item it = ...;
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    fprintf(log_fp, "\n");
}

void search_item(int sock_fd, char *query, FILE *log_fp)
{
    int count = 0;
    // TODO
    fprintf(log_fp, "=== Search results for \"%s\" (%d matches) ===\n", query, count);
    // TODO: for each match:
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    fprintf(log_fp, "\n");
}

// BONUS (+10): like search_item, but cipher the query on the wire.
// Use encrypt_str() from utils.h; the server will call decrypt_str().
void enc_search_item(int sock_fd, char *query, FILE *log_fp)
{
    int count = 0;
    // TODO (bonus)
    fprintf(log_fp, "=== Search results for \"%s\" (%d matches) ===\n", query, count);
    // TODO (bonus): for each match:
    //   fprintf(log_fp, "  %s | stock: %d | price: $%.2f\n",
    //           it.name, it.stock, it.price);
    fprintf(log_fp, "\n");
}

void get_stock(int sock_fd, char *item_name, FILE *log_fp)
{
    int stock = 0;
    float price = 0;
    char err[MAX_STR] = {0};
    // TODO: on success:
    fprintf(log_fp, "Stock check: %s | stock: %d | price: $%.2f\n\n",
            item_name, stock, price);
    // TODO: on error:
    //   fprintf(log_fp, "Stock check error for %s: %s\n\n", item_name, err);
    (void)err;
}

void buy_item(int sock_fd, char *item_name, int amount, FILE *log_fp)
{
    int new_stock = 0;
    float total_cost = 0;
    char err[MAX_STR] = {0};
    // TODO: on success:
    fprintf(log_fp, "Bought %d x %s for $%.2f (remaining stock: %d)\n\n",
            amount, item_name, total_cost, new_stock);
    // TODO: on error:
    //   fprintf(log_fp, "Buy error for %s: %s\n\n", item_name, err);
    (void)err;
}

void sell_item(int sock_fd, char *item_name, int amount, FILE *log_fp)
{
    int new_stock = 0;
    char err[MAX_STR] = {0};
    // TODO: on success:
    fprintf(log_fp, "Sold %d x %s (new stock: %d)\n\n",
            amount, item_name, new_stock);
    // TODO: on error:
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
}

int main(int argc, char *argv[])
{
    // TODO:
    //   1. check argc == 3, call printSyntax() on error
    //   2. parse server_addr and server_port
    //   3. sock_fd = connect_to_server(...)
    //   4. process_input(?)
    //   5. close(?)
    return 0;
}
