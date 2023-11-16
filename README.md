# Exploring Redis
![exploring-redis](https://github.com/nkilm/exploring-redis/assets/79012023/d6577c95-16a6-4c6b-bc98-8130830e98f8)

Redis is an in-memory datastore which is an acronym for **Remote Dictionary Server**. It is used as a database, cache, streaming engine, and message broker.

Official docs - [https://redis.io/](https://redis.io/)

## Features
Explore different branches to track the code's evolution, with the _main_ branch containing the final code. Key implemented features include:

- Custom Protocol (TLV - Type-Length-Value)
- Event Loop and Non-Blocking IO - using `poll()` 
- Efficient Hashtables with Chaining and Progressive Resizing
- Data Serialization
- AVL Trees - implement `zset`
- Timers for poll() using a doubly-linked list
- Min-Heap for Time-To-Live (TTL) tracking
- Multi-threading for handling resource-intensive operations like zset deletions

## Commands

> General Key-Value Commands

- `keys` - Lists all keys along with their corresponding values.
- `get` - Retrieves the value associated with a given key.
- `set` - Sets a key to hold a specified string value.
- `del` - Deletes a specified key and its associated value.
- `pexpire` - Sets a time-to-live (TTL) for a key in milliseconds.
- `pttl` - Returns the remaining time to live of a key with a TTL, in milliseconds.

> ZSet (Sorted Set) Commands

For more info, check [server/zset/README.md](https://github.com/nkilm/exploring-redis/blob/main/server/zset/README.md)

- `zadd` - Adds one or more members to a sorted set with associated scores.
- `zrem` - Removes one or more members from a sorted set.
- `zscore` - Returns the score of a member in a sorted set.
- `zquery` - Performs queries and operations on sorted sets.

## Running Locally

### Prerequisites
Before running the project, ensure you have the following tools installed:

- [g++](https://gcc.gnu.org/) or [clang](https://clang.llvm.org/) (for compilation)
- [Make](https://www.gnu.org/software/make/) (for using Makefiles)

```bash
sudo apt install make g++
```

### Compilation

```bash
cd server
make
```

```bash
# new terminal
cd client
make
```
After the compilation step, you can run the server and client as needed.

> To remove the executables, type `make clean`

## References
[build-your-own.org/redis](https://build-your-own.org/redis/)
