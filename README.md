# Elite-RC-Telemetry-Dashboard
Elite RC Telemetry is a sophisticated, web-based dashboard designed for the advanced performance monitoring of remote-controlled vehicles.

# Elite RC Telemetry Dashboard

## Description

Elite RC Telemetry is a sophisticated, web-based dashboard designed for the advanced performance monitoring of remote-controlled vehicles. It provides a real-time interface to visualize critical telemetry data, such as velocity, acceleration, G-force, and distance. Built with modern web technologies, this dashboard offers a seamless experience for RC enthusiasts and engineers to track, analyze, and save performance data from their vehicles.

The application is a **single, self-contained HTML file** with zero dependencies, requiring no server or installation. It can connect directly to an RC car via Web Bluetooth for live data streaming. For users without a compatible device, it includes an interactive "Demo Mode" with a physics-based car simulator that can be controlled with a keyboard or a virtual joystick on touch devices.

The dashboard features a dynamic and customizable graphing system powered by Chart.js, allowing users to toggle different data streams. Sessions can be saved, reloaded for later analysis, and exported to CSV or PNG formats. The user interface is fully responsive, featuring a sleek dark/light theme toggle and intuitive controls.

## Features

-   **Real-Time Data Streaming**: Connects to your RC vehicle using the Web Bluetooth API for live, low-latency telemetry data.
-   **Interactive Demo Mode**: A built-in car simulator with physics, keyboard (`WASD`/`Arrows`), and virtual joystick controls for demonstration and testing.
-   **Advanced Data Visualization**: A powerful and responsive charting system displays Velocity, Acceleration, Distance, Displacement, and Jerk.
-   **Live Gauges**: Visual indicators for Throttle, Steering, and G-Force provide an at-a-glance overview of the vehicle's state in real-time.
-   **Comprehensive Session Statistics**: Tracks and displays key performance metrics like Max Velocity, Average Velocity, Total Distance, and Session Duration.
-   **Lap Timer**: Manually mark laps with a dedicated button to track and compare lap times during a session.
-   **Session Management**:
    -   **Save**: Store complete telemetry sessions, including all graph data and stats, in your browser's local storage.
    -   **Load**: Review and analyze past sessions at any time with a clean, searchable interface.
    -   **Delete**: Easily manage and remove old sessions.
-   **Data Export**:
    -   Export full session data as a `.csv` file for external analysis in tools like Excel or Google Sheets.
    -   Export the current graph view as a high-quality `.png` image.
-   **Customizable Interface**:
    -   Switch between metric (`m/s`, `km/h`) and imperial (`mph`) units on the fly.
    -   Toggle between a sleek dark mode and a clean light mode.
    -   Enter a fullscreen graph view for detailed, unobstructed analysis.
-   **Fully Responsive**: The layout is optimized for desktops, tablets, and mobile devices.
-   **Zero Dependencies**: A single, self-contained HTML file that runs in any modern browser without needing a server, build steps, or installation.

## Getting Started

No installation or build steps are required. Simply open the `vt graph.html` file in a modern web browser that supports the Web Bluetooth API.

**Recommended Browsers:**
*   Google Chrome (Desktop & Android)
*   Microsoft Edge (Desktop)

### Usage

1.  **Open the File**: Download the `vt graph.html` file and open it in a compatible web browser.
2.  **Connect to a Device**:
    *   Power on your RC car equipped with the compatible firmware.
    *   Click the **"🔗 Connect"** button.
    *   Select your device from the Bluetooth pop-up window.
    *   Once connected, you will see live data streaming to the dashboard.
3.  **Use Demo Mode**:
    *   If you don't have a compatible device, click the **"🎮 Demo Mode"** button.
    *   Control the simulated car using:
        *   **Desktop**: `WASD` or `Arrow Keys` for driving, `Q`/`E` for turn signals.
        *   **Mobile**: A virtual joystick will appear on-screen for steering and throttle.
4.  **Analyze Data**:
    *   Watch the live velocity and session stats update in real-time.
    *   The graph shows a live 15-second scrolling preview. Click the **View Full Session** button (bar chart icon) to see all data from the start of the session.
    *   Use the toggles below the graph to show or hide different data sets (Velocity, Accel, Distance, etc.).
    *   Click the **Fullscreen** button to expand the graph to fill the entire screen for better visibility.
5.  **Save & Load Sessions**:
    *   During or after a session, click the **"💾 Save"** button to store your data.
    *   Click the **"📂 Load"** button to view, load, or delete previously saved sessions.

## Technologies Used

-   **HTML5**: For the semantic structure of the web page.
-   **Tailwind CSS (via CDN)**: For a modern, utility-first CSS framework that enables a responsive design without a build step.
-   **JavaScript (ES6+)**: For all application logic, including UI interactions, data processing, physics simulation, and state management, written in a modular, class-based structure.
-   **Chart.js**: For creating beautiful, responsive, and interactive charts.
-   **Web Bluetooth API**: For wireless communication with the RC vehicle.
