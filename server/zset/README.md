# ZSet - Sorted Set

A Redis sorted set is a collection of unique strings (members) ordered by an associated score. When more than one string has the same score, the strings are ordered lexicographically. Some use cases for sorted sets include:

## Use Cases

- Leaderboards - we can use sorted sets to easily maintain ordered lists of the highest scores in a massive online game.
- Rate limiters - we can use a sorted set to build a sliding-window rate limiter to prevent excessive API requests.

## Available Commands

- `zadd`
- `zrem`
- `zscore`
- `zquery`

> ZNode definition
```cpp
struct ZNode
{
    AVLNode tree;
    HNode hmap;
    double score = 0;
    size_t len = 0;
    char name[0]; // struct hack
    // INFO: https://www.geeksforgeeks.org/struct-hack/
};
```

## Testing ZSet (Sorted Set)

The leaderboard contains three entries: Alice, Bob, and Jon. Each entry has an associated score, and we use ZSet's features to manage and query this data, such as adding entries, retrieving scores, and performing various leaderboard-related operations.

![Screenshot of commands executed](https://github.com/nkilm/exploring-redis/assets/79012023/5906d065-6df9-4307-bad0-7158a138b150)


## References
Official docs - [Sorted Set in Redis](https://redis.io/docs/data-types/sorted-sets/)
