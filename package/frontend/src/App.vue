<script setup lang="ts">
import TopBar from './component/TopBar.vue';
</script>

<template>
  <TopBar />
  <div id="page">
    <router-view />
  </div>
</template>
