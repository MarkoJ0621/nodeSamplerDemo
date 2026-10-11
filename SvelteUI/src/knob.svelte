<script lang="ts">
    interface Props {
        value?: number;
        min?: number;
        max?: number;
        step?: number;
        disabled?: boolean;
        size?: number;
        strokeWidth?: number;
        label?: string;
        showValue?: boolean;
        startAngle?: number; // degrees, 0 = 12 o'clock, clockwise
        endAngle?: number; // 270deg sweep by default, 90deg gap at the bottom
        dragMode?: "vertical" | "horizontal" | "angular";
        sensitivity?: number; // px of drag to cover the full min-max range (vertical/horizontal only)
        oninput?: (value: number) => void;
        onchange?: (value: number) => void;
    }

    let {
        value = $bindable(50),
        min = 0,
        max = 100,
        step = 1,
        disabled = false,
        size = 64,
        strokeWidth = 8,
        label = "",
        showValue = false,
        startAngle = -135,
        endAngle = 135,
        dragMode = "vertical",
        sensitivity = 200,
        oninput,
        onchange,
    }: Props = $props();

    let knobEl: HTMLDivElement;
    let dragging = $state(false);
    let dragStartValue = 0;
    let dragStartX = 0;
    let dragStartY = 0;

    const cx = 50,
        cy = 50,
        r = 40; // fixed viewBox geometry, scaled by CSS size

    let range = $derived(max - min);
    let sweep = $derived(endAngle - startAngle);
    let decimals = $derived(decimalsOf(step));
    let valueAngle = $derived(
        startAngle + ((clamp(value, min, max) - min) / range) * sweep,
    );
    let trackPath = $derived(describeArc(startAngle, endAngle));
    let progressPath = $derived(describeArc(startAngle, valueAngle));
    let thumb = $derived(polarToCartesian(valueAngle));
    let faceRadius = $derived(r + strokeWidth / 2); // covers the ring's outer edge fully

    function clamp(v: number, lo: number, hi: number): number {
        return Math.min(hi, Math.max(lo, v));
    }

    function decimalsOf(n: number): number {
        const s = String(n);
        return s.includes(".") ? s.split(".")[1].length : 0;
    }

    function roundToStep(v: number): number {
        const steps = Math.round((v - min) / step);
        const raw = min + steps * step;
        return clamp(parseFloat(raw.toFixed(decimals)), min, max);
    }

    function angleToValue(deg: number): number {
        const clamped = clamp(deg, startAngle, endAngle);
        const pct = (clamped - startAngle) / sweep;
        return roundToStep(min + pct * range);
    }

    function pointerAngle(clientX: number, clientY: number): number {
        const rect = knobEl.getBoundingClientRect();
        const dx = clientX - (rect.left + rect.width / 2);
        const dy = clientY - (rect.top + rect.height / 2);
        return Math.atan2(dx, -dy) * (180 / Math.PI); // 0 = up, clockwise positive
    }

    function setFromPointer(clientX: number, clientY: number) {
        const next = angleToValue(pointerAngle(clientX, clientY));
        if (next !== value) {
            value = next;
            oninput?.(value);
        }
    }

    function handlePointerDown(e: PointerEvent) {
        if (disabled) return;
        dragging = true;
        knobEl.setPointerCapture(e.pointerId);
        dragStartValue = value;
        dragStartX = e.clientX;
        dragStartY = e.clientY;
        // Angular mode jumps straight to the clicked angle. Vertical/horizontal
        // modes are relative — clicking without moving shouldn't change anything,
        // only the drag delta does, same as a DAW-style knob.
        if (dragMode === "angular") {
            setFromPointer(e.clientX, e.clientY);
        }
        e.preventDefault();
    }

    function handlePointerMove(e: PointerEvent) {
        if (!dragging) return;
        if (dragMode === "angular") {
            setFromPointer(e.clientX, e.clientY);
            return;
        }
        const rawDelta =
            dragMode === "vertical"
                ? dragStartY - e.clientY
                : e.clientX - dragStartX;
        const precision = e.shiftKey ? 4 : 1; // hold Shift to drag more slowly for finer control
        const deltaValue = (rawDelta / (sensitivity * precision)) * range;
        const next = roundToStep(clamp(dragStartValue + deltaValue, min, max));
        if (next !== value) {
            value = next;
            oninput?.(value);
        }
    }

    function handlePointerUp(e: PointerEvent) {
        if (!dragging) return;
        dragging = false;
        knobEl.releasePointerCapture(e.pointerId);
        onchange?.(value);
    }

    function commit(next: number) {
        if (next === value) return;
        value = next;
        oninput?.(value);
        onchange?.(value);
    }

    function handleKeydown(e: KeyboardEvent) {
        if (disabled) return;
        const bigStep = step * 10;
        switch (e.key) {
            case "ArrowUp":
            case "ArrowRight":
                e.preventDefault();
                commit(clamp(value + step, min, max));
                break;
            case "ArrowDown":
            case "ArrowLeft":
                e.preventDefault();
                commit(clamp(value - step, min, max));
                break;
            case "PageUp":
                e.preventDefault();
                commit(clamp(value + bigStep, min, max));
                break;
            case "PageDown":
                e.preventDefault();
                commit(clamp(value - bigStep, min, max));
                break;
            case "Home":
                e.preventDefault();
                commit(min);
                break;
            case "End":
                e.preventDefault();
                commit(max);
                break;
        }
    }

    function handleWheel(e: WheelEvent) {
        if (disabled) return;
        e.preventDefault();
        const dir = e.deltaY < 0 ? 1 : -1;
        commit(clamp(value + dir * step, min, max));
    }

    function polarToCartesian(angleDeg: number): { x: number; y: number } {
        const rad = (angleDeg * Math.PI) / 180;
        return { x: cx + r * Math.sin(rad), y: cy - r * Math.cos(rad) };
    }

    // Draws the arc from a0 to a1 as two half-sweeps instead of one.
    // A single SVG arc command gets numerically unstable as its chord
    // approaches the circle's diameter (sweep approaching/crossing 180deg) —
    // some renderers pick the wrong one of the two possible circles and the
    // arc visibly kinks or shoots off. Splitting at the midpoint keeps each
    // half well under 180deg (max total sweep here is 270, so each half maxes
    // out at 135deg), which sidesteps the unstable case entirely.
    function describeArc(a0: number, a1: number): string {
        if (Math.abs(a1 - a0) < 0.0001) return "";
        const mid = a0 + (a1 - a0) / 2;
        const p0 = polarToCartesian(a0);
        const pMid = polarToCartesian(mid);
        const p1 = polarToCartesian(a1);
        return `M ${p0.x} ${p0.y} A ${r} ${r} 0 0 1 ${pMid.x} ${pMid.y} A ${r} ${r} 0 0 1 ${p1.x} ${p1.y}`;
    }
</script>

<svelte:window
    onpointermove={handlePointerMove}
    onpointerup={handlePointerUp}
/>

<div class="knob-wrap" style="--knob-size: {size}px" class:disabled>
    <div
        bind:this={knobEl}
        class="knob {dragMode}"
        role="slider"
        tabindex={disabled ? -1 : 0}
        aria-valuemin={min}
        aria-valuemax={max}
        aria-valuenow={value}
        aria-labelledby={label ? "knob-label" : undefined}
        aria-disabled={disabled}
        onpointerdown={handlePointerDown}
        onkeydown={handleKeydown}
        onwheel={handleWheel}
    >
        <svg viewBox="0 0 100 100">
            <circle class="face" {cx} {cy} r={faceRadius} />
            <path
                class="track"
                d={trackPath}
                style="stroke-width: {strokeWidth}"
            />
            <path
                class="progress"
                d={progressPath}
                style="stroke-width: {strokeWidth}"
            />
            <circle
                class="thumb"
                cx={thumb.x}
                cy={thumb.y}
                r={strokeWidth / 2 + 1}
            />
        </svg>

        {#if showValue}
            <div class="value">{value}</div>
        {/if}
    </div>
</div>

<style>
    .knob-wrap {
        display: inline-flex;
        flex-direction: column;
        align-items: center;
        gap: 0.35rem;
        font-family: system-ui, sans-serif;
        user-select: none;
        -webkit-user-select: none;
    }

    .knob-wrap.disabled {
        opacity: 0.5;
    }

    .knob-label {
        font-size: 0.7rem;
        color: var(--knob-label-color, #000000);
    }

    .knob {
        position: relative;
        width: var(--knob-size);
        height: var(--knob-size);
        cursor: grab;
        touch-action: none;
        outline: none;
        border-radius: 50%;
    }

    .knob:active {
        cursor: grabbing;
    }

    .knob.vertical {
        cursor: grab;
    }

    .knob.horizontal {
        cursor: grab;
    }

    .knob-wrap.disabled .knob {
        cursor: not-allowed;
        pointer-events: none;
    }

    .knob:focus-visible {
        outline: 2px solid var(--knob-focus-ring, #000000);
        outline-offset: 2px;
    }

    svg {
        width: 100%;
        height: 100%;
        display: block;
        overflow: visible;
    }

    .face {
        fill: var(--knob-face-color, #ffffff);
    }

    .track {
        fill: none;
        stroke: var(--knob-track-color, #e5e7eb);
        stroke-linecap: round;
    }

    .progress {
        fill: none;
        stroke: var(--knob-accent-color, #000000);
        stroke-linecap: round;
    }

    .thumb {
        fill: var(--knob-thumb-color, #000000);
    }

    .value {
        position: absolute;
        inset: 0;
        display: flex;
        align-items: center;
        justify-content: center;
        font-size: calc(var(--knob-size) * 0.22);
        font-variant-numeric: tabular-nums;
        color: var(--knob-value-color, #1f2937);
        pointer-events: none;
    }

    @media (prefers-reduced-motion: reduce) {
        .progress,
        .thumb {
            transition: none;
        }
    }
</style>
