#include "server.h"

void printSyntax(){
    printf("incorrect usage syntax!\n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

// ============================================================
// load_inventory: read items.csv into the global inventory[] array
// CSV format:
//     name,stock,price
//     laptop,50,999.99
//     ...
// Skip the header line. Set num_items to the number of items loaded.
// ============================================================
void load_inventory(char *filename)
{
    // TODO: open the file, skip the header, read each line into
    //       inventory[num_items], increment num_items.
}

// ============================================================
// handle_list_items: send the full inventory to the client.
// Protocol:
//     write ITEM_LIST (msg_enum)
//     write num_items (int)
//     write each struct item (num_items of them)
// Remember to lock the inventory_lock mutex while reading inventory.
// ============================================================
void handle_list_items(int client_fd)
{
    msg_enum rsp = ITEM_LIST;
    write(client_fd, &rsp, sizeof(msg_enum));

    pthread_mutex_lock(&inventory_lock);
    write(client_fd, &num_items, sizeof(int));
    int i = 0;
    while (i < num_items)
    {
        write(client_fd, &inventory[i], sizeof(struct item));
        i++;
    }
    pthread_mutex_unlock(&inventory_lock);
}

// ============================================================
// handle_search: read a query string from the client, then return
// every item whose name contains the query as a substring.
// Protocol (recv): char query[MAX_STR]
// Protocol (send): SEARCH_RESULTS (msg_enum), count (int),
//                  then count x struct item
// ============================================================
void handle_search(int client_fd)
{
    // TODO: read the query, scan inventory, build a results array,
    //       send the count and each matching item back.
}

// ============================================================
// BONUS (optional, +10 pts):
// handle_enc_search: like handle_search, but the query was ciphered on
// the wire using the Caesar shift from utils.h. Call decrypt_str() on
// it before running the normal substring search. Reply format is the
// standard SEARCH_RESULTS message (unciphered).
// ============================================================
void handle_enc_search(int client_fd)
{
    // TODO (bonus): read char query[MAX_STR]; decrypt_str(query);
    //               then run the same search loop as handle_search,
    //               and write SEARCH_RESULTS + count + items back.
}

// ============================================================
// handle_get_stock: read an item name and respond with its stock/price.
// Protocol (recv): char name[MAX_STR]
// Protocol (send): STOCK_INFO (msg_enum), stock (int), price (float)
//             or:  ERROR_MSG (msg_enum), char err[MAX_STR]
// ============================================================
void handle_get_stock(int client_fd)
{
    // TODO
}

// ============================================================
// handle_buy_item: read item name and amount, decrement stock.
// Protocol (recv): char name[MAX_STR], int amount
// Protocol (send): BUY_OK (msg_enum), new_stock (int), total_cost (float)
//             or:  ERROR_MSG (msg_enum), char err[MAX_STR]
// Send an error if the item doesn't exist OR if stock < amount.
// ============================================================
void handle_buy_item(int client_fd)
{
    // TODO
}

// ============================================================
// handle_sell_item: read item name and amount, increment stock.
// Protocol (recv): char name[MAX_STR], int amount
// Protocol (send): SELL_OK (msg_enum), new_stock (int)
//             or:  ERROR_MSG (msg_enum), char err[MAX_STR]
// ============================================================
void handle_sell_item(int client_fd)
{
    // TODO
}

// ============================================================
// save_inventory: write the current inventory[] to output/inventory.csv
// Format should match items.csv (header: "name,stock,price").
// ============================================================
void save_inventory()
{
    // TODO
}

// ============================================================
// handle_client: the worker thread function for one client connection.
// Loop reading a msg_enum from the socket and dispatching to the right
// handler. Break out of the loop and close the socket when the client
// disconnects (read returns <= 0).
// ============================================================
void *handle_client(void *arg)
{
    // TODO: extract client_fd from arg, free(arg), loop reading
    //       msg_type and calling handle_* functions. Remember to
    //       dispatch ENC_SEARCH_ITEM to handle_enc_search (bonus).
    return NULL;
}

// ============================================================
// sigterm_handler: on SIGTERM, save inventory and exit(0).
// ============================================================
void sigterm_handler(int sig)
{
    // TODO
}

int main(int argc, char *argv[])
{
    // TODO:
    //   1. check argc, call printSyntax() on error
    //   2. parse server_addr, server_port (and num_workers if you use it)
    //   3. call bookeepingCode() to set up output/
    //   4. load_inventory("items.csv")
    //   5. signal(SIGTERM, sigterm_handler)
    //   6. create a TCP socket, bind, listen
    //   7. accept loop: for each client, malloc an int*, store the fd,
    //      pthread_create(handle_client, ...), pthread_detach(...)
    return 0;
}
