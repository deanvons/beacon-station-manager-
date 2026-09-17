# Beacon Station Manager — Qt Widgets Fundamentals

Aircraft find their way using radio beacons on the ground. Someone has to configure those beacons, and someone has to watch that they're working. You'll build a desktop tool that does both: a **configuration editor** for a set of beacons, and a **monitoring console** that shows their live status.

**Tech stack:** C++17, Qt 6 Widgets, CMake. Recommended IDE: Qt Creator.

**How to approach this:** the brief is deliberately bigger than the time you have. Work through the milestones **in order** and go as far as you can. A clean, working Milestone 4A is worth more than a half-finished Milestone 7. The point is to meet the fundamentals of Qt Widgets properly, not to finish everything.

**Rough plan:**
- **Week 1:** Milestones 0–4A (a working configuration tool).
- **Week 2:** Milestone 4B (polish), then 5–7 (the console).
- Stretch goals if you get there.

---

## 1) Setup

1. **Install Qt** with the Qt Online Installer from qt.io (a free Qt account is needed). Under *Qt 6.8* or newer, select:
   - **MinGW 64-bit** (the Qt libraries)
   - Under *Developer and Designer Tools*: **Qt Creator**, the matching **MinGW** compiler, **CMake** and **Ninja**
2. **Open the project:** in Qt Creator, *File → Open File or Project…* and pick `CMakeLists.txt` in this folder. Choose the *Desktop Qt 6.x MinGW 64-bit* kit and click *Configure Project*.
3. **Run it:** press **Ctrl+R**. A window titled "Beacon Station Manager" should open.
4. **Read `src/main.cpp` and `src/mainwindow.cpp`**, including the comments. That's the whole application so far.

Starter files:

| File | What it is |
|---|---|
| `src/main.cpp` | Creates the application and shows the main window |
| `src/mainwindow.h/.cpp` | An empty main window — you'll build it out |
| `src/beacon.h/.cpp` | The `Beacon` data type (given) and `validateBeacon` (you implement it) |
| `data/beacons.json` | Five sample beacons to load |
| `CMakeLists.txt` | Builds the app and the non-UI library |
| `README.md` | Quick start, project layout, troubleshooting |
| `js-to-qt-reference.md` | Lookup sheet: JavaScript ideas translated into Qt. Keep it open while you work |

**When you add a file**, add it to `CMakeLists.txt`: non-UI code (data, registry, simulator) goes in `beacon_core`, widgets go in `beacon_manager`.

---

## 2) The domain in two minutes

You don't need any aviation knowledge beyond this section.

**A navigation beacon** is a radio transmitter on the ground. Aircraft tune in to its frequency to work out where they are. Each beacon identifies itself with a short code (its **ident**), such as `NVK`. This tool manages two kinds:

| Type | What it tells the aircraft | Frequency band |
|---|---|---|
| **NDB** — Non-Directional Beacon | *"The beacon is in that direction."* The aircraft's needle simply points at it. | 190.0–1750.0 **kHz**, in steps of 0.5 |
| **VOR** — VHF Omnidirectional Range | *"You are on this compass bearing from the station."* More precise than an NDB. | 108.00–117.95 **MHz**, in steps of 0.05 |

A VOR can also have **DME** (Distance Measuring Equipment) fitted, which tells the aircraft how far away the station is. An NDB never has DME.

Every beacon also has a position (latitude and longitude), a usable **range** in nautical miles (NM), and an **in service** flag. A beacon that is *out of service* is switched off for maintenance.

**The console** watches each beacon's **transmitter power**, as a percentage of normal:

| Power | Status | Colour |
|---|---|---|
| Beacon out of service | **OFF** | Grey |
| 90% or more | **NORMAL** | Green |
| 70% to under 90% | **WARNING** | Amber |
| Under 70% | **ALARM** | Red |

There is no real hardware: the power readings are **simulated** (Milestone 6).

All beacon names, idents and positions in this assignment are fictional.

---

## 3) Three things Qt does differently

Whatever you have built before, these three rules catch people out. Everything in this assignment leans on them.

1. **Nothing updates itself.** When data changes, some code has to update each widget that shows it. The clean way is for the data object to emit a signal, and for each view to update itself in response. You'll build exactly that.
2. **Parents own their children.** When you create a widget with a parent (`new QLabel("Hi", this)`), or add it to a layout or a window, the parent deletes it automatically when the parent is deleted. That's why Qt code has plenty of `new` and almost no `delete`. A widget *without* a parent is yours to delete.
3. **Signals and slots are typed and checked by the compiler.** Use the pointer syntax (`&QPushButton::clicked`); the compiler then catches typos and wrong argument types. Any class that declares signals or slots needs the `Q_OBJECT` macro.

If you have written JavaScript before, `js-to-qt-reference.md` translates the ideas you already know into Qt: elements and events, timers, JSON, and the traps worth knowing.

---

## 4) Core requirements

### The rules

- **Build the user interface in code**, not in Qt Designer (Designer is stretch A). Writing the layouts yourself is how you learn how widgets nest and who owns what.
- **Keep non-UI code out of widgets.** Validation, the registry and the simulator must not include any widget headers. Keeping them separate is what makes the UI simple.
- **Commit at least once per milestone**, with a meaningful message.
- **No tests are required.** This assignment is about the UI. See "Adding tests later" at the end if you want them.

---

### Milestone 0 — Get it running

**Warm-up (about 5 minutes): make a project from scratch.** This is how you'd start any new Qt app, so it's worth seeing once.

1. In Qt Creator: *File → New Project… → Application (Qt) → Qt Widgets Application*.
2. Name it `hello_qt` and save it somewhere outside this repository. Keep the defaults: build system **CMake**, class `MainWindow`, base class `QMainWindow`. Tick **Generate form** (this creates a Qt Designer `.ui` file).
3. Pick the MinGW kit and press **Ctrl+R**. An empty window appears.
4. Look at what the wizard generated: `main.cpp`, `mainwindow.h/.cpp`, `mainwindow.ui` and `CMakeLists.txt`. Double-click `mainwindow.ui` to see Qt Designer, then close it.
5. Close that project (*File → Close All Projects and Editors*).

The starter has the same shape, minus the `.ui` file: you'll build the UI in code.

**Then the real thing:**

- Open the starter (*File → Open File or Project…* → this folder's `CMakeLists.txt`).
- Build and run the starter. See the window.
- Read `main.cpp` and `mainwindow.cpp`, including the comments.

---

### Milestone 1 — The layout (no behaviour yet)

Build the configuration screen in `MainWindow`, by hand in code. Nothing needs to *do* anything yet.

- The central widget is a `QTabWidget` with two tabs: **Configuration** and **Console** (leave Console empty for now).
- The Configuration tab is a `QSplitter` with:
  - **Left:** a `QListWidget` for the beacons, with **Add** and **Delete** buttons underneath it.
  - **Right:** a new widget class, **`BeaconEditor`** (a `QWidget` subclass in its own files), containing a `QFormLayout` with these rows:

    | Label | Widget | Settings |
    |---|---|---|
    | Ident | `QLineEdit` | max length 3 |
    | Name | `QLineEdit` | max length 40 |
    | Type | `QComboBox` | items `NDB`, `VOR` |
    | Frequency | `QDoubleSpinBox` | VOR settings for now: 108.00 to 117.95, step 0.05, 2 decimals, suffix `" MHz"` (Milestone 3 makes this change with the type) |
    | DME fitted | `QCheckBox` | |
    | Latitude | `QDoubleSpinBox` | −90 to 90, 4 decimals |
    | Longitude | `QDoubleSpinBox` | −180 to 180, 4 decimals |
    | Range | `QSpinBox` | 10 to 200, suffix `" NM"` |
    | In service | `QCheckBox` | |

  - Under the form: a `QLabel` for error messages (red text), then **Apply** and **Revert** buttons on one row, aligned right.

**Check yourself:** resize the window. Everything should stretch and move sensibly. If something doesn't, it isn't in a layout.

---

### Milestone 2 — The data behind the screen (no UI)

**A) Validation — `validateBeacon` in `src/beacon.cpp`**

Implement the rules below. `validateBeacon` returns one message per broken rule, using **exactly** these texts; an empty list means the beacon is valid.

| Rule | Message |
|---|---|
| Ident is 2 or 3 characters, capital letters A–Z only | `Ident must be 2 to 3 capital letters (A-Z).` |
| Name is not empty (ignoring spaces) | `Name is required.` |
| Name is at most 40 characters | `Name must be 40 characters or fewer.` |
| NDB: frequency is 190.0–1750.0 | `NDB frequency must be between 190.0 and 1750.0 kHz.` |
| NDB: frequency is a multiple of 0.5 (only checked if in range) | `NDB frequency must be a multiple of 0.5 kHz.` |
| NDB: DME is not fitted | `Only a VOR can have DME.` |
| VOR: frequency is 108.00–117.95 | `VOR frequency must be between 108.00 and 117.95 MHz.` |
| VOR: frequency is a multiple of 0.05 (only checked if in range) | `VOR frequency must be a multiple of 0.05 MHz.` |
| Latitude is −90 to 90 | `Latitude must be between -90 and 90.` |
| Longitude is −180 to 180 | `Longitude must be between -180 and 180.` |
| Range is 10 to 200 | `Range must be between 10 and 200 NM.` |

> **Worked example — "is it a multiple of 0.05?"** Floating-point numbers aren't exact, so `fmod(113.25, 0.05) == 0` doesn't work reliably. Scale to whole units instead:
>
> - **113.25 MHz:** 113.25 × 100 = 11325 → a whole number (within 0.000001), and 11325 ÷ 5 leaves 0 → **valid**
> - **113.27 MHz:** 113.27 × 100 = 11327 → whole, but 11327 ÷ 5 leaves 2 → **invalid**
> - **113.251 MHz:** 113.251 × 100 = 11325.1 → not a whole number → **invalid**
>
> For an NDB, multiply by 10 instead: 402.5 × 10 = 4025, and 4025 ÷ 5 leaves 0 → **valid**.

**B) The registry — `BeaconRegistry`** (new files `src/beaconregistry.h/.cpp`)

This is the one place that holds the list of beacons — the single source of truth. Every view reads from it, and it announces every change with a signal.

```cpp
class BeaconRegistry : public QObject
{
    Q_OBJECT
public:
    explicit BeaconRegistry(QObject* parent = nullptr);

    const QList<Beacon>& beacons() const;
    int indexOf(const QString& ident) const;               // -1 if not found

    QStringList add(const Beacon& beacon);                 // empty list = added
    QStringList update(int index, const Beacon& beacon);   // empty list = updated
    void remove(int index);
    void clear();

    bool loadFromFile(const QString& path, QString* errorMessage);
    bool saveToFile(const QString& path, QString* errorMessage) const;

signals:
    void beaconAdded(int index);
    void beaconChanged(int index);
    void beaconRemoved(int index);
    void beaconsReset();                                   // after clear() or a successful load
};
```

Rules:

- `add` and `update` return the messages from `validateBeacon`, plus `Ident XXX is already in use.` if another beacon has the same ident (with `XXX` replaced by the ident). They change nothing and emit nothing if the list isn't empty.
- `update` may keep a beacon's own ident. It's only a duplicate if a *different* beacon has it.
- `loadFromFile` either loads **everything** or **nothing**. If the file can't be read, isn't valid JSON, has a `version` other than `1`, has an unknown `type`, or contains an invalid or duplicate beacon, it returns `false`, sets `errorMessage`, and leaves the registry unchanged.
- The file format is the one in `data/beacons.json`. `type` is the string `"NDB"` or `"VOR"`. Use `QJsonDocument`, `QJsonObject` and `QJsonArray`.

---

### Milestone 3 — Wire it up

Now connect the pieces with signals and slots. `MainWindow` owns one `BeaconRegistry`.

**`BeaconEditor`** gets this public interface:

```cpp
void setBeacon(const Beacon& beacon);   // fill the form
Beacon beacon() const;                  // read the form
void setErrors(const QStringList& messages);

signals:
    void applyRequested(const Beacon& beacon);
    void dirtyChanged(bool dirty);
```

Behaviour:

- **The list** shows one row per beacon: `NVK — Nordvik (VOR/DME)`, with `(NDB)`, `(VOR)` or `(VOR/DME)` at the end. Out-of-service beacons are shown in grey italics.
- **Selecting a row** fills the editor. With nothing selected, the editor is disabled.
- **Changing Type** reconfigures the Frequency box:
  - NDB: range 190.0–1750.0, step 0.5, 1 decimal, suffix `" kHz"`
  - VOR: range 108.00–117.95, step 0.05, 2 decimals, suffix `" MHz"`
- **DME fitted** is only enabled for a VOR. Switching to NDB unticks it.
- **Ident** is converted to capitals as you type.
- **Dirty tracking:** Apply and Revert are enabled only when the form differs from the beacon it was filled with. Compare them with the `==` / `!=` operators provided in `beacon.h`: `beacon() != m_original`. The editor emits `dirtyChanged` when that changes.
- **Apply** emits `applyRequested(beacon())`. `MainWindow` passes it to `registry.update(...)`. If that returns messages, they're shown with `setErrors` and nothing else changes.
- **Revert** refills the form from the stored beacon.
- **The list updates itself** by reacting to the registry's `beaconChanged` signal. Apply must **not** update the list directly. This is rule 1 from Section 3: nothing updates itself.

**Temporary shortcut:** until Milestone 4A, load `data/beacons.json` once at startup so you have data to work with.

---

### Milestone 4A — A working application

- **Add:** creates a beacon and selects it, ready to edit:
  - ident: the first of `NWA`, `NWB`, … `NWZ` that isn't already in use;
  - name `New beacon`, type VOR, 108.00 MHz, no DME, position 0/0, range 50, in service.
- **Delete:** asks for confirmation (`QMessageBox::question`), then removes the selected beacon.
- **File menu:** New, Open…, Save, Save As… and Quit. Each item is a `QAction` with a standard shortcut (e.g. `QKeySequence::Open`).
  - **New** clears the registry.
  - **Save** writes to the current file, or behaves like Save As… if there isn't one yet.
- **Open / Save As:** use `QFileDialog` with the filter `Beacon files (*.json)`. If loading or saving fails, show the error in a `QMessageBox::warning`.
- **Remove the Milestone 3 startup shortcut.** Data now comes from File → Open.

At this point New and Open simply replace what's there, without asking. Milestone 4B adds the safety prompts.

---

### Milestone 4B — Polish

- **Beacon menu:** Add and Delete as `QAction`s, connected to the same code as the buttons.
- **Toolbar:** Open, Save, Add and Delete, reusing the same `QAction` objects as the menus.
- **Status bar:** `5 beacons` on the left. After saving, show `Saved` for 3 seconds.
- **Window title:** `Beacon Station Manager — beacons.json[*]`. Call `setWindowModified(true)` whenever the registry changes and `false` after a successful save or load; Qt then shows the `*` for you.
- **Unsaved changes:** New, Open and closing the window (override `closeEvent`) ask *Save / Discard / Cancel* if there are unsaved changes.
- **Switching beacons with unapplied edits** in the editor asks *Apply / Discard / Cancel*.

---

### Milestone 5 — The console: status tiles

**`StatusTile`** is a custom widget that draws itself (new files; a `QWidget` subclass):

```cpp
explicit StatusTile(const QString& ident, QWidget* parent = nullptr);
void setReading(BeaconType type, BeaconStatus status, double powerPercent);
QSize sizeHint() const override;              // 160 × 100

signals:
    void clicked(const QString& ident);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
```

- `paintEvent` uses `QPainter` to draw a rounded rectangle filled with the status colour:

  | Status | Colour |
  |---|---|
  | OFF | `#9E9E9E` |
  | NORMAL | `#2E7D32` |
  | WARNING | `#F9A825` |
  | ALARM | `#C62828` |

  On top of it, draw the ident in large bold text and, underneath, the type and power (`VOR · 97%`) or `OUT OF SERVICE`.
- `setReading` stores the values and calls `update()`. **Never call `paintEvent` yourself**: `update()` asks Qt to repaint when it's ready.
- The Console tab shows one tile per beacon in a `QGridLayout`, 4 per row, in registry order.
- The console reacts to the registry's signals in two ways:
  - **Rebuild from scratch** on `beaconAdded`, `beaconRemoved` and `beaconsReset`: delete every tile, then create one per beacon in registry order. Add a beacon on the Configuration tab and its tile appears.
  - **Update in place** on `beaconChanged` (and, from Milestone 6, `readingsUpdated`): call `setReading` on the existing tiles. Don't recreate them.
- **`BeaconStatus`** goes in a new file, `src/beaconsimulator.h`. Create it now, containing only the enum, and add it to `beacon_core` in `CMakeLists.txt`:

  ```cpp
  enum class BeaconStatus { Off, Normal, Warning, Alarm };
  ```

  Until Milestone 6, show every beacon as NORMAL at 100%, or OFF if it's out of service.

---

### Milestone 6 — Live simulation

**A) Status and power rules** (add to `src/beaconsimulator.h` from Milestone 5, plus a new `src/beaconsimulator.cpp` in `beacon_core`)

```cpp
enum class BeaconStatus { Off, Normal, Warning, Alarm };

QString statusName(BeaconStatus status);          // "OFF", "NORMAL", "WARNING", "ALARM"
BeaconStatus statusFor(bool inService, double powerPercent);
double nextPower(double powerPercent, int step);
```

- `statusFor` follows the table in Section 2: Off if out of service, otherwise Normal at 90 or more, Warning at 70 or more, Alarm below 70.
- `nextPower` moves the power randomly, but pulls it back toward 100% so it doesn't drift away forever:

  ```
  next = power + step + (100 − power) ÷ 10,   then limited to at most 100 (and at least 0)
  ```

> **Worked example.** A fault has dropped a beacon to 60%.
>
> - Tick 1, step −2: 60 − 2 + (100 − 60) ÷ 10 = 60 − 2 + 4 = **62.0** → still ALARM
> - Tick 2, step +5: 62 + 5 + (100 − 62) ÷ 10 = 62 + 5 + 3.8 = **70.8** → now WARNING (a status change)
> - A healthy beacon at 99%, step +5: 99 + 5 + 0.1 = 104.1 → limited to **100.0**

**B) `BeaconSimulator`**

```cpp
class BeaconSimulator : public QObject
{
    Q_OBJECT
public:
    BeaconSimulator(BeaconRegistry* registry, quint32 seed, QObject* parent = nullptr);

    void start(int intervalMs = 1000);
    void stop();
    bool isRunning() const;

    double powerPercent(const QString& ident) const;
    BeaconStatus status(const QString& ident) const;

public slots:
    void tick();                                  // one simulation step
    void injectFault(const QString& ident);

signals:
    void readingsUpdated();
    void statusChanged(const QString& ident, BeaconStatus from, BeaconStatus to);
};
```

Rules:

- Every beacon starts at **100%**.
- A `QTimer` calls `tick()` every `intervalMs` while running. `tick()` is public so you can also call it directly, without waiting for a timer.
- **`tick()`** goes through the beacons **in registry order**. For each *in-service* beacon it draws one step with `random.bounded(-5, 6)` (a whole number from −5 to 5) and applies `nextPower`. It then re-evaluates every status and emits `readingsUpdated()`.
- The random numbers come from a `QRandomGenerator` created with `seed`. **The same seed always gives the same readings**, so a run is repeatable.
- **Out-of-service beacons** don't draw a step. They're OFF, and their power is reset to 100%, so they come back healthy.
- **`injectFault(ident)`** sets an in-service beacon's power to 60% and re-evaluates. It does nothing for an out-of-service beacon.
- **On any registry signal**, the simulator rebuilds its readings map from the registry:
  - idents that no longer exist are dropped;
  - new idents start at 100%;
  - existing ones keep their power;
  - then every status is re-evaluated.

  Taking a beacon out of service on the Configuration tab therefore turns it OFF straight away. A renamed beacon counts as a new ident.
- **`statusChanged`** is emitted only when a beacon's status actually changes, never on the first evaluation of a new beacon.

**C) Connect it to the console**

- `MainWindow` creates the simulator with seed **42** and starts it when the app starts.
- On `readingsUpdated`, every tile gets its new reading.
- Above the tiles, add a control row:
  - a **Start/Stop** button whose text matches the state;
  - a `QComboBox` of in-service beacon idents with an **Inject fault** button next to it.
- The status bar shows `Simulation running (seed 42)` or `Simulation stopped`.

---

### Milestone 7 — Alarm log

- Under the tiles, add a `QTableWidget` with the columns **Time** (`hh:mm:ss`), **Ident**, **From**, **To** and **Acknowledged**.
- Every `statusChanged` adds a row **at the top**.
- Rows where *To* is ALARM and not yet acknowledged are shown in **bold**.
- **Acknowledge** (acts on the selected rows) and **Acknowledge all** buttons set the Acknowledged cell to `Yes` and remove the bold.
- The Console tab's title shows the number of unacknowledged ALARM rows: `Console (2)`, or just `Console` when there are none.
- **Clicking a tile** (`StatusTile::clicked`) switches to the Configuration tab and selects that beacon.

---

## 5) Stretch goals

Pick any, in any order.

**A) Qt Designer.** Rebuild `BeaconEditor`'s layout as a `.ui` file in Qt Designer and load it with the generated `Ui::BeaconEditor` class. Keep the same public interface, so nothing else changes. In your README, compare this with building the layout in code: what got easier, and what got harder?

**B) Model/View.** Replace the `QListWidget` with a `QTableView` backed by your own `BeaconTableModel : QAbstractTableModel`, with columns Ident, Name, Type, Frequency and In service. The model forwards the registry's signals as `beginInsertRows` / `endInsertRows`, `dataChanged` and so on. Add a search box using `QSortFilterProxyModel` that filters by ident or name. This is Qt's way of making a view follow its data automatically.

**C) Style sheets.** Give the console a dark theme with a Qt Style Sheet (`setStyleSheet`), Qt's version of CSS. Keep the Configuration tab in the default style.

**D) Plan view.** A third tab with a custom-painted map: each beacon is a dot at its latitude/longitude (scale the positions to fit the widget), coloured by status, with a faint circle showing its range. Out-of-service beacons are drawn hollow.

**E) Remember settings.** Use `QSettings` to restore the window size and position, the splitter position, and the last opened file.

**F) Add tests.** See the section below.

---

## 6) Adding tests later (optional)

Nothing in this assignment needs tests, and you should not let them slow you down. When you want them — or when we come back to this together — Qt gives you two kinds.

**Testing the logic (the easy win).** `validateBeacon`, `BeaconRegistry` and `BeaconSimulator` contain no widgets, so a test can call them directly: does an ident of one letter produce the right message? Does `add` reject a duplicate? Does the same seed give the same readings? This is where the value is.

**Testing the widgets.** Qt Test can also drive the UI, much like React Testing Library: type with `QTest::keyClicks(identEdit, "NVK")`, click with `QTest::mouseClick(applyButton, Qt::LeftButton)`, then check what happened. `QSignalSpy` records the signals a widget emitted so a test can check them.

If you want to try it, Qt Test is already available (`find_package(Qt6 ... Test)`), and the pattern is one small `.cpp` per test with its own executable.

---

## 7) Submission

Submit the following in a private repository:

- All source code, with a `README.md` that includes:
  - which milestone you reached;
  - a screenshot of the Configuration tab and, if you got there, the Console tab;
  - a few sentences on what surprised you most about Qt.
- A clean commit history with meaningful commit messages — at least one per milestone.
- Add your lecturer as a Maintainer.
- Submit only the repository link — not the "clone with SSH" link.

**Deadline:** TBD
