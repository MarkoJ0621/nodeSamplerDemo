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

    function handleNoteChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { note: value });
        const setMidiTrigger = Juce.getNativeFunction("setParameter");
        setMidiTrigger(value, id, "setMidi");
    }
    function triggerNote() {
        console.log("bang!");
        const trigger = Juce.getNativeFunction("triggerOutput");
        trigger(id);
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">MIDI trigger</label>
        <input
            type="range"
            id="gain"
            min="0"
            max="60"
            step="1"
            class="nodrag"
            oninput={handleNoteChange}
            value={data.note ?? 0}
        />
        <button class="nodrag" onclick={triggerNote}>trigger</button>
        <div>current note selected: {data.note}</div>
        <Handle type="target" position={Position.Top} id="input" />
    </div>
</div>
