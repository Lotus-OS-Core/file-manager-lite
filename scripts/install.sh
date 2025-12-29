#!/bin/bash

# Lotus-DIR Interactive Installation Script
# This script installs Lotus-DIR file manager on Linux systems

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
APP_NAME="Lotus-DIR"
INSTALL_DIR="/opt/Lotus-DIR"
BINARY_NAME="lotus-dir"
DESKTOP_FILE="/usr/share/applications/lotus.desktop"
ICON_DIR="/usr/share/icons/hicolor/scalable/apps"
# Get the script directory and project root directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(dirname "$SCRIPT_DIR")"

# Print functions
print_banner() {
    echo -e "${BLUE}"
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║                     Lotus-DIR Installer                       ║"
    echo "║          Linux Finder File Manager for Linux                  ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root or with sudo
check_root() {
    if [[ $EUID -ne 0 ]]; then
        if command -v sudo &> /dev/null; then
            print_info "This installation requires root privileges. Using sudo..."
            SUDO="sudo"
        else
            print_error "This installation requires root privileges."
            exit 1
        fi
    else
        SUDO=""
    fi
}

# Check for required dependencies
check_dependencies() {
    print_info "Checking for required dependencies..."
    
    local missing_deps=()
    
    # Check for Qt5
    if ! command -v qmake &> /dev/null && ! pkg-config --exists Qt5Core 2>/dev/null; then
        missing_deps+=("qtbase5-dev")
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("build-essential")
        missing_deps+=("cmake")
    fi
    
    # Check for g++
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("build-essential")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        print_warning "Missing dependencies: ${missing_deps[*]}"
        read -p "Do you want to install missing dependencies? (y/n): " install_deps
        if [[ "$install_deps" == "y" || "$install_deps" == "Y" ]]; then
            print_info "Installing dependencies..."
            $SUDO apt-get update
            $SUDO apt-get install -y "${missing_deps[@]}"
            print_success "Dependencies installed successfully!"
        else
            print_error "Cannot proceed without required dependencies."
            exit 1
        fi
    else
        print_success "All dependencies are satisfied!"
    fi
}

# Check if application is already installed
check_existing_installation() {
    if [[ -f "$INSTALL_DIR/$BINARY_NAME" ]]; then
        print_warning "$APP_NAME is already installed at $INSTALL_DIR"
        read -p "Do you want to reinstall? This will overwrite existing installation. (y/n): " reinstall
        if [[ "$reinstall" != "y" && "$reinstall" != "Y" ]]; then
            print_info "Installation cancelled."
            exit 0
        fi
    fi
}

# Ask for installation directory
ask_install_directory() {
    print_info "Default installation directory: $INSTALL_DIR"
    read -p "Enter installation directory (press Enter for default): " custom_dir
    
    if [[ -n "$custom_dir" ]]; then
        INSTALL_DIR="$custom_dir"
    fi
    
    # Ensure the directory is absolute path
    if [[ ! "$INSTALL_DIR" = /* ]]; then
        INSTALL_DIR="$(pwd)/$INSTALL_DIR"
    fi
    
    print_info "Installation directory: $INSTALL_DIR"
}

# Build the application
build_application() {
    print_info "Building Lotus-DIR..."
    
    # Create build directory
    local build_dir="$SOURCE_DIR/build"
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    if ! cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1; then
        print_error "CMake configuration failed!"
        exit 1
    fi
    
    # Build
    if ! make -j$(nproc) 2>&1; then
        print_error "Build failed!"
        exit 1
    fi
    
    print_success "Build completed successfully!"
    cd "$SOURCE_DIR"
}

# Install the application
install_application() {
    print_info "Installing $APP_NAME to $INSTALL_DIR..."
    
    # Create installation directory
    $SUDO mkdir -p "$INSTALL_DIR"
    
    # Copy binary
    $SUDO cp "$SOURCE_DIR/build/lotus-dir" "$INSTALL_DIR/"
    $SUDO chmod +x "$INSTALL_DIR/lotus-dir"
    
    # Copy resources
    $SUDO mkdir -p "$INSTALL_DIR/resources"
    $SUDO cp -r "$SOURCE_DIR/resources/"* "$INSTALL_DIR/resources/"
    
    # Create symlink in /usr/bin so command is available
    $SUDO ln -sf "$INSTALL_DIR/lotus-dir" /usr/bin/lotus-dir
    print_success "Created command symlink: /usr/bin/lotus-dir"
    
    # Install desktop file with correct Exec path
    $SUDO mkdir -p "$(dirname "$DESKTOP_FILE")"
    cat > /tmp/lotus.desktop << EOF
[Desktop Entry]
Name=Lotus-DIR
Comment=A Linux Finder File Manager for Linux
Exec=$INSTALL_DIR/lotus-dir
Icon=$INSTALL_DIR/resources/icons/app.svg
Terminal=false
Type=Application
Categories=System;FileManager;Utility;
StartupNotify=true
MimeType=inode/directory;
EOF
    $SUDO cp /tmp/lotus.desktop "$DESKTOP_FILE"
    $SUDO chmod +x "$DESKTOP_FILE"
    
    # Install icon to system icons
    $SUDO mkdir -p "$ICON_DIR"
    $SUDO cp "$SOURCE_DIR/resources/icons/app.svg" "$ICON_DIR/lotus-dir.svg"
    
    # Also install icons to application directory
    $SUDO mkdir -p "$INSTALL_DIR/resources/icons"
    $SUDO cp "$SOURCE_DIR/resources/icons/"*.svg "$INSTALL_DIR/resources/icons/"
    
    # Update desktop database
    if command -v update-desktop-database &> /dev/null; then
        $SUDO update-desktop-database
    fi
    
    # Update icon cache
    if command -v gtk-update-icon-cache &> /dev/null; then
        $SUDO gtk-update-icon-cache -f "$ICON_DIR" 2>/dev/null || true
    fi
    
    print_success "$APP_NAME installed successfully!"
}

# Create uninstall script
create_uninstaller() {
    print_info "Creating uninstall script..."
    
    # Create uninstall script in temp location first
    cat > /tmp/lotus-uninstall.sh << 'UNINSTALL_EOF'
#!/bin/bash

# Lotus-DIR Uninstaller

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

APP_NAME="Lotus-DIR"
INSTALL_DIR="$(dirname "$(readlink -f "$0")")"
BINARY_NAME="lotus-dir"
DESKTOP_FILE="/usr/share/applications/lotus.desktop"
ICON_DIR="/usr/share/icons/hicolor/scalable/apps"

print_banner() {
    echo -e "${YELLOW}"
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║                  Lotus-DIR Uninstaller                        ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_banner

# Check for root
if [[ $EUID -ne 0 ]]; then
    if command -v sudo &> /dev/null; then
        SUDO="sudo"
    else
        print_error "This uninstallation requires root privileges."
        exit 1
    fi
else
    SUDO=""
fi

print_warning "This will uninstall $APP_NAME from your system."
read -p "Are you sure you want to continue? (y/n): " confirm

if [[ "$confirm" != "y" && "$confirm" != "Y" ]]; then
    echo "Uninstallation cancelled."
    exit 0
fi

# Remove symlink from /usr/bin
if [[ -L /usr/bin/lotus-dir ]]; then
    $SUDO rm -f /usr/bin/lotus-dir
    print_success "Removed command symlink: /usr/bin/lotus-dir"
fi

# Remove installation directory
if [[ -d "$INSTALL_DIR" ]]; then
    rm -rf "$INSTALL_DIR"
    print_success "Removed installation directory: $INSTALL_DIR"
fi

# Remove desktop entry
if [[ -f "$DESKTOP_FILE" ]]; then
    $SUDO rm -f "$DESKTOP_FILE"
    print_success "Removed desktop entry"
fi

# Remove icon
if [[ -f "$ICON_DIR/lotus-dir.svg" ]]; then
    $SUDO rm -f "$ICON_DIR/lotus-dir.svg"
    print_success "Removed icon"
fi

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    $SUDO update-desktop-database 2>/dev/null || true
fi

print_success "$APP_NAME has been completely removed from your system!"
UNINSTALL_EOF
    
    # Copy to installation directory with sudo
    $SUDO cp /tmp/lotus-uninstall.sh "$INSTALL_DIR/uninstall.sh"
    $SUDO chmod +x "$INSTALL_DIR/uninstall.sh"
    rm /tmp/lotus-uninstall.sh
    
    print_success "Uninstall script created at $INSTALL_DIR/uninstall.sh"
}

# Final message
show_final_message() {
    echo ""
    echo -e "${GREEN}╔═══════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║                    Installation Complete!                     ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${BLUE}To launch Lotus-DIR:${NC}"
    echo "  • From application menu: Search for 'Lotus-DIR'"
    echo "  • From terminal: lotus-dir"
    echo ""
    echo -e "${BLUE}Installation details:${NC}"
    echo "  • Binary: $INSTALL_DIR/lotus-dir"
    echo "  • Resources: $INSTALL_DIR/resources"
    echo ""
    echo -e "${BLUE}To uninstall:${NC}"
    echo "  • Run: $INSTALL_DIR/uninstall.sh"
    echo "  • Or: lotus-dir --uninstall"
    echo ""
    print_success "Enjoy your new file manager!"
}

# Main installation process
main() {
    print_banner
    
    check_root
    check_dependencies
    check_existing_installation
    ask_install_directory
    build_application
    install_application
    create_uninstaller
    show_final_message
}

# Run main function
main "$@"
