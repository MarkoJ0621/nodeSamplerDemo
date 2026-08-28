<script lang="ts">
    import {
        BaseEdge,
        EdgeLabel,
        getStraightPath,
        useEdges,
        type EdgeProps,
    } from "@xyflow/svelte";
    import * as Juce from "./js/juce/index.js";

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
</script>

<BaseEdge {id} path={edgePath} />
<EdgeLabel x={labelX} y={labelY}>
    <label for="text">modDepth!</label>
    <input
        type="range"
        id="gain"
        min="-5"
        max="5"
        step="0.01"
        class="nodrag nopan"
        oninput={handleDepthChange}
    />
</EdgeLabel>
