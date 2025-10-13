#!/bin/bash

set -euo pipefail

# === CONFIGURATION GLOBALE ===
UNREAL_ROOT="/home/tb248952/Documents/Linux_Unreal_Engine_5.5.4"
MARKETPLACE_DIR="$UNREAL_ROOT/Engine/Plugins/Marketplace"
EXPORTER_SCRIPT="./package_plugin.sh"
TARGET_PLATFORM="Linux"

# Définis ici tes plugins : source path et nom
XDE_PHYSICS_PATH="/home/tb248952/Documents/toto/ros2_sandbox/Plugins/xsm3_unreal/Physics"
XDE_PHYSICS_NAME="XdePhysics"

SCORE_PATH="/home/tb248952/Documents/toto/ros2_sandbox/Plugins/score3_unreal"
SCORE_NAME="Score"

# === FONCTIONS ===
clean_package() {
    local pkg_dir=$1
    echo "🧹 Nettoyage dans $pkg_dir ..."
    find "$pkg_dir" -type f \( -name "*.cpp" -o -name "*.pdb" -o -name "*.sym" -o -name "*.obj" -o -name "*.log" \) -exec rm -v {} \; || true
    find "$pkg_dir" -type d \( -name "Intermediate" -o -name "Saved" -o -name "Build" \) -exec rm -rfv {} + || true
    rm -rf "$pkg_dir/Source/"*/Private || true
}

move_to_marketplace() {
    local staged_export=$1  # ex: /tmp/XdePhysics_export.abcd1234
    local plugin_name=$2
    local dest="$MARKETPLACE_DIR/$plugin_name"

    echo "📦 Déplacement de $staged_export vers $dest (marketplace)..."
    rm -rf "$dest"
    mkdir -p "$(dirname "$dest")"
    mv "$staged_export" "$dest"
}

export_plugin() {
    local plugin_path=$1
    local plugin_name=$2

    echo "=== Export de $plugin_name ==="
    local staging_dir
    # staging_dir=$(mktemp -d /tmp/${plugin_name}_export.XXXXXX)
    staging_dir="$(pwd)/plugin_exports/${plugin_name}"
    trap "echo '🧹 Cleanup staging $staging_dir'; rm -rf \"$staging_dir\"" RETURN

    "$EXPORTER_SCRIPT" -u "$UNREAL_ROOT" -p "$plugin_path" -n "$plugin_name" -t "$TARGET_PLATFORM" -o "$staging_dir"

    move_to_marketplace "$staging_dir" "$plugin_name"
}

# === EXECUTION ===

# 1. Export XdePhysics en premier
export_plugin "$XDE_PHYSICS_PATH" "$XDE_PHYSICS_NAME"

# 2. Puis Score (qui peut dépendre de XdePhysics déjà installé dans Marketplace)
export_plugin "$SCORE_PATH" "$SCORE_NAME"

# 3. Nettoyage final
# it must be done after building the packages because
# score needs xsm3 generated files that will be cleaned
clean_package "$MARKETPLACE_DIR/$XDE_PHYSICS_NAME"
clean_package "$MARKETPLACE_DIR/$SCORE_NAME"

echo "✅ Tous les plugins ont été exportés, déplacés et nettoyés."
