<script lang="ts">
    import {
        BaseEdge,
        EdgeLabel,
        getStraightPath,
        useEdges,
        type EdgeProps,
    } from "@xyflow/svelte";
    import * as Juce from "./js/juce/index.js";
    import Knob from "./knob.svelte";
    let { id, sourceX, sourceY, targetX, targetY, target }: EdgeProps =
        $props();
    let [edgePath, labelX, labelY] = $derived(
        getStraightPath({
            sourceX,
            sourceY,
            targetX,
            targetY,
        }),
    );

    const edges = useEdges();

    function handleDepthChange(event: Event) {
        const value = Number((event.target as HTMLInputElement).value);
        const adjustDepth = Juce.getNativeFunction("setParameter");
        adjustDepth(value, target, "modulationDepth");
        console.log(target);
    }
    let modDepth = $state(0);
</script>

<BaseEdge {id} path={edgePath} />
<EdgeLabel x={labelX} y={labelY} style="background: transparent;">
    <input
        type="hidden"
        id="gain"
        min="-5"
        max="5"
        step="0.01"
        class="nodrag nopan"
        oninput={handleDepthChange}
    />
    <Knob
        bind:value={modDepth}
        min={-5}
        max={5}
        label="mod depth"
        size={40}
        onchange={(v) => console.log("committed", v)}
    />
</EdgeLabel>
