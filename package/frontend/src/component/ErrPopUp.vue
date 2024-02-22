<script setup lang="ts">
import { inject } from 'vue';

const errList = inject<Err[]>('errList');
</script>

<template>
  <link rel="stylesheet"
    href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200" />
  <div :class="$style.root">
    <div :class="$style.tile" v-for="e in errList">
      <div>
        <span class="material-symbols-outlined">warning</span>
        <h3>Err: {{ e.title }}</h3>
      </div>
      <p>{{ e.msg }}</p>
    </div>
  </div>
</template>

<style module lang="scss">
.root {
  display: flex;
  flex-direction: column;

  position: absolute;
  bottom: 0;
  right: 0;

  margin: 0.4rem;

  .tile {
    min-width: 15rem;
    width: fit-content;
    max-width: 30%;

    padding: 0.5rem;
    margin: 0.2rem 0;

    background-color: rgba(190, 0, 0, 0.2);

    border: solid 2px rgb(190, 0, 0);
    border-radius: 7px;

    user-select: none;

    div {
      display: flex;
      color: rgb(184, 0, 0);
    }

    p {
      font-size: 90%;
      white-space: nowrap;
      margin: 0;
    }
  }
}</style>