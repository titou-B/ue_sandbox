#!/bin/bash

set -e

usage() {
    cat <<EOF
Usage: $0 [options]

Options:
  -u <unreal_root>     Racine de l'engine Unreal (par défaut: /home/tb248952/Documents/Linux_Unreal_Engine_5.5.4)
  -p <plugin_path>     Chemin vers le dossier du plugin source (contenant .uplugin)
  -n <plugin_name>     Nom du plugin (ex: XdePhysics)
  -t <target_platform> Plateforme cible (par défaut: Linux)
  -o <output_dir>      Répertoire de sortie (par défaut: ../plugin_exports/<plugin_name> sous le projet courant)
  -h                   Affiche cette aide

Exemple :
  $0 -u /chemin/UE5 -p /mon/projet/Plugins/xsm3_unreal/Physics -n XdePhysics
EOF
    exit 1
}

# === DEFAULTS ===
UNREAL_ROOT="/home/tb248952/Documents/Linux_Unreal_Engine_5.5.4"
TARGET_PLATFORM="Linux"
OUTPUT_DIR=""  # sera calculé si pas fourni

# === PARSE ARGS ===
while getopts "u:p:n:t:o:h" opt; do
    case "$opt" in
        u) UNREAL_ROOT="$OPTARG" ;;
        p) PLUGIN_PATH="$OPTARG" ;;
        n) PLUGIN_NAME="$OPTARG" ;;
        t) TARGET_PLATFORM="$OPTARG" ;;
        o) OUTPUT_DIR="$OPTARG" ;;
        h|*) usage ;;
    esac
done

# vérification minimale
if [ -z "${PLUGIN_PATH:-}" ] || [ -z "${PLUGIN_NAME:-}" ]; then
    echo "❌ Il faut fournir au moins -p <plugin_path> et -n <plugin_name>"
    usage
fi

# === DERIVED ===
if [ -z "$OUTPUT_DIR" ]; then
    OUTPUT_DIR="$(pwd)/${PLUGIN_NAME}"
fi
UAT_PATH="$UNREAL_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
UPLUGIN_FILE="$PLUGIN_PATH/${PLUGIN_NAME}.uplugin"
BIN_PATH="$PLUGIN_PATH/BinXsm"
BIN_BACKUP="$PLUGIN_PATH/../BinXsm_backup"

# === CHECKS ===
if [ ! -f "$UAT_PATH" ]; then
    echo "❌ UAT script introuvable à : $UAT_PATH"
    exit 1
fi

if [ ! -f "$UPLUGIN_FILE" ]; then
    echo "❌ Fichier .uplugin introuvable à : $UPLUGIN_FILE"
    exit 1
fi

# === STEP 0: Backup BinXsm ===
echo "🔁 Déplacement de BinXsm vers BinXsm_backup..."
mv "$BIN_PATH" "$BIN_BACKUP"

# === CLEANUP ===
cleanup() {
    echo "🗑 Suppression du BinXsm temporaire (symlinks résolus)..."
    rm -rf "$BIN_PATH"

    echo "↩️ Restauration de BinXsm depuis backup..."
    mv "$BIN_BACKUP" "$BIN_PATH" 2>/dev/null || true
    rm -rf "$BIN_BACKUP"
}
trap cleanup EXIT

# === STEP 1: Recréation de BinXsm avec symlinks résolus ===
echo "🛠 Recréation de BinXsm avec symlinks résolus..."
mkdir -p "$BIN_PATH/Linux"

for sofile in "$BIN_BACKUP/Linux/"*.so*; do
    base=$(basename "$sofile")
    if [ -L "$sofile" ]; then
        realfile=$(readlink -f "$sofile")
        echo "→ $base est un symlink, résolution vers $realfile"
        cp --remove-destination "$realfile" "$BIN_PATH/Linux/$base"
    else
        cp --remove-destination "$sofile" "$BIN_PATH/Linux/$base"
    fi
done

# === STEP 2: Packaging ===
echo "🚀 Packaging du plugin dans : $OUTPUT_DIR"
"$UAT_PATH" BuildPlugin \
    -Plugin="$UPLUGIN_FILE" \
    -Package="$OUTPUT_DIR" \
    -TargetPlatforms="$TARGET_PLATFORM" \
    -Rocket

echo "✅ Plugin packagé avec succès dans : $OUTPUT_DIR"
