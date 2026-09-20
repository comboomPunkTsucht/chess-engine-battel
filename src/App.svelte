<script lang="ts">
  import reactLogo from "./assets/react.svg";
  import viteLogo from "/electron-vite.animate.svg";
  import "./App.css";
  import { onMount } from "svelte";
  import { log, LogLevel } from "./log";

  let count = $state(0);

  onMount(() => {
    if (window.ipcRenderer) {
      window.ipcRenderer.on("main-process-message", (_event, message) => {
        console.log("Nachricht vom Main-Prozess:", message);
      });
    }
  });

  $effect(() => {
    if (window.ipcRenderer) {
      const handleLog = (_event: any, message: string) => {
        console.log(message);
      };

      window.ipcRenderer.on("log", handleLog);

      // Wird aufgerufen, wenn die Komponente zerstört oder durch HMR neu geladen wird
      return () => {
        window.ipcRenderer.off("log", handleLog);
      };
    }
  });

  function sendMessage() {
    log(LogLevel.INFO, "Sende Nachricht an den Main-Prozess");
  }
</script>

<main>
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
  <button onclick={sendMessage}>Nachricht senden</button>
</main>
