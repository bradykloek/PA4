#include "server.h"

void printSyntax()
{
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
    // open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return;
    }
    char buffer[MAX_STR];
    // skip the header
    if (fgets(buffer, sizeof(buffer), file) == NULL)
    {
        fclose(file);
        return;
    }
    // may need to change the format specifiers I always forget these lol
    // read each line into inventory[num_items] and increment num_items.
    while (fscanf(file, "%s, %d, %.2f", inventory[num_items].name, &inventory[num_items], &inventory[num_items]) == 3)
    {
        num_items++;
        if (num_items >= MAX_ITEMS)
        {
            fprintf(stderr, "buffer overflow risk, reading aborted");
            break;
        }
    }

    fclose(file);
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
    char query[MAX_STR];
    // read the query
    read(client_fd, query, MAX_STR);

    struct item results[MAX_RESULTS];
    int count = 0;

    // critical section: scan inventory
    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        if (strcmp(inventory[i].name, query) == 0)
        {
            // build results array
            results[count++] = inventory[i];
        }
    }
    pthread_mutex_unlock(&inventory_lock);

    // send the count and each matching item back
    msg_enum rsp = SEARCH_RESULTS;
    write(client_fd, &rsp, sizeof(msg_enum));
    write(client_fd, &count, sizeof(int));
    write(client_fd, results, count * sizeof(struct item));
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
    char query[MAX_STR];
    // read the query
    read(client_fd, query, MAX_STR);

    // call decrypt
    decrypt_str(query);

    struct item results[MAX_RESULTS];
    int count = 0;

    // critical section: scan inventory
    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        if (strcmp(inventory[i].name, query) == 0)
        {
            // build results array
            results[count++] = inventory[i];
        }
    }
    pthread_mutex_unlock(&inventory_lock);

    // send the count and each matching item back
    msg_enum rsp = SEARCH_RESULTS;
    write(client_fd, &rsp, sizeof(msg_enum));
    write(client_fd, &count, sizeof(int));
    write(client_fd, results, count * sizeof(struct item));
}

// ============================================================
// handle_get_stock: read an item name and respond with its stock/price.
// Protocol (recv): char name[MAX_STR]
// Protocol (send): STOCK_INFO (msg_enum), stock (int), price (float)
//             or:  ERROR_MSG (msg_enum), char err[MAX_STR]
// ============================================================
void handle_get_stock(int client_fd)
{
    char name[MAX_STR];
    read(client_fd, name, MAX_STR);

    int found = 0;
    int stock;
    float price;

    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        if (strcmp(inventory[i].name, name) == 0)
        {
            found = 1;
            stock = inventory[i].stock;
            price = inventory[i].price;
            break;
        }
    }
    pthread_mutex_unlock(&inventory_lock);

    if (found == 1)
    {
        msg_enum rsp = STOCK_INFO;

        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, &stock, sizeof(int));
        write(client_fd, &price, sizeof(float));
    }
    else
    {
        msg_enum rsp = ERROR_MSG;
        char err[MAX_STR] = "Stock check error for nothing: item not found";
        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, err, MAX_STR);
    }
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
    char name[MAX_STR];
    int amount;

    read(client_fd, name, MAX_STR);
    read(client_fd, amount, sizeof(int));

    float stock;
    int idx = -1;

    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        if (strcmp(inventory[i].name, name) == 0)
        {
            stock = inventory[i].stock;
            idx = i;
            break;
        }
    }
    if (idx != -1 && stock >= amount)
    {
        inventory[idx].stock -= amount;
        int new_stock = inventory[idx].stock;
        float total_cost = inventory[idx].price * amount;
        pthread_mutex_unlock(&inventory_lock);

        msg_enum rsp = BUY_OK;
        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, &new_stock, sizeof(int));
        write(client_fd, &total_cost, sizeof(float));
    }
    else
    {
        pthread_mutex_unlock(&inventory_lock);

        msg_enum rsp = ERROR_MSG;
        char err[MAX_STR];
        if (idx == -1)
        {
            // stock not found
            *err = "Stock check error for nothing: item not found";
        }
        else
        {
            // insufficient funds
            *err = "Buy error for monitor: not enough stock";
        }
        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, err, MAX_STR);
    }
}

// ============================================================
// handle_sell_item: read item name and amount, increment stock.
// Protocol (recv): char name[MAX_STR], int amount
// Protocol (send): SELL_OK (msg_enum), new_stock (int)
//             or:  ERROR_MSG (msg_enum), char err[MAX_STR]
// ============================================================
void handle_sell_item(int client_fd)
{
    char name[MAX_STR];
    int amount;

    read(client_fd, name, MAX_STR);
    read(client_fd, amount, sizeof(int));

    float stock;
    int idx = -1;

    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        if (strcmp(inventory[i].name, name) == 0)
        {
            stock = inventory[i].stock;
            idx = i;
            break;
        }
    }
    if (idx != -1)
    {
        inventory[idx].stock += amount;
        int new_stock = inventory[idx].stock;
        pthread_mutex_unlock(&inventory_lock);

        msg_enum rsp = SELL_OK;
        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, &new_stock, sizeof(int));
    }
    else
    {
        pthread_mutex_unlock(&inventory_lock);

        msg_enum rsp = ERROR_MSG;
        char err[MAX_STR] = "Stock check error for nothing: item not found";
        write(client_fd, &rsp, sizeof(msg_enum));
        write(client_fd, err, MAX_STR);
    }
}

// ============================================================
// save_inventory: write the current inventory[] to output/inventory.csv
// Format should match items.csv (header: "name,stock,price").
// ============================================================
void save_inventory()
{
    FILE *file = fopen("output/inventory.csv", "w");
    if (file == NULL)
    {
        fprintf(stderr, "failed to create output file\n");
        return;
    }

    // write header
    fprintf(file, "name,stock,price\n");

    pthread_mutex_lock(&inventory_lock);
    for (int i = 0; i < num_items; i++)
    {
        fprintf(file, "%s,%d,%.2f\n",
                inventory[i].name,
                inventory[i].stock,
                inventory[i].price);
    }
    pthread_mutex_unlock(&inventory_lock);

    fclose(file);
}

// ============================================================
// handle_client: the worker thread function for one client connection.
// Loop reading a msg_enum from the socket and dispatching to the right
// handler. Break out of the loop and close the socket when the client
// disconnects (read returns <= 0).
// ============================================================
void *handle_client(void *arg)
{
    // extract client_fd from arg, free(arg)
    int client_fd = *((int *)arg);
    free(arg);

    msg_enum msg_type;

    // loop reading msg_type and calling handle_* functions
    while (read(client_fd, msg_type, sizeof(msg_type) > 0))
    {
        switch (msg_type)
        {
        case LIST_ITEMS:
            handle_list_items(client_fd);
            break;
        case SEARCH_ITEM:
            handle_search(client_fd);
            break;
        case GET_STOCK:
            handle_get_stock(client_fd);
            break;
        case BUY_ITEM:
            handle_buy_item(client_fd);
            break;
        case SELL_ITEM:
            handle_sell_item(client_fd);
            break;
        // extra credit
        case ENC_SEARCH_ITEM:
            handle_enc_search(client_fd);
            break;
        default:
            fprintf(stderr, "Unknown message type: %d\n", msg_type);
            break;
        }
    }
    close(client_fd);
    return NULL;
}

// ============================================================
// sigterm_handler: on SIGTERM, save inventory and exit(0).
// ============================================================
void sigterm_handler(int sig)
{
    save_inventory();
    exit(0);
}

int main(int argc, char *argv[])
{
    // check argc, call printSyntax() on error
    if (argc < 4)
    {
        printSyntax();
    }

    // parse server_addr, server_port (and num_workers if you use it)
    char *server_addr = argv[1];
    int server_port = atoi(argv[2]);
    int num_workers = atoi(argv[3]);

    // call bookeepingCode() to set up output/
    bookeepingCode();

    // call load_inventory("items.csv")
    load_inventory("items.csv");

    // call signal(SIGTERM, sigterm_handler)
    signam(SIGTERM, sigterm_handler);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);

    // Convert IP string to binary format
    address.sin_addr.s_addr = inet_addr(server_addr);

    if (address.sin_addr.s_addr == INADDR_NONE)
    {
        fprintf(stderr, "Invalid IP address\n");
    }

    // create a TCP socket, bind, listen
    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // TODO: change 3 to however many clients were allowed to have running
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //  accept loop: for each client, malloc an int* & store the fd,
    while (1)
    {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("accept");
            continue; // server keeps running even if one fails
        }

        int *client_fd = malloc(sizeof(int));
        if (client_fd == NULL)
        {
            perror("client malloc failed");
            free(client_fd);
            close(new_socket);
        }
        *client_fd = new_socket;
        pthread_t thread_id;

        //  pthread_create(handle_client, ...), pthread_detach(...)
        if (pthread_create(handle_client, NULL, &thread_id, client_fd) != 0)
        {
            perror("pthread_create failed");
            free(client_fd);
            close(new_socket);
        }
        else
        {
            pthread_detach(thread_id);
        }
    }

    close(server_fd);
    return 0;
}
