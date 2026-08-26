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
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
    </div>
</div>
