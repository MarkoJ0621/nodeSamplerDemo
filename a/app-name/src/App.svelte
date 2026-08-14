<script lang="ts">
  import {
    SvelteFlow,
    Controls,
    Background,
    MiniMap,
    Panel,
    type Node,
    type Edge,
    type OnDelete,
  } from "@xyflow/svelte";
  import * as Juce from "./js/juce/index.js";

  import gainSliderNode from "./gainSliderNode.svelte";

  import "@xyflow/svelte/dist/style.css";

  const nodeTypes = {
    gainSliderNode,
  };

  let nodes = $state.raw<Node[]>([
    {
      id: "1",
      data: { label: "audio source" },
      position: { x: 0, y: 0 },
    },
    {
      id: "2",
      data: { label: "audio out" },
      position: { x: 150, y: 150 },
    },
    {
      id: "3",
      type: "gainSliderNode",
      data: { label: "gain node" },
      position: { x: 300, y: 0 },
      class: "svelte-flow__node-default",
    },
  ]);

  let gainSliderConnected = false;
  let audioSourceConnected = false;
  //listener function for when a connection is made
  function handleConnect(connection: Connection) {
    console.log("Connection made:", connection);

    nodes = nodes.map((node) =>
      node.id === connection.target
        ? { ...node, data: { ...node.data, label: "Audio connected!" } }
        : node,
    );
    if (connection.target === "2") {
      audioSourceConnected = true;
    }
    if (connection.target === "3" && connection.source === "1") {
      gainSliderConnected = true;
      handleGainChange(nodes.find((n) => n.id === "3")?.data.gain as number);
    }
    if (audioSourceConnected && gainSliderConnected) {
      handleGainChange(nodes.find((n) => n.id === "3")?.data.gain as number);
    } else if (audioSourceConnected && !gainSliderConnected) {
      handleGainChange(1);
    }
  }
  function handleKeyDown(event: KeyboardEvent) {
    if (event.key === "Backspace" || event.key === "Delete") {
      event.preventDefault();
    }
  }
  //callback function for node or edge deletion, idk why one is a callback and the other isnt
  const handleDelete: OnDelete<Node, Edge> = ({
    nodes: deletedNodes,
    edges: deletedEdges,
  }) => {
    if (deletedEdges.length) {
      console.log("Edges deleted:", deletedEdges);

      const deletedTargetIds = new Set(deletedEdges.map((e) => e.target));

      nodes = nodes.map((node) =>
        deletedTargetIds.has(node.id)
          ? { ...node, data: { ...node.data, label: "Audio disconnected!" } }
          : node,
      );
      if (deletedEdges.some((e) => e.target === "3" && audioSourceConnected)) {
        gainSliderConnected = false;
        handleGainChange(1);
      }
      if (deletedEdges.some((e) => e.target === "2")) {
        audioSourceConnected = false;
        handleGainChange(0);
      }
    }
  };

  let gainValue = $derived(nodes.find((n) => n.id === "3")?.data.gain);
  $effect(() => {
    console.log(
      "audio:",
      audioSourceConnected,
      "gain:",
      gainSliderConnected,
      "value:",
      gainValue,
    );
    if (!audioSourceConnected) {
      handleGainChange(0);
    } else if (gainValue !== undefined && gainSliderConnected) {
      console.log("here");
      handleGainChange(gainValue as number);
    }
  });

  function handleGainChange(value: number) {
    console.log("Gain changed to:", value);
    window.__JUCE__?.backend.emitEvent("gainChange", value);
  }
</script>

<svelte:window onkeydown={handleKeyDown} />
<div style:height="100vh">
  <SvelteFlow
    bind:nodes
    {nodeTypes}
    fitView
    onconnect={handleConnect}
    ondelete={handleDelete}
  >
    <Panel position="top-right">hello...</Panel>
    <Controls />
    <Background />
    <MiniMap />
  </SvelteFlow>
</div>
