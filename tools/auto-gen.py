#!/usr/bin/env python3
"""
RGB565 Palette & Index Map Generator
=====================================

Batch-converts a folder tree of images into:
  - one shared palette.txt   -> up to 255 unique RGB565 colours (2 bytes each)
  - one <image>.txt per image -> a WxH array of palette indices (0-255)

Rules implemented:
  - Index 0 is reserved for "transparent" pixels.
    A pixel counts as transparent if its alpha is <= 50% (alpha <= 127/255).
    A pixel with alpha > 50% is treated as fully opaque (its own transparency
    is ignored) and gets mapped to a palette colour.
  - The palette is shared across ALL selected images (they all draw from the
    same 255-colour table), because the target hardware can only hold one
    255-colour + 1 transparent-index table (256 total).
  - If more than 255 unique colours are found across all images, colours are
    reduced/merged (median-cut quantization + nearest-colour remapping) so
    the final palette never exceeds 255 entries.
  - Every colour is stored/matched in RGB565 (5-6-5 bit) precision, since
    that's the format the target actually uses.

Requirements:
    pip install pillow numpy --break-system-packages

Usage:
    python3 rgb565_palette_tool.py
"""

import os
import threading
import queue
import traceback
from collections import Counter

import numpy as np
from PIL import Image

import tkinter as tk
from tkinter import ttk, filedialog, messagebox

# --------------------------------------------------------------------------
# Config
# --------------------------------------------------------------------------

IMAGE_EXTENSIONS = {".png", ".bmp", ".gif", ".tga", ".tiff", ".tif", ".jpg", ".jpeg", ".webp"}
MAX_PALETTE_COLOURS = 255          # colours 1..255 (index 0 reserved for transparent)
ALPHA_OPAQUE_THRESHOLD = 127       # alpha > this  => treated opaque (ignore its transparency)
                                    # alpha <= this => treated fully transparent -> index 0
QUANTIZE_SAMPLE_CAP = 200_000      # cap on synthetic pixel count fed to the quantizer
NEAREST_CHUNK = 2000               # chunk size for nearest-colour search (memory control)


# --------------------------------------------------------------------------
# RGB565 helpers
# --------------------------------------------------------------------------

def rgb_to_565(r, g, b):
    """Pack 8-bit r,g,b into a 16-bit RGB565 value."""
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    return (r5 << 11) | (g6 << 5) | b5


def rgb565_to_rgb(v):
    """Expand a 16-bit RGB565 value back to an approximate 8-bit (r,g,b)."""
    r5 = (v >> 11) & 0x1F
    g6 = (v >> 5) & 0x3F
    b5 = v & 0x1F
    r = (r5 * 255 + 15) // 31
    g = (g6 * 255 + 31) // 63
    b = (b5 * 255 + 15) // 31
    return (r, g, b)


def rgb_array_to_565(arr_r, arr_g, arr_b):
    """Vectorised RGB -> RGB565 for numpy uint arrays."""
    r5 = (arr_r.astype(np.uint32) >> 3) & 0x1F
    g6 = (arr_g.astype(np.uint32) >> 2) & 0x3F
    b5 = (arr_b.astype(np.uint32) >> 3) & 0x1F
    return (r5 << 11) | (g6 << 5) | b5


# --------------------------------------------------------------------------
# Core processing
# --------------------------------------------------------------------------

def find_image_files(root_folder):
    files = []
    for dirpath, _dirnames, filenames in os.walk(root_folder):
        for fn in filenames:
            if os.path.splitext(fn)[1].lower() in IMAGE_EXTENSIONS:
                files.append(os.path.join(dirpath, fn))
    return sorted(files)


def compute_pixel_565_and_mask(path):
    """Load an image and return (r565_array HxW uint32, opaque_mask HxW bool)."""
    img = Image.open(path).convert("RGBA")
    arr = np.array(img)
    r, g, b, a = arr[:, :, 0], arr[:, :, 1], arr[:, :, 2], arr[:, :, 3]
    r565 = rgb_array_to_565(r, g, b)
    opaque_mask = a > ALPHA_OPAQUE_THRESHOLD
    return r565, opaque_mask


def build_colour_counts(paths, log):
    """Pass 1: scan every image, count opaque-pixel colours (in RGB565 space)."""
    counts = Counter()
    for i, path in enumerate(paths):
        try:
            r565, opaque_mask = compute_pixel_565_and_mask(path)
        except Exception as e:
            log(f"  ! Skipped (could not read): {path} ({e})")
            continue
        opaque_vals = r565[opaque_mask]
        if opaque_vals.size:
            vals, freq = np.unique(opaque_vals, return_counts=True)
            counts.update(dict(zip(vals.tolist(), freq.tolist())))
        log(f"  [{i+1}/{len(paths)}] scanned {os.path.basename(path)} "
            f"({len(counts)} unique colours so far)")
    return counts


def reduce_palette(counts, log):
    """Return a list of up to MAX_PALETTE_COLOURS RGB565 ints."""
    unique_vals = list(counts.keys())
    if len(unique_vals) <= MAX_PALETTE_COLOURS:
        log(f"  {len(unique_vals)} unique colours <= {MAX_PALETTE_COLOURS}; no reduction needed.")
        return sorted(unique_vals)

    log(f"  {len(unique_vals)} unique colours > {MAX_PALETTE_COLOURS}; "
        f"merging close colours via median-cut quantization...")

    total = sum(counts.values())
    scale = min(1.0, QUANTIZE_SAMPLE_CAP / max(total, 1))

    sample_pixels = []
    for v, cnt in counts.items():
        n = max(1, int(round(cnt * scale)))
        sample_pixels.append((rgb565_to_rgb(v), n))

    flat = []
    for rgb, n in sample_pixels:
        flat.extend([rgb] * n)
        if len(flat) > QUANTIZE_SAMPLE_CAP * 1.2:
            break
    flat_arr = np.array(flat, dtype=np.uint8)
    w = max(1, len(flat_arr))
    sample_img = Image.fromarray(flat_arr.reshape(1, w, 3), "RGB")

    try:
        method = Image.Quantize.MEDIANCUT
    except AttributeError:
        method = Image.MEDIANCUT  # older Pillow

    quantized = sample_img.quantize(colors=MAX_PALETTE_COLOURS, method=method)
    pal = quantized.getpalette()[: MAX_PALETTE_COLOURS * 3]

    seen = set()
    palette = []
    for i in range(0, len(pal), 3):
        r, g, b = pal[i], pal[i + 1], pal[i + 2]
        v = rgb_to_565(r, g, b)
        if v not in seen:
            seen.add(v)
            palette.append(v)

    log(f"  Reduced to {len(palette)} palette colours.")
    return palette


def build_mapping(counts, palette, log):
    """Map every original RGB565 colour seen -> nearest palette index (1-based)."""
    palette_set = {v: i + 1 for i, v in enumerate(palette)}
    palette_rgb = np.array([rgb565_to_rgb(v) for v in palette], dtype=np.int32)  # (P,3)

    mapping = {}
    to_search = []
    for v in counts.keys():
        if v in palette_set:
            mapping[v] = palette_set[v]
        else:
            to_search.append(v)

    if to_search:
        log(f"  Nearest-colour matching {len(to_search)} merged colours...")
        to_search_arr = np.array(to_search, dtype=np.int64)
        for start in range(0, len(to_search), NEAREST_CHUNK):
            chunk_vals = to_search_arr[start:start + NEAREST_CHUNK]
            chunk_rgb = np.array([rgb565_to_rgb(int(v)) for v in chunk_vals], dtype=np.int32)
            diff = chunk_rgb[:, None, :] - palette_rgb[None, :, :]
            dist = np.sum(diff * diff, axis=2)
            nearest = np.argmin(dist, axis=1)
            for v, idx in zip(chunk_vals.tolist(), nearest.tolist()):
                mapping[v] = idx + 1

    return mapping  # dict: original_rgb565 -> palette index (1..255)


def write_palette_file(palette, out_folder, log):
    path = os.path.join(out_folder, "palette.txt")
    with open(path, "w") as f:
        f.write("# Shared palette (RGB565, 2 bytes per colour)\n")
        f.write("# index 0 is reserved for fully/mostly-transparent pixels\n")
        f.write("# index  hex565  r   g   b\n")
        f.write("0\tTRANSPARENT\n")
        for i, v in enumerate(palette, start=1):
            r, g, b = rgb565_to_rgb(v)
            f.write(f"{i}\t0x{v:04X}\t{r}\t{g}\t{b}\n")
    log(f"  Wrote shared palette: {path} ({len(palette)} colours + 1 transparent)")
    return path


def write_index_file(path_in, out_folder, base_folder, lookup, log):
    r565, opaque_mask = compute_pixel_565_and_mask(path_in)
    idx = lookup[r565]
    idx = np.where(opaque_mask, idx, 0).astype(np.uint16)
    h, w = idx.shape

    rel = os.path.relpath(path_in, base_folder)
    rel_no_ext = os.path.splitext(rel)[0]
    out_path = os.path.join(out_folder, rel_no_ext + ".txt")
    os.makedirs(os.path.dirname(out_path), exist_ok=True)

    with open(out_path, "w") as f:
        f.write(f"{w} {h}\n")
        for row in idx:
            f.write(",".join(map(str, row.tolist())))
            f.write("\n")

    log(f"  Wrote index map: {out_path}  ({w}x{h})")
    return out_path


def run_pipeline(input_folder, output_folder, log, progress_cb):
    paths = find_image_files(input_folder)
    if not paths:
        log("No image files found in the selected folder (or its subfolders).")
        return
    log(f"Found {len(paths)} image file(s).")

    log("Pass 1/3: scanning colours across all images...")
    counts = build_colour_counts(paths, log)
    progress_cb(0.4)

    if not counts:
        log("No opaque pixels found in any image; nothing to do.")
        return

    log("Pass 2/3: building shared palette (max 255 colours)...")
    palette = reduce_palette(counts, log)
    mapping = build_mapping(counts, palette, log)

    lookup = np.zeros(65536, dtype=np.uint16)
    for v, i in mapping.items():
        lookup[v] = i

    os.makedirs(output_folder, exist_ok=True)
    write_palette_file(palette, output_folder, log)
    progress_cb(0.55)

    log("Pass 3/3: writing per-image index maps...")
    n = len(paths)
    for i, p in enumerate(paths):
        try:
            write_index_file(p, output_folder, input_folder, lookup, log)
        except Exception as e:
            log(f"  ! Failed on {p}: {e}")
        progress_cb(0.55 + 0.45 * (i + 1) / n)

    log("Done.")


# --------------------------------------------------------------------------
# GUI
# --------------------------------------------------------------------------

class App:
    def __init__(self, root):
        self.root = root
        root.title("RGB565 Palette & Index Map Generator")
        root.geometry("720x520")

        self.input_folder = tk.StringVar(value="")
        self.output_folder = tk.StringVar(value="")
        self.msg_queue = queue.Queue()

        pad = {"padx": 10, "pady": 6}

        frm_top = ttk.Frame(root)
        frm_top.pack(fill="x", **pad)

        ttk.Label(frm_top, text="Parent image folder (all subfolders included):").grid(
            row=0, column=0, sticky="w")
        ttk.Entry(frm_top, textvariable=self.input_folder).grid(
            row=1, column=0, sticky="we", padx=(0, 6))
        ttk.Button(frm_top, text="Browse...", command=self.pick_input).grid(row=1, column=1)

        ttk.Label(frm_top, text="Output folder:").grid(row=2, column=0, sticky="w", pady=(10, 0))
        ttk.Entry(frm_top, textvariable=self.output_folder).grid(
            row=3, column=0, sticky="we", padx=(0, 6))
        ttk.Button(frm_top, text="Browse...", command=self.pick_output).grid(row=3, column=1)

        frm_top.columnconfigure(0, weight=1)

        frm_rules = ttk.LabelFrame(root, text="Rules in effect")
        frm_rules.pack(fill="x", **pad)
        ttk.Label(
            frm_rules,
            justify="left",
            text=(
                "• Max 255 colours, shared across ALL selected images, stored as RGB565 (2 bytes).\n"
                "• Index 0 = reserved for transparent pixels.\n"
                "• Alpha > 50% -> treated fully opaque (colour used as-is).\n"
                "• Alpha <= 50% (incl. fully transparent) -> pixel index = 0.\n"
                "• If more than 255 colours exist, close colours are merged automatically."
            ),
        ).pack(anchor="w", padx=8, pady=6)

        self.run_btn = ttk.Button(root, text="Run", command=self.start_run)
        self.run_btn.pack(pady=(0, 6))

        self.progress = ttk.Progressbar(root, mode="determinate", maximum=100)
        self.progress.pack(fill="x", padx=10, pady=(0, 6))

        frm_log = ttk.Frame(root)
        frm_log.pack(fill="both", expand=True, padx=10, pady=(0, 10))
        self.log_text = tk.Text(frm_log, height=15, wrap="word", state="disabled")
        scrollbar = ttk.Scrollbar(frm_log, command=self.log_text.yview)
        self.log_text.configure(yscrollcommand=scrollbar.set)
        self.log_text.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.root.after(100, self.poll_queue)

    def pick_input(self):
        d = filedialog.askdirectory(title="Select parent image folder")
        if d:
            self.input_folder.set(d)

    def pick_output(self):
        d = filedialog.askdirectory(title="Select output folder")
        if d:
            self.output_folder.set(d)

    def log(self, message):
        self.msg_queue.put(("log", message))

    def progress_cb(self, fraction):
        self.msg_queue.put(("progress", fraction))

    def poll_queue(self):
        try:
            while True:
                kind, payload = self.msg_queue.get_nowait()
                if kind == "log":
                    self.log_text.configure(state="normal")
                    self.log_text.insert("end", payload + "\n")
                    self.log_text.see("end")
                    self.log_text.configure(state="disabled")
                elif kind == "progress":
                    self.progress["value"] = payload * 100
                elif kind == "done":
                    self.run_btn.configure(state="normal")
                elif kind == "error":
                    self.run_btn.configure(state="normal")
                    messagebox.showerror("Error", payload)
        except queue.Empty:
            pass
        self.root.after(100, self.poll_queue)

    def start_run(self):
        in_folder = self.input_folder.get().strip()
        out_folder = self.output_folder.get().strip()
        if not in_folder or not os.path.isdir(in_folder):
            messagebox.showwarning("Missing input", "Please select a valid parent image folder.")
            return
        if not out_folder:
            messagebox.showwarning("Missing output", "Please select an output folder.")
            return

        self.log_text.configure(state="normal")
        self.log_text.delete("1.0", "end")
        self.log_text.configure(state="disabled")
        self.progress["value"] = 0
        self.run_btn.configure(state="disabled")

        t = threading.Thread(target=self._worker, args=(in_folder, out_folder), daemon=True)
        t.start()

    def _worker(self, in_folder, out_folder):
        try:
            run_pipeline(in_folder, out_folder, self.log, self.progress_cb)
            self.msg_queue.put(("progress", 1.0))
        except Exception:
            self.msg_queue.put(("error", traceback.format_exc()))
        finally:
            self.msg_queue.put(("done", None))


def main():
    root = tk.Tk()
    App(root)
    root.mainloop()


if __name__ == "__main__":
    main()