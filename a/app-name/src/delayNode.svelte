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
    // }
    let samplesFlag = $state(false);

    function timeChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { delayTime: value });
        const adjustTime = Juce.getNativeFunction("setParameter");
        adjustTime(value, id, "time");
    }

    function sampleDelayChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { sampleDelay: value });
        const adjustTime = Juce.getNativeFunction("setParameter");
        adjustTime(value, id, "time");
    }

    function feedbackChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { feedback: value });
        const adjustFeedback = Juce.getNativeFunction("setParameter");
        adjustFeedback(value, id, "feedback");
    }

    function mixChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        updateNodeData(id, { mix: value });
        const adjustMix = Juce.getNativeFunction("setParameter");
        adjustMix(value, id, "mix");
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
            class="nodrag slider"
            value={data.delayTime ?? 0}
            disabled={samplesFlag}
            oninput={timeChange}
        />
        <span>{Number(data.delayTime ?? 1).toFixed(0)} ms</span>
        <div>feedback</div>
        <input
            type="range"
            id="feedback"
            min="0"
            max="1"
            step="0.01"
            class="nodrag slider"
            value={data.feedback ?? 0}
            oninput={feedbackChange}
        />
        <div>{Number(data.feedback ?? 0).toFixed(2)}</div>
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
            step="1"
            class="nodrag"
            value={data.sampleDelay ?? 0}
            disabled={!samplesFlag}
            oninput={sampleDelayChange}
        />
        <div>mix</div>
        <input
            type="range"
            id="mix"
            min="0"
            max="1"
            step="0.01"
            class="nodrag slider"
            value={data.mix ?? 0}
            oninput={mixChange}
        />
        <span>{Number(data.mix ?? 0).toFixed(2)}</span>
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

<style>
    #samplesDelay {
        box-sizing: border-box;
        max-width: 100%;
    }

    #time:disabled {
        opacity: 0.35;
        cursor: not-allowed;
    }

    #time:disabled::-webkit-slider-thumb {
        background: #888;
        cursor: not-allowed;
    }

    #time:disabled::-moz-range-thumb {
        background: #888;
        cursor: not-allowed;
    }
</style>
