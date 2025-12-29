#!/bin/bash

# Lotus-DIR Uninstaller

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
APP_NAME="Lotus-DIR"
BINARY_NAME="lotus-dir"
DESKTOP_FILE="/usr/share/applications/lotus.desktop"
ICON_DIR="/usr/share/icons/hicolor/scalable/apps"

# Print functions
print_banner() {
    echo -e "${YELLOW}"
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║                  Lotus-DIR Uninstaller                        ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
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

# Detect installation directory
if [[ -f "/opt/Lotus-DIR/lotus-dir" ]]; then
    INSTALL_DIR="/opt/Lotus-DIR"
elif [[ -f "$HOME/.local/bin/lotus-dir" ]]; then
    INSTALL_DIR="$HOME/.local"
elif [[ -f "/usr/local/bin/lotus-dir" ]]; then
    INSTALL_DIR="/usr/local"
else
    # Ask user for installation directory
    read -p "Enter Lotus-DIR installation directory: " INSTALL_DIR
    if [[ ! -d "$INSTALL_DIR" ]]; then
        print_error "Directory does not exist: $INSTALL_DIR"
        exit 1
    fi
fi

print_warning "This will uninstall $APP_NAME from your system."
print_warning "Installation directory: $INSTALL_DIR"
echo ""
read -p "Are you sure you want to continue? (y/n): " confirm

if [[ "$confirm" != "y" && "$confirm" != "Y" ]]; then
    echo "Uninstallation cancelled."
    exit 0
fi

# Check for root
if [[ $EUID -ne 0 ]]; then
    if command -v sudo &> /dev/null; then
        SUDO="sudo"
        print_info "Using sudo for system operations..."
    else
        print_error "This uninstallation requires root privileges."
        exit 1
    fi
else
    SUDO=""
fi

# Remove installation directory
if [[ -d "$INSTALL_DIR/Lotus-DIR" ]]; then
    rm -rf "$INSTALL_DIR/Lotus-DIR"
    print_success "Removed installation directory: $INSTALL_DIR/Lotus-DIR"
elif [[ -d "$INSTALL_DIR" ]]; then
    rm -rf "$INSTALL_DIR"
    print_success "Removed installation directory: $INSTALL_DIR"
fi

# Remove symlink from /usr/bin
if [[ -L "/usr/bin/lotus-dir" ]]; then
    $SUDO rm -f "/usr/bin/lotus-dir"
    print_success "Removed command symlink: /usr/bin/lotus-dir"
fi

# Remove binary from common locations
BINARY_REMOVED=false
for dir in "/usr/bin" "/usr/local/bin" "$HOME/.local/bin"; do
    if [[ -f "$dir/lotus-dir" || -L "$dir/lotus-dir" ]]; then
        $SUDO rm -f "$dir/lotus-dir"
        print_success "Removed binary from: $dir"
        BINARY_REMOVED=true
    fi
done

# Remove desktop entry
if [[ -f "$DESKTOP_FILE" ]]; then
    $SUDO rm -f "$DESKTOP_FILE"
    print_success "Removed desktop entry: $DESKTOP_FILE"
fi

# Remove icon
if [[ -f "$ICON_DIR/lotus-dir.svg" ]]; then
    $SUDO rm -f "$ICON_DIR/lotus-dir.svg"
    print_success "Removed icon: $ICON_DIR/lotus-dir.svg"
fi

# Remove config directory
if [[ -d "$HOME/.config/Lotus-DIR" ]]; then
    rm -rf "$HOME/.config/Lotus-DIR"
    print_success "Removed configuration directory: $HOME/.config/Lotus-DIR"
fi

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    $SUDO update-desktop-database 2>/dev/null || true
fi

# Update icon cache
if command -v gtk-update-icon-cache &> /dev/null; then
    $SUDO gtk-update-icon-cache -f "$ICON_DIR" 2>/dev/null || true
fi

echo ""
echo -e "${GREEN}╔═══════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║                 Uninstallation Complete!                      ║${NC}"
echo -e "${GREEN}╚═══════════════════════════════════════════════════════════════╝${NC}"
echo ""
print_success "$APP_NAME has been completely removed from your system!"
echo ""
echo "Note: Some configuration files may remain in:"
echo "  - $HOME/.config/Lotus-DIR"
echo ""
