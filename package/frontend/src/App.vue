<script setup lang="ts">
import { ref, provide, watch } from 'vue';
import TopBar from './component/TopBar.vue';
import ErrPopUp from './component/ErrPopUp.vue';

const errList = ref<Err[]>([]);
provide('errList', errList);

watch(errList, () => {
  if (errList.value.length > 5) {
    errList.value.pop();
  } else {
    setTimeout(() => {
      errList.value.pop();
    }, 2000);
  }
}, { deep: true });
</script>

<template>
  <TopBar />
  <div id="page">
    <router-view />
  </div>
  <ErrPopUp />
</template>
