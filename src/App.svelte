<script lang="ts">
  import reactLogo from "./assets/react.svg";
  import viteLogo from "/electron-vite.animate.svg";
  import "./App.css";

  let count = $state(0);
</script>

<div>
  <a href="https://electron-vite.github.io" target="_blank">
    <img src={viteLogo} class="logo" alt="Vite logo" />
  </a>
  <a href="https://react.dev" target="_blank">
    <img src={reactLogo} class="logo react" alt="React logo" />
  </a>
</div>
<h1>Vite + React</h1>
<div class="card">
  <button onclick={() => count++}>count is {count}</button>
  <p>
    Edit <code>src/App.svelte</code> and save to test HMR
  </p>
</div>
<p class="read-the-docs">Click on the Vite and React logos to learn more</p>

<style>
</style>
