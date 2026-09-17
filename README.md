# Beacon Station Manager

A desktop tool, built with Qt Widgets, for configuring navigation beacons and monitoring their status.

The full instructions are in the assignment brief (`assignment.md`). Start there.

Coming from React or plain JavaScript? `js-to-qt-reference.md` translates the ideas you already know into Qt.

**New to Qt Widgets?** Work through the *Qt Widgets by Example* session and its playground project first (or alongside this). Each example there is a few lines: a button that shows a message, an input box the button reads, a list you can add to. Milestone 1 is those examples put together.

---

## Quick start (Qt Creator)

1. **Open** Qt Creator → *File → Open File or Project…* → select `CMakeLists.txt` in this folder.
2. **Pick the kit** *Desktop Qt 6.x MinGW 64-bit* → *Configure Project*.
3. **Run the app:** **Ctrl+R**. A window titled *Beacon Station Manager* opens.

That's it — this assignment is about the UI, so there are no tests to run.

---

## What's in this folder

```
.
├── src/
│   ├── main.cpp            starts the application and shows the main window
│   ├── mainwindow.h/.cpp   the main window (empty for now)
│   └── beacon.h/.cpp       the Beacon data type and validateBeacon()
├── tests/
│   └── tst_beacon.cpp      optional example test — not built (see assignment.md)
├── data/
│   └── beacons.json        five sample beacons (File → Open)
├── CMakeLists.txt          builds everything
├── assignment.md           the assignment brief
├── js-to-qt-reference.md   React / vanilla JavaScript → Qt lookup sheet
└── README.md               this file
```

`CMakeLists.txt` builds two targets:

| Target | What it is | Add to it… |
|---|---|---|
| `beacon_core` | Everything that is **not** user interface: data, validation, registry, simulator | …your non-UI `.h`/`.cpp` files |
| `beacon_manager` | The application | …your widget classes |

There's a commented-out test block at the bottom of `CMakeLists.txt`. Tests are optional here; uncomment it only if you decide to write some.

After editing `CMakeLists.txt`, Qt Creator reconfigures automatically when you save.

---

## Signals and slots checklist

If a signal or slot "does nothing", check these first:

- [ ] The class has `Q_OBJECT` at the top of its declaration.
- [ ] The class's `.h` and `.cpp` are listed in `CMakeLists.txt`, and CMake has re-run since you added `Q_OBJECT`. Qt generates the signal code during that step.
- [ ] You used the pointer syntax: `connect(sender, &Sender::signal, receiver, &Receiver::slot);`
- [ ] The `connect` call actually runs. Put a `qDebug() << "connected";` next to it.
- [ ] The sender and receiver still exist. A widget created without a parent inside a function may already be gone.

---

## Building from the command line (optional)

Qt Creator is the easiest way. If you prefer a terminal, Qt's compiler and tools need to be on your `PATH`. In PowerShell, adjust `6.x.x` to your version:

```
$env:PATH = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;" + $env:PATH
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/mingw_64
cmake --build build
./build/beacon_manager
```

To run the app outside Qt Creator, the Qt DLLs must be on your `PATH` too: add `C:\Qt\6.x.x\mingw_64\bin`.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| *No valid kits found* when opening the project | Qt isn't installed for MinGW. Re-run the Qt Maintenance Tool and add **Qt 6.x → MinGW 64-bit**. |
| `undefined reference to vtable for …` | Usually `Q_OBJECT` was added after the class was first built. Run *Build → Run CMake*, then rebuild. Also check the class's `.h` and `.cpp` are both listed in `CMakeLists.txt`. |
| A new file you added doesn't compile into the app | Add its `.h` and `.cpp` to the right target in `CMakeLists.txt`, then *Build → Run CMake*. |
| The app starts from Qt Creator but not by double-clicking the `.exe` | The Qt DLLs aren't found. Run it from Qt Creator, or see *Building from the command line*. |
| *(If you add tests)* one that uses a timer hangs or fails | Use `QTEST_GUILESS_MAIN` instead of `QTEST_APPLESS_MAIN` in that test file. |
