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

    function mixChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { gain: value });
        const adjustMix = Juce.getNativeFunction("setParameter");
        adjustMix(value, id, "mix");
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
        <label for="text">delay!</label>
        <div>time</div>
        <input
            type="range"
            id="time"
            min="1"
            max="2000"
            step="1"
            class="nodrag"
            value={data.delayTime ?? 0}
            disabled={samplesFlag}
            oninput={timeChange}
        />
        <div>feedback</div>
        <input
            type="range"
            id="feedback"
            min="0"
            max="1"
            step="0.01"
            class="nodrag"
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
        <div>mix</div>
        <input
            type="range"
            id="mix"
            min="0"
            max="1"
            step="0.01"
            class="nodrag"
            value={data.mix ?? 0}
            oninput={mixChange}
        />
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
    </div>
</div>
