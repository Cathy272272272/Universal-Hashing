# Universal-Hashing
I implement a universal family of hash function to achieve better performance in an adversarial environment if someone knows what hash function you use in advance. For instance, if someone who knows you are using a specific hash function on a set table size might construct a pathological set of keys that they know will all hash to the same value, so that the hash table only fills a single bin and lookup performance decays from O(1) to O(n) time. 

I provide two ways to implement universal hashing.

1.Using a fixed hashing vector.
The fixed vector is dependent on a specific hashing function, and the vector is updated only if the hash table touches its rehashing conditon.

2.Without a fixed hashing vector.
This could be achieved by fixing a hashing seed, and calculate the hashing vector in place each time when hashing. When the hash table touches its rehashing condition, the hashing seed is updated.

