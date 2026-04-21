CC = gcc
CFLAGS = -g
CLINK = -pthread

SRCDIR=src
INCLDIR=include
LIBDIR=lib
SOLDIR=solution_bin

LOCALADDR=127.0.0.1
PORT=42069

# ------------------------------------------------------------
# Build everything (your code)
# ------------------------------------------------------------
all: client server

client: $(SRCDIR)/client.c $(LIBDIR)/utils.o
	$(CC) $(CLINK) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/client.c -o client

server: $(SRCDIR)/server.c $(LIBDIR)/utils.o
	$(CC) $(CLINK) $(CFLAGS) -I$(INCLDIR) $(LIBDIR)/utils.o $(SRCDIR)/server.c -o server

$(LIBDIR)/utils.o: $(SRCDIR)/utils.c | $(LIBDIR)
	$(CC) $(CLINK) $(CFLAGS) -I$(INCLDIR) -c $(SRCDIR)/utils.c -o $(LIBDIR)/utils.o

$(LIBDIR):
	mkdir -p $(LIBDIR)

# ------------------------------------------------------------
# End-to-end tests — uses YOUR client and server.
# Starts the server in the background, runs the client against an
# input file (stdin redirected with <, stdout redirected with >),
# then SIGTERMs the server so it writes output/inventory.csv.
# ------------------------------------------------------------
run1: all
	@./server $(LOCALADDR) $(PORT) 1 & \
	 SRV=$$!; sleep 2; \
	 ./client $(LOCALADDR) $(PORT) < input1.txt > output/input1.txt.log; \
	 kill -TERM $$SRV; wait $$SRV 2>/dev/null || true

run2: all
	@./server $(LOCALADDR) $(PORT) 1 & \
	 SRV=$$!; sleep 2; \
	 ./client $(LOCALADDR) $(PORT) < input2.txt > output/input2.txt.log; \
	 kill -TERM $$SRV; wait $$SRV 2>/dev/null || true

t1: run1
	diff expected/inventory1.csv output/inventory.csv
	@echo passed

t2: run2
	diff expected/inventory2.csv output/inventory.csv
	@echo passed

# ------------------------------------------------------------
# Component-only tests: lets you test ONE file at a time by using
# the reference binary (in solution_bin/) for the other side.
#
# Useful when you're stuck on one piece — you can test your
# client against our working server, and vice versa.
# ------------------------------------------------------------

# test only your client.c (uses reference server)
testclientonly: client
	@cp $(SOLDIR)/server ./server
	@./server $(LOCALADDR) $(PORT) 1 & \
	 SRV=$$!; sleep 2; \
	 ./client $(LOCALADDR) $(PORT) < input1.txt > output/input1.txt.log; \
	 kill -TERM $$SRV; wait $$SRV 2>/dev/null || true
	@diff expected/inventory1.csv output/inventory.csv && echo "CLIENT PASSED" || (echo "CLIENT FAILED"; rm -f ./server; exit 1)
	@rm -f ./server

# test only your server.c (uses reference client)
testserveronly: server
	@cp $(SOLDIR)/client ./client
	@./server $(LOCALADDR) $(PORT) 1 & \
	 SRV=$$!; sleep 2; \
	 ./client $(LOCALADDR) $(PORT) < input1.txt > output/input1.txt.log; \
	 kill -TERM $$SRV; wait $$SRV 2>/dev/null || true
	@diff expected/inventory1.csv output/inventory.csv && echo "SERVER PASSED" || (echo "SERVER FAILED"; rm -f ./client; exit 1)
	@rm -f ./client

.PHONY: all clean run1 run2 t1 t2 testclientonly testserveronly zip

# ------------------------------------------------------------
# Package your submission for Gradescope.
# Produces submission.zip containing client.c, server.c, utils.c
# at the top level. The autograder accepts both layouts
# (top-level or inside src/), so either works.
# ------------------------------------------------------------
zip:
	@rm -f submission.zip
	@cd $(SRCDIR) && zip -q ../submission.zip client.c server.c utils.c
	@echo "created submission.zip — upload to Gradescope"
	@unzip -l submission.zip

clean:
	rm -f server client $(LIBDIR)/utils.o submission.zip
	rm -rf output
