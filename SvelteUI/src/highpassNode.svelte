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

    function handleCutoffChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { cutoff: value });
        const setFrequency = Juce.getNativeFunction("setParameter");
        setFrequency(value, id, "frequencyHPF");
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">HPF</label>
        <input
            type="range"
            id="cutoff"
            min="100"
            max="20000"
            step="0.01"
            class="nodrag slider"
            value={data.cutoff ?? 100}
            oninput={handleCutoffChange}
        />
        <span>{Number(data.cutoff ?? 100).toFixed(0)} Hz</span>
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle
            type="target"
            position={Position.Right}
            id="modulation"
            class="modulation-handle"
        />
    </div>
</div>
