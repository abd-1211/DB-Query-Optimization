# RAII Datatypes

> **RAII (Resource Acquisition Is Initialization)** — a C++ idiom where resources are acquired in a constructor and released in the destructor, guaranteeing no resource leaks and full exception safety.

---

## Smart Pointers — Dynamic Memory Management

| Type | Description |
|------|-------------|
| `std::unique_ptr` | Sole ownership of a heap resource; frees memory when it goes out of scope |
| `std::shared_ptr` | Shared ownership via reference counting; frees memory when the last owner is destroyed |

---

## STL Containers — Heap Storage Management

| Type | Description |
|------|-------------|
| `std::vector` | Dynamic array; manages its heap buffer automatically |
| `std::string` | Dynamic character buffer; cleans up on destruction |
| `std::map` | Sorted key-value store; manages its tree nodes automatically |
| `std::set` | Sorted unique-element collection; manages its tree nodes automatically |
| `std::list` | Doubly linked list; manages its nodes automatically |

---

## Concurrency & Locking Utilities

| Type | Description |
|------|-------------|
| `std::lock_guard` | Strict scope-based lock — locks on construction, unlocks on destruction |
| `std::unique_lock` | Scope-based lock with support for manual unlocking/re-locking |
| `std::scoped_lock` | Deadlock-safe wrapper for locking **multiple mutexes simultaneously** |
| `std::jthread` | Automatically signals a stop request and joins the thread on destruction |

---

## Custom RAII Classes

When no standard wrapper fits, write your own:

```cpp
class MyResource {
public:
    MyResource()  { /* acquire resource */ }
    ~MyResource() { /* release resource */ }

    // Non-copyable, movable only
    MyResource(const MyResource&)            = delete;
    MyResource& operator=(const MyResource&) = delete;
    MyResource(MyResource&&)                 = default;
    MyResource& operator=(MyResource&&)      = default;
};
```

**Pattern:** resource is acquired in the constructor and cleaned up in the destructor — no manual `free`/`close`/`unlock` calls needed.

---

## Why RAII?

- **Eliminates resource leaks** — the destructor always runs when scope exits, even via exception
- **Guarantees exception safety** — no `try/finally` boilerplate required
- **Makes ownership explicit** — the object's lifetime *is* the resource's lifetime
