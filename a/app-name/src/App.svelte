<script lang="ts">
  import {
    SvelteFlow,
    Controls,
    Background,
    MiniMap,
    Panel,
    addEdge,
    type Node,
    type Edge,
    type Connection,
    type OnDelete,
  } from "@xyflow/svelte";
  import Knob from "./knob.svelte";
  import * as Juce from "./js/juce/index.js";

  import gainSliderNode from "./gainSliderNode.svelte";
  import lfoNode from "./lfoNode.svelte";
  import samplerNode from "./samplerNode.svelte";
  import lowpassNode from "./lowpassNode.svelte";
  import highpassNode from "./highpassNode.svelte";
  import midiTriggerNode from "./midiTriggerNode.svelte";
  import delayNode from "./delayNode.svelte";

  import "@xyflow/svelte/dist/style.css";
  import { untrack } from "svelte";
  import modulationEdge from "./modulationEdge.svelte";

  let edges = $state.raw<Edge[]>([
    {
      id: "sampler-output",
      source: "2",
      target: "1",
    },
  ]);

  const nodeTypes = {
    gainNode: gainSliderNode,
    lfoNode,
    samplerNode,
    lowpassNode,
    highpassNode,
    midiTriggerNode,
    delayNode,
  };

  const edgeTypes = {
    modulationEdge,
  };
  let nodeCount = 4;
  let nodes = $state.raw<Node[]>([
    {
      id: "1",
      data: { label: "audio out" },
      position: { x: 150, y: 150 },
    },
    {
      id: "2",
      type: "samplerNode",
      data: { label: "audio file node" },
      position: { x: 0, y: 300 },
      class: "svelte-flow__node-default",
    },
    {
      id: "3",
      type: "gainNode",
      data: { label: "gain node" },
      position: { x: 300, y: 0 },
      class: "svelte-flow__node-default",
    },
  ]);

  let gainSliderConnected = false;
  let LFOConnected = false;
  //listener function for when a connection is made
  function isAllowedConnection(connection: Connection) {
    if (connection.targetHandle !== "modulation") return true;
    return (
      nodes.find((node) => node.id === connection.source)?.type === "lfoNode"
    );
  }

  function handleConnect(connection: Connection) {
    const addConnection = Juce.getNativeFunction("addConnection");
    if (connection.targetHandle === "modulation") {
      if (!isAllowedConnection(connection)) {
        return;
      }
      addConnection(connection.source, connection.target, 2);
    } else {
      addConnection(connection.source, connection.target, 0);
    }
    const type =
      connection.targetHandle === "modulation" ? "modulationEdge" : undefined;
    edges = addEdge({ ...connection, type }, edges);
  }
  $effect(() => {
    const needsFix = edges.some(
      (e) => e.targetHandle === "modulation" && e.type !== "modulationEdge",
    );
    if (needsFix) {
      edges = edges.map((e) =>
        e.targetHandle === "modulation" && e.type !== "modulationEdge"
          ? { ...e, type: "modulationEdge" }
          : e,
      );
    }
  });

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
    if (deletedNodes.length > 0) {
      // compute numeric ids of deleted nodes
      const deletedIds = deletedNodes
        .map((n) => Number(n.id))
        .filter((x) => !Number.isNaN(x))
        .sort((a, b) => a - b);

      nodes = nodes.map((n) => {
        const cur = Number(n.id);
        if (Number.isNaN(cur)) return n;
        const shift = deletedIds.filter((d) => d < cur).length;
        return { ...n, id: String(cur - shift) };
      });

      // keep nodeCount in sync with current nodes length
      nodeCount = nodes.length + 1;
      for (let i = 0; i < deletedNodes.length; i = i + 1) {
        const deleteNode = Juce.getNativeFunction("deleteNode");
        deleteNode(deletedNodes[i].id);
      }
    }

    if (deletedEdges && deletedEdges.length > 0) {
      const removeConnection = Juce.getNativeFunction("removeConnection");
      if (deletedEdges[0].targetHandle === "modulation") {
        removeConnection(deletedEdges[0].source, deletedEdges[0].target, 2);
      }
      removeConnection(deletedEdges[0].source, deletedEdges[0].target, 0);
    }
  };

  let gainValue = $derived(nodes.find((n) => n.type === "gainNode")?.data.gain);
  $effect(() => {
    if (gainValue !== undefined && gainSliderConnected) {
      handleGainChange(gainValue as number);
    }
  });
  let freqValue = $derived(nodes.find((n) => n.type === "lfoNode")?.data.freq);
  $effect(() => {
    if (LFOConnected) {
      handleFreqChange(freqValue as number);
    } else {
      handleFreqChange(0);
    }
  });
  let ampValue = $derived(
    nodes.find((n) => n.type === "lfoNode")?.data.amplitude,
  );
  $effect(() => {
    if (LFOConnected) {
      handleAmpChange(ampValue as number);
    } else {
      handleAmpChange(-1);
    }
  });
  function handleGainChange(value: number) {
    window.__JUCE__?.backend.emitEvent("gainChange", value);
  }
  function handleFreqChange(value: number) {
    window.__JUCE__?.backend.emitEvent("freqChange", value);
  }
  function handleAmpChange(value: number) {
    window.__JUCE__?.backend.emitEvent("ampChange", value);
  }
  // Add a node dynamically to the flow
  function addNode(
    type:
      | "gainNode"
      | "lfoNode"
      | "lowpassNode"
      | "highpassNode"
      | "samplerNode"
      | "midiTriggerNode"
      | "delayNode",
  ) {
    const newNodeBackend = Juce.getNativeFunction("newNode");

    const id = String(nodeCount);
    nodeCount = nodeCount + 1;
    const idx = nodes.length;
    const position = {
      x: 50 + (idx % 6) * 160,
      y: 50 + Math.floor(idx / 6) * 120,
    };
    newNodeBackend(type);
    const newNode = {
      id,
      type,
      data: { label: `${type.replace(/Node$/, "")} ${id}` },
      position,
      class: "svelte-flow__node-default",
    } as Node;

    nodes = [...nodes, newNode];
  }
</script>

<svelte:window onkeydown={handleKeyDown} />
<div style:height="100vh">
  <SvelteFlow
    bind:nodes
    bind:edges
    {nodeTypes}
    {edgeTypes}
    isValidConnection={isAllowedConnection}
    fitView
    onconnect={handleConnect}
    ondelete={handleDelete}
  >
    <Panel position="top-left">
      <div style="display:flex;gap:8px;align-items:center;">
        <button class="nodrag" onclick={() => addNode("gainNode")}
          >Add Gain</button
        >
        <button class="nodrag" onclick={() => addNode("lfoNode")}
          >Add LFO</button
        >
        <button class="nodrag" onclick={() => addNode("samplerNode")}
          >Add Audio File</button
        >
        <button class="nodrag" onclick={() => addNode("lowpassNode")}
          >add lowpass</button
        >
        <button class="nodrag" onclick={() => addNode("highpassNode")}
          >add highpass</button
        >
        <button class="nodrag" onclick={() => addNode("midiTriggerNode")}
          >add MIDI trigger</button
        >
        <button class="nodrag" onclick={() => addNode("delayNode")}
          >add delay</button
        >
      </div>
    </Panel>
    <Controls />
    <Background />
    <MiniMap />
  </SvelteFlow>
</div>
