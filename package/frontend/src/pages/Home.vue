<script setup lang="ts">
import { ref } from 'vue';
const pass = ref<string>('');
const fetch_API = async (ep: 'create' | 'find') =>
  await fetch(`https://reversi.kawaii-music.xyz/api/${ep}`, {
    method: 'POST',
    body: JSON.stringify({
      pass: pass.value.normalize("NFC"),
    })
  }).then(r => r.json()).then(d => pass.value = d);
</script>

<template>
  <fieldset>
    <legend>
      <h1>Online🌐 Reversi Game</h1>
    </legend>
    <input 
      type="text"
      placeholder="Please enter any Pass (4 characters)" 
      maxlength="4" 
      :value="pass"/>
    <div :class="$style.container">
      <a class="button" @click="fetch_API('create')">create</a>
      <a class="button" @click="fetch_API('find')">Join</a>
    </div>
  </fieldset>
</template>

<style module lang="scss">
fieldset {
  display: flex;
  flex-direction: column;
  align-items: center;

  border: solid 2px;
}

.container {
  display: flex;
  align-items: center;
  justify-content: space-evenly;

  width: 100%;

  margin: 1% 0;

  align-items: center;
}
</style>