# MEM_SCAN

A Cheat Engine-style Windows process memory scanner built from scratch in pure C using the Win32 API.

---

## What It Does

MEM_SCAN lets you enumerate running processes, attach to a target, and scan its virtual address space for values you specify. Found matches can be narrowed down through successive scans, letting you pinpoint the exact memory address of any value in a live process.

Core functionality:
- **Process enumeration** via `CreateToolhelp32Snapshot` with a selectable list UI
- **Address space walking** using `NtQueryVirtualMemory` / `VirtualQueryEx` to enumerate readable regions
- **Memory reading** via `ZwReadVirtualMemory` (resolved at runtime through `GetProcAddress`)
- **First scan** — search all readable regions for a target value
- **Next scan** — filter the previous match list against a new value
- **Threaded scanning** — scan runs on a dedicated worker thread to keep the UI responsive and avoiding blocking
- **ListView results** with custom draw for displaying matched addresses and values
- **Status bar** showing scan progress and match count

---

## Project Structure

```
MEM_SCAN/
├── src/
│   ├── the_windows.c      # Main window, WndProc, UI layout
│   ├── proclist.c         # Process list dialog and enumeration
│   ├── process_handle.c   # Handle acquisition and memory read wrappers
│   └── process_array.c    # Dynamic array for storing scan matches
├── Includes/              # Header files
|   ├── the_windows.h    
│   ├── proclist.h         
│   ├── process_handle.h  
│   └── process_array.h    
├── bin/                   # Build output
└── somenotes.md           # Some notes on threading patterns
```

---

## Usage

1. Launch `memscan.exe`
2. Click **"Select Process"** — pick your target from the process list
3. Enter the value you want to find, click **"First Scan"**
4. Change the value in-game/in-app, enter the new value, click **"Next Scan"**
5. Repeat until the address list is narrow enough to identify your target

---

## Technical Notes

- NT functions (`ZwReadVirtualMemory`, `NtQueryVirtualMemory`) are resolved dynamically via `GetProcAddress` from `ntdll.dll` to avoid linking against undocumented import libs
- Scan work is dispatched with `CreateThread`; multi-parameter passing uses a heap-allocated struct cast through `LPVOID lpParam` (see `somenotes.md`)
- Results are stored in a resizable dynamic array (`process_array.c`) that grows on demand

---

## Caveats

- **Requires appropriate privileges** — scanning system or protected processes will fail without elevation
- 32-bit vs 64-bit address space handling depends on the build target; scanning a 64-bit process from a 32-bit build won't work correctly
- This is a learning/research project, not a production tool
- A fair ammount of stuff is incomplete


## License

Do whatever you want with it doubt it's pretty useless when other stuff like cheat engine actually exists
