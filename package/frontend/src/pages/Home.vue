<script setup lang="ts">
import { Ref, ref, inject } from 'vue';
import router from '../router';
import cookie from '../scripts/cookie';

const pass = ref<string>('');
const errList = inject<Ref<Err[]>>('errList');

const fetch_API = async (ep: 'create' | 'find'): Promise<void> =>
  await fetch(`https://reversi.kawaii-music.xyz/api/${ep}`, {
    method: 'POST',
    body: JSON.stringify({
      pass: pass.value,
    }),
  }).then(async r => {
    const body = (await r.json());
    if (r.ok && body) {
      cookie.write('roomId', body.roomId);
      cookie.write('playerId', body.playerId);
      router.push('/play');
    } else {
      errList?.value.push({
        title: `${r.status}`,
        msg: body.err,
      });
    }
  });
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
      v-model="pass"/>
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