# From JavaScript to Qt Widgets — a reference

A lookup sheet for anyone moving to Qt Widgets from **React** or **vanilla JavaScript**. Keep it open while you work; you don't need to read it end to end.

The assignment brief (`assignment.md`) tells you *what* to build. This document is about *how the ideas translate*.

**Contents**

1. [The mental model](#1-the-mental-model)
2. [Vanilla JavaScript → Qt](#2-vanilla-javascript--qt)
3. [React → Qt](#3-react--qt)
4. [The same job, side by side](#4-the-same-job-side-by-side)
5. [Traps that catch JavaScript developers](#5-traps-that-catch-javascript-developers)
6. [Vocabulary](#6-vocabulary)

---

## 1. The mental model

A browser page and a Qt window are built from the same idea: **a tree of elements that receive events**.

| Browser | Qt |
|---|---|
| The DOM tree | The widget tree (every widget has a parent) |
| `document` / `window` | Your `QMainWindow` |
| An element (`<input>`, `<button>`) | A widget (`QLineEdit`, `QPushButton`) |
| An event listener | A slot connected to a signal |
| The browser's event loop | `QApplication::exec()` |
| CSS | Layouts (for position/size) and Qt Style Sheets (for looks) |

Two differences run through everything below:

1. **Nothing re-renders by itself.** In the browser you can rebuild `innerHTML` from state; in React the view follows state automatically. In Qt, when data changes, something has to update each affected widget. The clean way is: the data object emits a signal, and each view updates itself in response.
2. **Parents own their children.** `new QLabel("Hi", this)` gives the label a parent, and the parent deletes it. JavaScript's garbage collector has no equivalent in C++; Qt's object tree is what replaces it.

---

## 2. Vanilla JavaScript → Qt

### Finding and creating elements

| JavaScript | Qt |
|---|---|
| `document.querySelector("#ident")` | Keep a pointer as a member: `m_identEdit` |
| `document.createElement("button")` | `new QPushButton(tr("Apply"))` |
| `parent.appendChild(el)` | `layout->addWidget(widget)` — this also sets the parent |
| `el.remove()` | `delete widget;` or `widget->deleteLater();` |
| `el.parentElement` | `widget->parentWidget()` |
| `container.children` | `container->findChildren<QWidget*>()` |
| `el.cloneNode(true)` | No equivalent — construct another widget |

Qt has no `innerHTML`: you never build a UI from a string. You create widget objects and put them in layouts.

### Reading and writing

| JavaScript | Qt |
|---|---|
| `el.textContent = "Nordvik"` | `label->setText("Nordvik")` |
| `input.value` | `lineEdit->text()`, `spinBox->value()`, `comboBox->currentText()` |
| `input.value = "NVK"` | `lineEdit->setText("NVK")` |
| `checkbox.checked` | `checkBox->isChecked()` |
| `select.selectedIndex` | `comboBox->currentIndex()` |
| `el.disabled = true` | `widget->setEnabled(false)` |
| `el.hidden = true` | `widget->setVisible(false)` |
| `el.dataset.id = "NVK"` | `widget->setProperty("id", "NVK")` |
| `el.title = "..."` (tooltip) | `widget->setToolTip("...")` |
| `el.classList.add("alarm")` | `widget->setStyleSheet(...)`, or draw it yourself in `paintEvent` |

### Events

| JavaScript | Qt |
|---|---|
| `btn.addEventListener("click", fn)` | `connect(btn, &QPushButton::clicked, this, fn)` |
| `input.addEventListener("input", fn)` | `connect(edit, &QLineEdit::textChanged, this, fn)` |
| `select.addEventListener("change", fn)` | `connect(combo, &QComboBox::currentIndexChanged, this, fn)` |
| `removeEventListener` | `disconnect(...)` — rarely needed; connections die with the objects |
| `event.target` | `sender()`, or capture the widget in the lambda |
| `new CustomEvent("apply", {detail})` + `dispatchEvent` | Declare your own `signal`, then `emit applyRequested(beacon)` |
| Event bubbling to ancestors | Unhandled events propagate to the parent widget |
| `event.stopPropagation()` | `event->accept()` (and `event->ignore()` to pass it on) |
| `event.preventDefault()` | Accept the event and don't call the base-class handler |
| Event delegation on a container | `installEventFilter` on the child widgets |
| `window.onbeforeunload` | Override `closeEvent(QCloseEvent*)` |
| `DOMContentLoaded` | The widget's constructor (or `showEvent` for first-shown work) |

### Timing, data and the rest

| JavaScript | Qt |
|---|---|
| `setInterval(fn, 1000)` | `QTimer` with `start(1000)`, connected to a slot |
| `setTimeout(fn, 1000)` | `QTimer::singleShot(1000, this, fn)` |
| `clearInterval(id)` | `timer->stop()` |
| `requestAnimationFrame` | `update()` — asks Qt to repaint when it can |
| `console.log("x", value)` | `qDebug() << "x" << value;` |
| `JSON.parse(text)` | `QJsonDocument::fromJson(bytes)` |
| `JSON.stringify(obj)` | `QJsonDocument(obj).toJson()` |
| `fetch(url)` | `QNetworkAccessManager::get()` + its `finished` signal |
| `localStorage` | `QSettings` |
| `alert()` / `confirm()` | `QMessageBox::information()` / `QMessageBox::question()` |
| `<canvas>` 2D context | `QPainter` inside `paintEvent` |
| `array.push(x)` | `list.append(x)` (or `push_back`) |
| `array.map/filter/find` | `<algorithm>`: `std::transform`, `std::copy_if`, `std::find_if` |
| `array.length` | `list.size()` |
| `` `${a} and ${b}` `` | `QString("%1 and %2").arg(a, b)` |
| `str.trim()`, `.toUpperCase()`, `.includes()` | `str.trimmed()`, `.toUpper()`, `.contains()` |
| `import { x } from "./x.js"` | `#include "x.h"`, plus the file listed in `CMakeLists.txt` |
| npm packages | Qt modules (`Qt6::Widgets`) linked in CMake |

---

## 3. React → Qt

| React | Qt |
|---|---|
| A component | A class that inherits `QWidget` |
| Props | Constructor arguments and setters (`setBeacon(...)`) |
| A callback prop (`onApply`) | A custom signal the child emits |
| `onClick={fn}` | `connect(btn, &QPushButton::clicked, this, fn)` |
| An inline arrow handler | A lambda passed to `connect` |
| `useState` | A member variable |
| A state setter (`setCount`) | A setter that updates the widgets, and usually emits a signal |
| Re-render on state change | Nothing automatic — update the widgets yourself |
| `useEffect(fn, [])` | Work in the constructor |
| `useEffect` with `setInterval` | A `QTimer` member |
| `useEffect` cleanup | The destructor — often nothing, thanks to parent ownership |
| `useRef` to reach a DOM node | A pointer member to the child widget |
| Context / a store | A shared `QObject` that emits signals (your `BeaconRegistry`) |
| Redux reducers and actions | Methods on that object; its signals are the "actions" that views listen for |
| `{items.map(i => <Row key={i.id} />)}` | Fill a `QListWidget`/`QTableWidget`, or write a model (`QAbstractTableModel`) |
| Keys in a list | The model's indexes, or the ident you store per row |
| Conditional rendering | `setVisible()` / `setEnabled()` |
| Controlled input | Read the widget when you need it, or connect to its `textChanged` |
| Lifting state up | Put the data in the shared registry, not in the widget |
| CSS-in-JS / CSS modules | Qt Style Sheets, or `QPalette` |
| Flexbox | `QHBoxLayout` / `QVBoxLayout` (plus `QFormLayout`, `QGridLayout`) |
| `flex: 1` spacer | `layout->addStretch()` |
| React DevTools component tree | Qt Creator's debugger, plus `dumpObjectTree()` |

**Where the analogy breaks down.** A React component *re-runs* to produce a new description of the UI. A Qt widget is a long-lived object you mutate. So "derive everything from state on every render" becomes "when the data changes, change the widgets that show it". That's what the registry's signals are for. Qt's model/view classes (stretch goal B) get closest to the React idea: the model holds the data, the view redraws whatever changed.

---

## 4. The same job, side by side

### A button that updates a label

```js
// vanilla JS
const btn = document.createElement("button");
btn.textContent = "Apply";
btn.addEventListener("click", () => {
    status.textContent = "Applied";
});
form.appendChild(btn);
```

```cpp
// Qt
auto* btn = new QPushButton(tr("Apply"));
connect(btn, &QPushButton::clicked, this, [this] {
    m_status->setText(tr("Applied"));
});
layout->addWidget(btn);          // also makes btn a child of this widget
```

### Showing a list of items

```js
// vanilla JS
list.innerHTML = "";
for (const b of beacons) {
    const li = document.createElement("li");
    li.textContent = `${b.ident} — ${b.name}`;
    list.appendChild(li);
}
```

```cpp
// Qt
m_list->clear();
for (const Beacon& b : registry.beacons()) {
    m_list->addItem(QString("%1 — %2").arg(b.ident, b.name));
}
```

### Telling the parent something happened

```js
// vanilla JS — a custom event
el.dispatchEvent(new CustomEvent("apply", { detail: beacon, bubbles: true }));

// React — a callback prop
props.onApply(beacon);
```

```cpp
// Qt — a signal declared in beaconeditor.h
signals:
    void applyRequested(const Beacon& beacon);

// ...emitted when the button is clicked
emit applyRequested(beacon());

// ...and connected by the parent
connect(editor, &BeaconEditor::applyRequested, this, &MainWindow::applyBeacon);
```

### Something that repeats

```js
const id = setInterval(tick, 1000);
clearInterval(id);
```

```cpp
auto* timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, &BeaconSimulator::tick);
timer->start(1000);
timer->stop();
```

### Reading a JSON file

```js
const data = JSON.parse(await (await fetch("beacons.json")).text());
for (const b of data.beacons) { /* ... */ }
```

```cpp
QFile file(path);
if (!file.open(QIODevice::ReadOnly)) { /* handle it */ }
const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
const QJsonArray array = doc.object().value("beacons").toArray();
for (const QJsonValue& value : array) {
    const QJsonObject o = value.toObject();
    const QString ident = o.value("ident").toString();
}
```

---

## 5. Traps that catch JavaScript developers

| Trap | What happens | What to do |
|---|---|---|
| **Expecting a re-render** | You change the data and the window doesn't move | Update the widgets, ideally in a slot connected to the data's signal |
| **`delete`-ing a child widget** | A crash, or a double delete | Let the parent own it. Use `deleteLater()` if you really must remove one |
| **A long loop in a slot** | The whole window freezes — one thread runs the UI | Keep slots short; use a `QTimer` for repeated work |
| **Capturing `this` in a lambda that outlives the object** | A dangling pointer | Always pass a context object: `connect(src, sig, this, [this]{...})` |
| **Comparing floating-point values with `==`** | Two "equal" numbers aren't | `qFuzzyCompare` (see `operator==` in `beacon.h`) |
| **Assuming signals are asynchronous** | Within one thread, a signal calls its slots immediately, like a function call | Fine — just don't expect a "next tick" |
| **`QString` vs `std::string`** | Qt APIs want `QString` | Stay with `QString` in Qt code; `.toStdString()` only at a boundary |
| **`int` division** | `7 / 2` is `3`, not `3.5` | Use `7.0 / 2` |
| **No truthiness** | `if (str)` doesn't mean "not empty" | `if (!str.isEmpty())`, `if (ptr)` for pointers |
| **No optional chaining** | `a?.b` doesn't exist | Check the pointer: `if (w) w->setText(...)` |
| **Forgetting `Q_OBJECT`** | Signals silently don't work, or a vtable link error | Add the macro, re-run CMake, rebuild |
| **`var`-style late declaration** | Uninitialised members hold junk | Initialise members where you declare them |

---

## 6. Vocabulary

| Term | Meaning |
|---|---|
| **Signal** | A message an object sends when something happens. You declare it; moc writes it |
| **Slot** | A function connected to a signal. Any member function can be one |
| **`connect`** | Wires a signal to a slot; done once, usually in a constructor |
| **`emit`** | Sends a signal. It's only a marker word — it compiles to nothing |
| **moc** | The Meta-Object Compiler: generates the code behind `Q_OBJECT`, signals and properties |
| **Widget** | Anything visible. A widget with no parent is a window |
| **Layout** | Positions and resizes child widgets. Qt's flexbox |
| **Parent/child** | Ownership: a parent deletes its children |
| **Event** | Input from outside (mouse, key, timer, repaint), delivered to a widget |
| **Event loop** | `app.exec()`: waits for events and dispatches them |
| **Model/view** | Qt's data-driven lists and tables: a model holds the data, views display it |
| **`tr("...")`** | Marks text for translation. Use it for anything the user reads |
| **RAII** | Objects clean up in their destructor. It replaces JavaScript's garbage collection |
