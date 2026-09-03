<script lang="ts">
    import {
        Position,
        useSvelteFlow,
        type NodeProps,
        Handle,
    } from "@xyflow/svelte";

    let { id, data }: NodeProps = $props();
    let { updateNodeData } = useSvelteFlow();
    import * as Juce from "./js/juce/index.js";

    // async function handleFile(event: Event) {
    //     const filePath = await window.__JUCE__?.backend.chooseFile();
    //     if (filePath) {
    //         updateNodeData(id, { filePath });
    //     }
    // }
    var loopFlag = false;
    function handleLoop() {
        if (loopFlag) {
            loopFlag = false;
        } else {
            loopFlag = true;
        }
        Juce.getNativeFunction("setParameter")(loopFlag, id, "loop");
    }
    function handlePlaybackSpeed(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { playbackSpeed: value });
        const adjustPlaybackSpeed = Juce.getNativeFunction("setParameter");
        adjustPlaybackSpeed(value, id, "playbackSpeed");
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">File!</label>
        <!-- <input type="file" id="file" class="nodrag" oninput={handleFile} /> -->
        <button onclick={() => Juce.getNativeFunction("chooseFile")(id)}
            >hi</button
        >

        <button onclick={() => Juce.getNativeFunction("start")(id)}>Play</button
        >
        <button onclick={() => Juce.getNativeFunction("stop")(id)}>stop</button>
        <input type="checkbox" id="loop" oninput={handleLoop} />
        <label for="loop">loop</label>
        <div>playback speed</div>
        <input
            type="range"
            id="playbackSpeed"
            min="0"
            max="2"
            step="0.01"
            class="nodrag"
            oninput={handlePlaybackSpeed}
        />
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
        <Handle
            type="target"
            position={Position.Right}
            id="playbackSpeed"
            style="top: 20px"
        />
    </div>
</div>
