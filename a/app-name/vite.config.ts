import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";

export default defineConfig({
  plugins: [
    svelte({
      emitCss: false,
    }),
  ],
  optimizeDeps: {
    exclude: ["@xyflow/svelte"],
  },
  build: {
    outDir: '../../plugin/ui/public',
    emptyOutDir: true,
  },
});
