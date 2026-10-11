# NodeSampler

NodeSampler is a Max-inspired, node-based audio plugin for flexible sampling. Load sounds, process them with connected nodes, and map them to MIDI notes.

## Requirements

- CMake 3.22 or newer
- A C++20 compiler
- Node.js and npm (or pnpm)
- JUCE 8.0.10

This plugin is untested on Windows or Linux, however it definetely runs on MacOS
## Build

Clone the repository and enter its directory:

```bash
git clone https://github.com/MarkoJ0621/nodeSamplerDemo.git
cd nodeSamplerDemo
```

Download the JUCE version used by this project:

```bash
git clone --depth 1 --branch 8.0.10 https://github.com/juce-framework/JUCE.git juce
```

Build the web UI. The UI source is in `SvelteUI`, and the build outputs directly to `plugin/ui/public`:

```bash
cd SvelteUI
npm install
npm run build
cd ../
```

Configure and build the plugin:

```bash
cmake -B build 
cmake --build build 
```

The generated plugin files are placed under:

```text
build/NodeSampler_artefacts
```

## macOS unsigned builds

The macOS plugin is not code-signed or notarized. Gatekeeper may therefore block it when it is first opened. After confirming that the build came from a trusted source, remove the quarantine attribute from the installed plugin:

after placing the plugin in your VST folder

```bash
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/NodeSampler.vst3
```

For an Audio Unit installed in the Components directory:

```bash
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/Components/NodeSampler.component
```

Restart the host application after installing or changing a plugin.

