<script lang="ts">
    import {
        Position,
        useSvelteFlow,
        type NodeProps,
        Handle,
    } from "@xyflow/svelte";
    import * as Juce from "./js/juce/index.js";

    let { id, data }: NodeProps = $props();
    let { updateNodeData } = useSvelteFlow();

    function handleGainChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { gain: value });
        const adjustGain = Juce.getNativeFunction("setParameter");
        adjustGain(value, id, "gain");
        console.log(id);
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">Gain!</label>
        <input
            type="range"
            id="gain"
            min="-60"
            max="0"
            step="0.01"
            class="nodrag"
            value={data.displayGain ?? data.gain ?? 0}
            oninput={handleGainChange}
        />
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
    </div>
</div>
