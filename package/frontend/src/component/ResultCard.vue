<script setup lang="ts">
import { ref } from 'vue';
const props = defineProps<{
  isWin?: boolean,
  count: number[],
}>();

const count = ref(props.count);
</script>

<template>
  <fieldset :class="$style.root">
    <legend>
      <h2>Result</h2>
    </legend>
    <div>
      <h3>あなた: {{ count[0] }}</h3>
      <h3>あいて: {{ count[1] }}</h3>
    </div>
    <h2>あなたの{{ isWin ? "勝ち": "負け" }}です。</h2>
    <RouterLink to="/" class="button">戻る</RouterLink>
  </fieldset>
</template>

<style module lang="scss">
.root {
  position: fixed;
  inset: 0;
  margin: auto;

  width: fit-content;
  height: fit-content;

  padding: 0 2rem 1rem 2rem;

  font-size: 120%;

  background-color: rgba(255, 255, 255, 0.8);

  border-radius: 7px;
  border: solid 3px;

  div {
    display: flex;
    justify-content: space-around;

    width: 100%;
  }
  h2 {
    text-align: center;
  }
}
</style>