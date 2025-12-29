# Lotus-DIR

A Light-weight Finder file manager for Linux, built with C++ and Qt5.

![Lotus-DIR Screenshot](https://via.placeholder.com/800x500?text=Lotus-DIR+File+Manager)

## Features

- **Linux Finder Interface**: Clean, modern design for Linux Finder/File-manager
- **Dark/Light Theme Support**: Toggle between themes with a single click
- **Sidebar Navigation**: Quick access to favorites, locations, and common directories
- **Multiple View Modes**: Icon view and list view with detailed file information
- **File Operations**: Copy, paste, delete, rename, and move files
- **Search Functionality**: Filter files by name in the current directory
- **Breadcrumb Navigation**: Easy navigation through file paths
- **Context Menu**: Right-click menu for quick file operations
- **Preview Panel**: View file metadata and information

## Requirements

- Linux operating system
- Qt5 (Qt5Widgets, Qt5Core)
- CMake 3.10 or higher
- C++17 compiler (g++ or clang++)
- Build tools (make)

## Installation

### Quick Install

1. Navigate to the project directory:
   ```bash
   cd Lotus-DIR
   ```

2. Make the install script executable:
   ```bash
   chmod +x scripts/install.sh
   ```

3. Run the installer:
   ```bash
   ./scripts/install.sh
   ```

The installer will:
- Check for required dependencies (Qt5, CMake, build tools)
- Prompt to install missing dependencies if needed
- Ask for installation directory (default: /opt/Lotus-DIR)
- Build the application
- Install the binary and resources
- Create desktop entry and icon
- Generate uninstall script

### Manual Build

If you prefer to build manually:

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Install
sudo make install
```

## Usage

### Launching Lotus-DIR

After installation, you can launch Lotus-DIR in several ways:

1. **From Application Menu**: Search for "Lotus-DIR" in your desktop environment's application launcher
2. **From Terminal**: Run `lotus-dir` command
3. **From Installation Directory**: Execute `/opt/Lotus-DIR/lotus-dir`

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Alt+Left` | Navigate back |
| `Alt+Right` | Navigate forward |
| `Backspace` | Navigate up |
| `Ctrl+C` | Copy selected files |
| `Ctrl+V` | Paste files |
| `Delete` | Delete selected files |
| `F2` | Rename selected file |
| `Ctrl+F` | Focus search bar |

### Mouse Operations

- **Single Click**: Select file/folder
- **Double Click**: Open file/folder
- **Right Click**: Show context menu
- **Drag & Drop**: Move files between directories

## Uninstallation

### Using Uninstall Script

If you installed using the install script:

```bash
/opt/Lotus-DIR/uninstall.sh
```

### Using Standalone Uninstall Script

```bash
chmod +x scripts/uninstall.sh
./scripts/uninstall.sh
```

### Manual Uninstallation

```bash
# Remove binary
sudo rm /opt/Lotus-DIR/lotus-dir

# Remove desktop entry
sudo rm /usr/share/applications/lotus.desktop

# Remove icon
sudo rm /usr/share/icons/hicolor/scalable/apps/lotus-dir.svg

# Remove config directory
rm -rf ~/.config/Lotus-DIR
```

## Project Structure

```
Lotus-DIR/
├── CMakeLists.txt          # CMake build configuration
├── src/
│   ├── main.cpp            # Application entry point
│   ├── mainwindow.h/cpp    # Main window implementation
│   ├── sidebar.h/cpp       # Sidebar navigation widget
│   └── filemodel.h/cpp     # Custom file model
├── resources/
│   ├── icons/              # SVG icons for the application
│   ├── qss/                # Qt Style Sheets (light/dark themes)
│   ├── icons.qrc           # Qt resource compilation file
│   └── lotus.desktop       # Desktop entry file
└── scripts/
    ├── install.sh          # Interactive installation script
    └── uninstall.sh        # Uninstallation script
```

## Customization

### Themes

Lotus-DIR comes with two built-in themes:

- **Light Theme**: Default Linux Finder appearance
- **Dark Theme**: Linux Dark Mode appearance

Toggle between themes using the toolbar button or the View menu.

### Adding Custom Icons

Place custom SVG icons in the `resources/icons/` directory and update `resources/icons.qrc` to include them.

### Custom Styles

Modify the stylesheets in `resources/qss/light.qss` and `resources/qss/dark.qss` to customize the appearance.

## Development

### Building for Development

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Debug Mode

Run with debugging enabled:
```bash
./build/lotus-dir
```

### Adding New Features

1. Fork the repository
2. Create a feature branch
3. Implement your feature
4. Test thoroughly
5. Submit a pull request

## Troubleshooting

### Common Issues

**Application won't start**
- Check if Qt5 libraries are installed: `ldd /opt/Lotus-DIR/lotus-dir`
- Install missing dependencies: `sudo apt-get install qtbase5-dev`

**Missing icons**
- Verify icons are installed: `ls /opt/Lotus-DIR/resources/icons/`
- Check Qt resource compilation

**Dark mode not working**
- Ensure stylesheets are installed: `ls /opt/Lotus-DIR/resources/qss/`

### Getting Help

- Open an issue on GitHub
- Check the wiki for detailed documentation
- Review logs: `journalctl -f` (while running from terminal)

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Built in C++
- Built with Qt5 framework
- Icons based on Feather Icons

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting pull requests.

---

**Lotus-DIR** - Simple light-weight file manager
