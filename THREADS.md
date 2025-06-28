# A Guide to Multi-threading: From Zero to Thread-Safe Hash Table

Hey! So you've got this C hash table project, and we're making it "multi-threaded." What does that even mean? And why does it suddenly get more complicated?

This guide will walk you through it all, step by step, using a simple analogy: **running a kitchen.**

## Part 1: What is a Thread? (A Single Chef)

Imagine a computer program is a kitchen.

A traditional, single-threaded program is like having **one chef** in the kitchen. This chef has to do *everything* in order:
1.  Take an order from a customer.
2.  Go to the fridge to get ingredients.
3.  Chop the vegetables.
4.  Cook the meal.
5.  Plate the food.
6.  Wash the dirty dishes.
7.  ...and only then can they take the next order.

This chef is a **thread**. A thread is just a single, independent path of instructions that a program follows. One chef, one set of instructions, one task at a time.

## Part 2: Multi-threading (A Kitchen with Many Chefs)

The one-chef kitchen is slow. To speed things up, you hire more chefs. This is **multi-threading**.

Now you have four chefs (four threads) in your kitchen. They can all work at the same time on different tasks.
*   Chef 1 can be taking an order.
*   Chef 2 can be chopping vegetables for another order.
*   Chef 3 can be cooking a meal.
*   Chef 4 can be washing dishes.

The kitchen gets way more work done in the same amount of time! This is the goal of multi-threading: **to perform multiple operations concurrently (at the same time) to improve performance.**

In our project, instead of chefs, our threads are all trying to `insert` and `search` the hash table at the same time.

## Part 3: The Big Problem! (The Shared Spice Rack)

Okay, so you have four chefs. Things are great, until Chef 1 and Chef 2 need to use the **one and only jar of salt** at the exact same moment.

What happens?
*   Chef 1 grabs the salt.
*   Chef 2 tries to grab the salt, but it's gone. He has to wait.
*   Or worse, they both try to update something shared, and create a mess.

Let's imagine they share a notebook to keep track of how many potatoes are left.
1.  The notebook says **"10 potatoes left"**.
2.  Chef 1 reads the notebook. "Okay, 10 potatoes." He takes one.
3.  **Before Chef 1 can write "9" in the notebook**, Chef 2 comes along and also reads it. It still says **"10 potatoes left"**. He also takes one.
4.  Chef 1 finally writes in the notebook: "9 potatoes left".
5.  Chef 2 then writes in the notebook: "9 potatoes left".

**This is a disaster!** Two potatoes were used, but the notebook says only one was. The count is now wrong.

This is called a **Race Condition**. It happens when multiple threads try to access and modify shared data (the notebook, the salt, our hash table's `count` and `items` array) at the same time, leading to corrupted data and unpredictable results.

## Part 4: The Solution - The Mutex (The "Kitchen Key")

To solve the chaos, you introduce a simple rule: **To use a shared resource, you must hold the "Kitchen Key". There is only one key.**

A **Mutex** (which stands for **Mut**ual **Ex**clusion) is exactly this: a lock.

Here's how it works:
1.  Chef 1 wants to use the shared notebook. He goes to the key hook and **locks** it by taking the key.
2.  Now that he has the key, he is free to read and write in the notebook.
3.  While Chef 1 has the key, Chef 2 also wants to use the notebook. He goes to the key hook, but the key is gone. He **must wait**. He is blocked.
4.  Chef 1 finishes his work, goes back to the hook, and **unlocks** by putting the key back.
5.  Now, Chef 2 (who was waiting) can take the key and do his work safely.

This system guarantees that only one thread can access the shared resource at any given time, preventing race conditions.

### How Threads Use a Mutex in C

In our code (`pthread` library), this looks like:

*   **Create the key:** In `ht_new()`, we initialize the mutex with `pthread_mutex_init(&ht->lock, NULL);`.
*   **Lock (Take the key):** Before touching the hash table, a thread calls `pthread_mutex_lock(&ht->lock);`.
*   **Unlock (Put the key back):** After it's done, it MUST call `pthread_mutex_unlock(&ht->lock);`.

The code that is protected between the `lock` and `unlock` calls is called a **Critical Section**.

```c
// A simplified look at our ht_insert function

void ht_insert(ht_hash_table* ht, const char* key, const char* value) {

    // 1. Chef wants to use the hash table. He takes the "Kitchen Key".
    pthread_mutex_lock(&ht->lock);

    // --- CRITICAL SECTION START ---
    // He is now in the "critical section". No other thread can enter.
    // It's safe to modify the shared hash table here.
    // (Check load, resize, add the new item, update ht->count...)
    // --- CRITICAL SECTION END ---

    // 2. Chef is done. He puts the key back for the next person.
    pthread_mutex_unlock(&ht->lock);
}
```

**The Golden Rule:** Any function that reads from or writes to a shared resource (our `ht_hash_table`) must lock the mutex before touching it and unlock it right after. If you forget to unlock, it's like a chef taking the key home—no one else can ever get in, and your whole program freezes. This is called a **deadlock**.

And that's it! You now understand the fundamental concepts that make our hash table thread-safe.

- More [C++ Multi-Threading](https://medium.com/nerd-for-tech/hands-on-multithreading-with-c-01-overview-e29087ebeadb)
---