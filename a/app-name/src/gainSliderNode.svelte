<script lang="ts">
    import {
        Position,
        useSvelteFlow,
        type NodeProps,
        Handle,
    } from "@xyflow/svelte";

    let { id, data }: NodeProps = $props();
    let { updateNodeData } = useSvelteFlow();

    function handleGainChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { gain: value });
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">Gain!</label>
        <input
            type="range"
            id="gain"
            min="0"
            max="1"
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
