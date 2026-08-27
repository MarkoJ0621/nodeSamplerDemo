<script lang="ts">
    import {
        Position,
        useSvelteFlow,
        type NodeProps,
        Handle,
    } from "@xyflow/svelte";
    import { onMount, onDestroy } from "svelte";
    import * as Juce from "./js/juce/index.js";
    let { id, data }: NodeProps = $props();
    let { updateNodeData } = useSvelteFlow();
    let setParameter = Juce.getNativeFunction("setParameter");
    function updateFreq(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { freq: value });
        setParameter(value, id, "lfofreq");
    }
    function updateAmplitude(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { amplitude: value });
        setParameter(value, id, "lfoamp");
    }
</script>

<div class="lfo-node">
    <div>
        <label for="text">LFO</label>
        <input
            type="range"
            id="freq"
            min="0.5"
            max="5"
            step="0.01"
            class="nodrag"
            value={data.freq ?? 0}
            oninput={updateFreq}
        />
        <label for="freq">Frequency</label>
        <input
            type="range"
            id="amplitude"
            min="0"
            max="1"
            step="0.01"
            class="nodrag"
            value={data.amplitude ?? 0}
            oninput={updateAmplitude}
        />
        <label for="amplitude">Amplitude</label>
        <Handle type="source" position={Position.Bottom} />
    </div>
</div>
