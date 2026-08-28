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

    function sliderToFrequency(
        sliderValue: number,
        min = 20,
        max = 20000,
    ): number {
        return min * Math.pow(max / min, sliderValue);
    }

    function frequencyToSlider(
        frequency: number,
        min = 20,
        max = 20000,
    ): number {
        return Math.log(frequency / min) / Math.log(max / min);
    }
</script>

<div class="gain-slider-node">
    <div>
        <label for="text">LPF</label>
        <input
            type="range"
            min="0"
            max="1"
            step="0.0001"
            class="nodrag"
            value={frequencyToSlider(Number(data.cutoff ?? 1000))}
            oninput={(e) => {
                const sliderVal = Number((e.target as HTMLInputElement).value);
                const freq = sliderToFrequency(sliderVal);
                updateNodeData(id, { cutoff: freq });
                Juce.getNativeFunction("setParameter")(freq, id, "frequency");
            }}
        />
        <Handle type="source" position={Position.Bottom} />
        <Handle type="target" position={Position.Top} id="input" />
        <Handle type="target" position={Position.Right} id="modulation" />
    </div>
</div>
