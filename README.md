<!DOCTYPE html>
<html>
<body>

  <h1>Titanic</h1>
  <img alt="e-schooler Logo" src="titanic.png" width="250px">
  <p>C++ task management application with a graphical user interface (Dear ImGui).</p>

  <hr>

  <h2>📋 Description</h2>
  <p><strong>Titanic</strong> is a task management system that allows users to:</p>
  <ul>
    <li><strong>Add</strong> tasks with title, description, priority, deadline, and duration.</li>
    <li><strong>Sort</strong> tasks by priority (Bubble Sort) or by deadline (Bubble Sort).</li>
    <li><strong>Search</strong> tasks by title (Linear Search).</li>
    <li><strong>Filter</strong> tasks by status or priority.</li>
    <li><strong>Recursively calculate</strong> total duration and the count of high-priority tasks.</li>
    <li><strong>Mark</strong> tasks as completed.</li>
    <li><strong>Edit</strong> and <strong>delete</strong> tasks.</li>
    <li><strong>Auto-save</strong> data to <code>tasks.dat</code>.</li>
  </ul>

  <hr>

  <h2>📑 Table of Contents</h2>
  <ul>
    <li><a href="#repository-structure">Repository Structure</a></li>
    <li><a href="#features">Features</a></li>
    <li><a href="#implemented-algorithms">Implemented Algorithms</a></li>
    <li><a href="#tools-and-languages">Tools and Languages</a></li>
    <li><a href="#team">Team</a></li>
  </ul>

  <hr>

  <h2 id="repository-structure">📂 Repository Structure</h2>
  <pre>
Titanic/
├── assets/             # Resources
│   └── fonts/          # Fonts (FontAwesome, JetBrainsMono, Syne)
├── deps/               # Dependencies
│   ├── glfw/           # GLFW libraries and headers (lib-vc2022)
│   └── imgui/          # Dear ImGui source code and backends
├── include/            # Header files
│   ├── data.h          # Data structures and declarations
│   ├── logic.h         # Business logic declarations
│   └── ui.h            # UI function declarations
├── src/                # Source code
│   ├── main.cpp        # Entry point — GLFW + ImGui loop
│   ├── data.cpp        # Data layer implementation
│   ├── logic.cpp       # Logic layer implementation
│   └── ui.cpp          # Presentation layer implementation
├── out/                # Build artifacts
│   └── build/          # Build directory
├── docs/               # Additional documentation
├── tests/              # Unit tests
├── CMakeLists.txt      # CMake configuration
└── CppProperties.json  # IntelliSense configuration (MSVC)
  </pre>

  <hr>

  <h2 id="features">🚀 Features</h2>
  <ul>
    <li>Interactive task tracking interface</li>
    <li>Recursive logic for statistics: <code>totalDurationRecursive()</code> and <code>countHighPriorityRecursive()</code></li>
    <li>Persistent storage using pipe-separated <code>tasks.dat</code></li>
    <li>Dynamic UI with custom font integration (FontAwesome & JetBrainsMono)</li>
  </ul>

  <hr>

  <h2 id="implemented-algorithms">⚙️ Implemented Algorithms</h2>
  <table width="100%">
    <thead>
      <tr align="left">
        <th>Algorithm</th>
        <th>Description</th>
        <th>File</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><strong>Bubble Sort (Priority)</strong></td>
        <td>Sorts tasks by priority level</td>
        <td><code>logic.cpp</code></td>
      </tr>
      <tr>
        <td><strong>Bubble Sort (Deadline)</strong></td>
        <td>Sorts tasks by YYYY-DD-MM date</td>
        <td><code>logic.cpp</code></td>
      </tr>
      <tr>
        <td><strong>Linear Search</strong></td>
        <td>Searches by title (case-insensitive)</td>
        <td><code>logic.cpp</code></td>
      </tr>
      <tr>
        <td><strong>Recursion - Sum</strong></td>
        <td>Calculates total duration of all tasks</td>
        <td><code>logic.cpp</code></td>
      </tr>
      <tr>
        <td><strong>Recursion - Counter</strong></td>
        <td>Counts tasks with HIGH priority</td>
        <td><code>logic.cpp</code></td>
      </tr>
    </tbody>
  </table>

  <hr>

  <h2 id="tools-and-languages">🛠️ Tools and Languages</h2>
  <p>
<a href="https://visualstudio.microsoft.com/"><img src="https://img.icons8.com/fluency/48/000000/visual-studio.png" alt="Visual Studio"/></a>
<a href="https://www.microsoft.com/en-ww/microsoft-365/word"><img src="https://img.icons8.com/fluency/48/000000/microsoft-word-2019.png" alt="MS Word logo" width=48px /></a>
<a href="https://www.microsoft.com/en-us/microsoft-365/powerpoint"><img src="https://img.icons8.com/fluency/48/000000/microsoft-powerpoint-2019.png" alt="MS PowerPoint logo" width=48px /></a>
<a href="https://github.com"><img src="https://cdn-icons-png.flaticon.com/512/25/25231.png" alt="GitHub" height=48px width=48px/></a>
<a href="https://teams.microsoft.com/"><img src="https://img.icons8.com/?size=100&id=zQ92KI7XjZgR&format=png&color=000000" alt="Teams" width="48px"></a>
<a href="https://www.cplusplus.com/"><img src="https://img.icons8.com/color/48/000000/c-plus-plus-logo.png" alt="C++"/></a>
  </p>

  <hr>

  <h2 id="team">👤 Team</h2>
  <table border="1" cellpadding="5">
    <thead>
      <tr align="left">
        <th>Name</th>
        <th>Role</th>
        <th>Class</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><strong>Dara Sokrateva</strong></td>
        <td><strong>Scrum Master</strong></td>
        <td>9A</td>
      </tr>
      <tr>
        <td><strong>Bozhidar Stanev</strong></td>
        <td><strong>Front-end Developer</strong></td>
        <td>9B</td>
      </tr>
      <tr>
        <td><strong>Rosica Velkova </strong></td>
        <td><strong>Back-end Developer</strong></td>
        <td>9A</td>
      </tr>
      <tr>
        <td><strong>Nikoleta Georgieva </strong></td>
        <td><strong>Quality Inspector</strong></td>
        <td>9A</td>
      </tr>
    </tbody>
  </table>

</body>
</html>
