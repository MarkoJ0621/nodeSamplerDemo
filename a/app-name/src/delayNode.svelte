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

    // function handleGainChange(event: Event) {
    //     const value = Number((event.target as HTMLInputElement).value);
    //     updateNodeData(id, { gain: value });
    //     const adjustGain = Juce.getNativeFunction("setParameter");
    //     adjustGain(value, id, "gain");
    //     console.log(id);
    // }
    let samplesFlag = $state(false);

    function timeChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { gain: value });
        const adjustTime = Juce.getNativeFunction("setParameter");
        adjustTime(value, id, "time");
        console.log(value);
    }

    function feedbackChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { gain: value });
        const adjustFeedback = Juce.getNativeFunction("setParameter");
        adjustFeedback(value, id, "feedback");
        console.log(value);
    }

    function delayToggle() {
        samplesFlag = !samplesFlag;
        const toggleSamples = Juce.getNativeFunction("setParameter");
        toggleSamples(samplesFlag, id, "samplesToggle");
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">Gain!</label>
        <input
            type="range"
            id="time"
            min="1"
            max="2000"
            step="1"
            class="nodrag"
            value={data.delayTime ?? 0}
            oninput={timeChange}
        />
        <input
            type="range"
            id="feedback"
            min="0"
            max="100"
            step="1"
            class="nodrag"
            disabled={samplesFlag}
            value={data.feedback ?? 0}
            oninput={feedbackChange}
        />
        <button
            onclick={delayToggle}
            id="samplesInput"
            aria-pressed={samplesFlag}
        >
            {samplesFlag ? "time" : "samples"}
        </button>
        <input
            type="number"
            id="samplesDelay"
            min="0"
            step="0"
            class="nodrag"
            disabled={!samplesFlag}
            oninput={timeChange}
        />
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
    </div>
</div>
