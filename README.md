# 🧾 Database Project – Development Log

This document outlines the development milestones of a basic SQLite-style database engine implemented in C.

---

## ✅ Milestones Completed

### 1. REPL Initialization

* Implemented a minimal Read-Eval-Print Loop (REPL).
* Supported meta-commands (starting with `.`).
* `.exit` command allows users to gracefully close the database.

---

### 2. Insert and Select Functionality

* Added support for:

  * `insert` – insert a new row into the table.
  * `select` – display all stored rows.
* Defined a `Row` struct to model data entries.
* Introduced a basic in-memory `Table` to hold rows linearly.

---

### 3. Test Coverage

* Created a test suite to verify:

  * Command parsing
  * Insertion and retrieval logic
* Prevents regressions during further development.

---

### 4. Persistence with Pager

* Introduced a `Pager` system to back table data with a file.
* Pages are fixed-size (e.g., 4096 bytes), and the pager:

  * Loads pages on demand from disk (lazy loading).
  * Writes pages back to disk with `pager_flush`.
* Database now supports persistent data storage across runs.

---

### 5. Table Structure with Pager

* `Table` struct now contains a `Pager` instance.
* The `Pager` manages:

  * File descriptor
  * File length
  * Page cache (array of pointers)
* Acts as the interface between in-memory rows and on-disk pages.

---

### 6. Cursor for Row Access

* Added a `Cursor` abstraction to handle:

  * Sequential and random row access
  * Read/Write navigation within pages
* Cursors enable structured CRUD operations on table rows.

---

## 🧠 Concepts Implemented

* Page-based memory management
* Serialization and deserialization of row data
* File I/O using `open`, `read`, `write`, `lseek`
* Manual memory management with `malloc`, `free`
* Simple REPL architecture

---

## 🛠️ Next Steps (Planned)

> *(Optional section, include only if helpful)*

* Row deletion and reuse of empty slots
* B-tree-based page layout
* Type checking and constraints
* SQL parsing improvements
* Better error messages and edge case handling
