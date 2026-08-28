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
    type OnDelete,
  } from "@xyflow/svelte";
  import * as Juce from "./js/juce/index.js";

  import gainSliderNode from "./gainSliderNode.svelte";
  import lfoNode from "./lfoNode.svelte";
  import samplerNode from "./samplerNode.svelte";
  import lowpassNode from "./lowpassNode.svelte";
  import highpassNode from "./highpassNode.svelte";
  import midiTriggerNode from "./midiTriggerNode.svelte";
  import "@xyflow/svelte/dist/style.css";
  import { untrack } from "svelte";
  import modulationEdge from "./modulationEdge.svelte";

  let edges = $state.raw<Edge[]>([]);

  const nodeTypes = {
    gainSliderNode,
    lfoNode,
    samplerNode,
    lowpassNode,
    highpassNode,
    midiTriggerNode,
  };

  const edgeTypes = {
    modulationEdge,
  };
  let nodeCount = 5;
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
      type: "samplerNode",
      data: { label: "audio file node" },
      position: { x: 0, y: 300 },
      class: "svelte-flow__node-default",
    },
    {
      id: "4",
      type: "gainSliderNode",
      data: { label: "gain node" },
      position: { x: 300, y: 0 },
      class: "svelte-flow__node-default",
    },
  ]);

  let gainSliderConnected = false;
  let audioSourceConnected = false;
  let LFOConnected = false;
  //listener function for when a connection is made
  function handleConnect(connection: Connection) {
    console.log("Connection made:", connection);
    const addConnection = Juce.getNativeFunction("addConnection");
    if (connection.targetHandle === "modulation") {
      console.log("modulation connection made");
      addConnection(connection.source, connection.target, 2);
    } else {
      addConnection(connection.source, connection.target, 0);
    }
    const type =
      connection.targetHandle === "modulation" ? "modulationEdge" : undefined;
    console.log(type);
    edges = addEdge({ ...connection, type }, edges);
    console.log(edges);
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
      console.log(nodes);
      console.log(nodeCount);
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
  let freqValue = $derived(nodes.find((n) => n.id === "4")?.data.freq);
  $effect(() => {
    console.log("freq:", freqValue);
    if (LFOConnected) {
      handleFreqChange(freqValue as number);
    } else {
      handleFreqChange(0);
    }
  });
  let ampValue = $derived(nodes.find((n) => n.id === "4")?.data.amplitude);
  $effect(() => {
    console.log("amp:", ampValue);
    if (LFOConnected) {
      handleAmpChange(ampValue as number);
    } else {
      handleAmpChange(-1);
      console.log("here!");
    }
  });
  function handleGainChange(value: number) {
    console.log("Gain changed to:", value);
    window.__JUCE__?.backend.emitEvent("gainChange", value);
  }
  function handleFreqChange(value: number) {
    console.log("Freq changed to:", value);
    window.__JUCE__?.backend.emitEvent("freqChange", value);
  }
  function handleAmpChange(value: number) {
    console.log("Amp changed to:", value);
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
      | "midiTriggerNode",
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
    fitView
    onconnect={handleConnect}
    ondelete={handleDelete}
  >
    <Panel position="top-left">
      <div style="display:flex;gap:8px;align-items:center;">
        <button onclick={() => addNode("gainNode")}>Add Gain</button>
        <button onclick={() => addNode("lfoNode")}>Add LFO</button>
        <button onclick={() => addNode("samplerNode")}>Add Audio File</button>
        <button onclick={() => addNode("lowpassNode")}>add lowpass</button>
        <button onclick={() => addNode("highpassNode")}>add highpass</button>
        <button onclick={() => addNode("midiTriggerNode")}
          >add MIDI trigger</button
        >
      </div>
    </Panel>
    <Panel position="top-right">hello...</Panel>
    <Controls />
    <Background />
    <MiniMap />
  </SvelteFlow>
</div>
