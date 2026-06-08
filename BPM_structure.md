# Buffer Pool Manager — Architecture

> The **Buffer Pool Manager (BPM)** keeps pages from disk in RAM. It is the coordinator between the rest of the database and disk I/O — every other component listed here is a helper tool that the BPM orchestrates.

---

## Component Overview

```
┌──────────────────────────────────────────────────────┐
│                  BufferPoolManager                   │
│  (takes a page_id → finds/loads frame → returns guard)│
│                                                      │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │
│  │FrameHeader[]│  │ ARCReplacer │  │DiskScheduler│  │
│  │  (RAM slots)│  │  (eviction) │  │  (disk I/O) │  │
│  └─────────────┘  └─────────────┘  └─────────────┘  │
│                                                      │
│  ┌─────────────────────┐  ┌──────────────────────┐   │
│  │     BPM_latch       │  │ Write/ReadPageGuard  │   │
│  │ (global bookkeeping)│  │  (RAII frame locks)  │   │
│  └─────────────────────┘  └──────────────────────┘   │
└──────────────────────────────────────────────────────┘
```

---

## Components

### 1. `FrameHeader`

Each RAM slot that holds a page from disk.

| Field | Purpose |
|-------|---------|
| `frame_id` | Identifies which slot this is |
| `rwlatch` | Read/write mutex protecting the frame's data |

- Allows **multiple concurrent readers**
- Only **one writer** at a time

---

### 2. `ARCReplacer`

Decides which frame to evict when all slots are full and a new page must be loaded. Implements the **ARC (Adaptive Replacement Cache)** policy.

| Method | Description |
|--------|-------------|
| `Evict()` | Returns the frame to evict according to policy |
| `RecordAccess()` | Records a frame access so the replacer can update bookkeeping |
| `SetEvictable()` | Marks a frame as evictable or pinned |
| `Remove()` | Removes an evictable frame from the replacer |
| `Size()` | Returns the number of currently evictable frames |

---

### 3. `DiskScheduler`

Handles all asynchronous reads and writes between RAM frames and disk.

- **Reads** pages from disk into a frame
- **Writes** dirty pages back to disk

The BPM contacts the DiskScheduler when a requested page is not already in memory.

---

### 4. `BPM_latch`

A single global mutex that protects the BPM's internal bookkeeping structures (page table, free list, etc.).

> Distinct from per-frame latches — this guards the BPM's *metadata*, not page *data*.

---

### 5. `WritePageGuard` / `ReadPageGuard`

RAII wrappers around frame latches so you **cannot forget to unlock a frame**.

- Acquires the appropriate latch (`rwlatch`) on construction
- Releases it automatically on destruction (scope exit, exception, or return)

```cpp
{
    auto guard = bpm->FetchPageWrite(page_id);  // latch acquired
    // ... modify page data via guard ...
}   // latch released automatically — no manual Unpin/Unlock needed
```

---

### 6. `BufferPoolManager` (BPM)

The top-level coordinator. Given a `page_id`, it:

1. Checks if the page is already in a frame
2. If not, asks `ARCReplacer` for a frame to evict and calls `DiskScheduler` to load the page
3. Pins the frame (marks it non-evictable while in use)
4. Returns a `ReadPageGuard` or `WritePageGuard` to the caller

All other components exist solely to support the BPM.
