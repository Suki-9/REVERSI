<script setup lang="ts">
import { ref, inject } from 'vue';
import cookie from '../scripts/cookie';
import router from '../router';


const board = ref<number[]>([...Array(64)].map(_ => (0)));
const errList = inject<Err[]>('errList');
const Myturn = ref<boolean>(false);

const roomId = cookie.read('roomId');
const playerId = cookie.read('playerId');

if (roomId && playerId) {
  const ws = new WebSocket(`wss://reversi.kawaii-music.xyz/api/stream?roomId=${roomId}&playerId=${playerId}`);

  ws.addEventListener('open', () => {
    ws.send(JSON.stringify({
      type: "connection",
    }))
  });

  ws.addEventListener('message', (e) => {
    const msg = JSON.parse(e.data);
    switch (msg.type) {
      case 'start':
        board.value = msg.body.board ?? board.value;
        Myturn.value = msg.body.yourTurn ?? false;
        break;
      case 'put':
        board.value = msg.body.board ?? board.value;
        break;
      case 'end':
        ws.close();
        break;
      default:
        break
    }
  });
} else { 
  errList?.push({
    title: 'Failed to read cookie.',
    msg: 'It will return to the title in a few seconds.'
  });
  setTimeout(() => {
    router.push('/');
  }, 2000);
}


const putStone = (p: number) => {
  if (ws.readyState == 1) { 
    ws.send(JSON.stringify({
      type: 'put',
      body: {
        put: p,
      },
    }))
  }
};

</script>

<template>
  <div :class="$style.root">
    <div :class="$style.board">
      <a 
        v-for="(s, i) in board" 
        :class="$style.cell" 
        v-text="s 
          ? s == 1
            ? '!'
            : s == 2
              ? '?'
              : '&'
          : ''"
        @click="putStone(i)"></a>
    </div>
    <div :class="$style.info">
      <p>あなた: </p>
      <p>あいて: </p>
    </div>
  </div>
</template>

<style module lang="scss">
.root {
  display: flex;
  flex-direction: column;
  align-items: center;

  width: 100%;

  .board {
    display: grid;
    grid-template-columns: repeat(8, 1fr);
    grid-gap: 1px;

    padding: 1px;

    margin: 1% 0;

    width: 90%;
    max-width: 85vh;

    .cell {
      border-radius: 3px;
      transition: background-color 0.4s ease-in-out 0s;
      background-color: rgba(0, 218, 0, 0.4);
      margin: 0;
      padding: 0;
      aspect-ratio: 1;

      &:hover {
        transition: background-color 0.1s ease-in-out 0s;
        background-color: rgba(0, 218, 0, 0.3);
      }

      &:active {
        transition: scale 0.1s ease-in-out 0s;
        border-radius: 5px;
        scale: 0.9;
      }

      &::before {}
    }
  }

  .info {
    display: flex;
    align-items: center;
    justify-content: space-around;

    width: 90%;
    max-width: 85vh;

    p {
      margin: 0;
    }
  }
}
</style>