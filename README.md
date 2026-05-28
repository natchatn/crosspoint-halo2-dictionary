# Xteink X4 Thai Dictionary Lookup System

A custom English → Thai dictionary firmware extension for the Xteink X4 e-reader.

This project adds an integrated in-reader dictionary system with word highlighting, instant lookup, smart text normalization, and optimized large-dictionary search directly inside the EPUB reader.

Built for embedded e-ink hardware with a strong focus on performance, low memory usage, and smooth reading experience.

---

# Project Lineage & Credits

This project is based on the following open-source projects:

* Crosspoint firmware
* crosspoint-halo2-custom by kocha01

Original firmware and multilingual infrastructure belong to their respective authors.

This repository extends the original firmware with a custom dictionary engine and EPUB lookup system developed by Natchanon.

Major additions include:

* English → Thai dictionary integration
* Embedded binary-search dictionary engine
* Smart lookup word cleaning
* Large dictionary optimization
* EPUB word selection workflow
* Highlight persistence improvements
* Reader UX enhancements

This project continues under the original MIT License.

---

# Features

## In-Reader Word Selection

* Long press `Confirm` to enter highlight mode
* Navigate between words using hardware buttons
* Highlight individual words directly on the page
* Selection persists after dictionary lookup

## Instant Dictionary Lookup

* Press `Confirm` on a selected word
* Opens Thai dictionary popup/activity
* Displays Thai meaning from `en_th.txt`

## Smart Word Cleaning

The lookup system automatically cleans words before searching.

### Case normalization

```txt
Strength -> strength
VIOLET -> violet
```

### Punctuation removal

```txt
strength. -> strength
strength," -> strength
“Damn -> damn
rucksack,” -> rucksack
```

### Handles smart quotes and unicode punctuation

Supported cleanup includes:

* `"`
* `'`
* `“ ”`
* `‘ ’`
* commas
* periods
* semicolons
* brackets
* symbols

---

# Large Dictionary Support

Optimized for very large dictionary files.

## Previous approach

* Loaded entire dictionary into RAM
* Poor scalability on large files
* Unstable deep-file lookups

## Current approach

* File-based binary search
* SD-card streamed lookup
* No full-file RAM loading
* Supports multi-megabyte dictionaries

---

# Fast Binary Search Lookup

The dictionary engine:

* Uses byte-level binary search
* Seeks directly into the dictionary file
* Reads only nearby lines
* Minimizes SD card reads
* Improves performance on huge dictionaries

---

# Dictionary Format

Dictionary file location:

```txt
/crosspoint/en_th.txt
```

Format:

```txt
word|meaning
```

Example:

```txt
game|เกม
idea|ความคิด
rucksack|เป้สะพายหลัง
```

IMPORTANT:

The dictionary file MUST be sorted alphabetically for binary search to function correctly.

---

# Reader Controls

## Highlight Mode

| Action               | Button       |
| -------------------- | ------------ |
| Enter highlight mode | Hold Confirm |
| Move word selection  | Left / Right |
| Move line selection  | Up / Down    |
| Lookup selected word | Confirm      |
| Exit highlight mode  | Back         |

---

# UX Improvements

* Returning from dictionary preserves highlighted selection
* Highlight mode remains active after lookup
* Word navigation no longer triggers accidental page turns

---

# Smart Lookup Pipeline

```txt
Raw EPUB Word
    ↓
cleanLookupWord()
    ↓
Lowercase conversion
    ↓
Unicode punctuation cleanup
    ↓
Dictionary binary search
    ↓
Thai meaning result
```

---

# Technical Highlights

## Custom Dictionary Engine

* Built specifically for embedded hardware
* Optimized for ESP32 memory limitations
* SD-card streamed lookup architecture

## Embedded-Friendly Design

Optimized for:

* low RAM environments
* slow SD card I/O
* large text databases

---

# Current Status

## Working

* Word highlighting
* Word navigation
* Dictionary lookup
* Word cleaning
* Large dictionary support
* Binary search engine
* Selection persistence
* Stable reader integration

## Planned

* Verb normalization

  * trudged → trudge
  * running → run

* Smarter stemming

* Faster cache/index system

---

# Tech Stack

* C++
* ESP32
* Xteink X4 Firmware
* SdFat / FsFile
* Custom EPUB rendering system

---

# Example

```txt
Input from EPUB:
“Violet,”

After cleaning:
violet

Lookup result:
สีม่วง
```

---

# Project Goal

To create a lightweight but scalable dictionary system for embedded e-ink readers with:

* instant lookup
* massive vocabulary support
* low memory usage
* smooth reading experience
* embedded-device optimization

# AI-Assisted Development

This project was developed as an AI-assisted learning and engineering project.

AI tools were heavily used throughout development for:

* code generation
* architecture iteration
* debugging assistance
* optimization ideas
* firmware integration support

The overall system design, feature integration, testing, customization, and embedded adaptation were directed and validated by the project author.

This repository represents a hands-on exploration of:

* embedded firmware modification
* EPUB interaction systems
* dictionary engine architecture
* large-file lookup optimization
* AI-assisted software development workflows
