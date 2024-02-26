<script setup lang="ts">
import ResultCard from '../component/ResultCard.vue';

import { Ref, ref, inject } from 'vue';
import cookie from '../scripts/cookie';
import router from '../router';

const board = ref<number[]>([...Array(64)].map(_ => (0)));
const errList = inject<Ref<Err[]>>('errList');
const i = ref<number>();

const result = ref<boolean>();

const roomId = cookie.read('roomId');
const playerId = cookie.read('playerId');
const placed = ref<number[]>([0, 0]);

const ws = new WebSocket(`wss://reversi.kawaii-music.xyz/api/stream?roomId=${roomId}&playerId=${playerId}`);

if (roomId && playerId) {
  ws.addEventListener('open', () => {
    ws.send(JSON.stringify({
      type: "connection",
    }))
  });

  ws.addEventListener('message', (e) => {
    const msg = JSON.parse(e.data);
    console.log(msg);
    switch (msg.type) {
      case 'start':
        i.value = msg.body.yourNum;
        updateBoard(msg.body);
        break;
      case 'reconnection':
        i.value = msg.body.yourNum;
        updateBoard(msg.body);
        break;
      case 'put':
        updateBoard(msg.body)
        break;
      case 'err':
        errList?.value.push({
          title: 'WS Err',
          msg: msg.body.err,
        });
        break;
      case 'gameSet':
        updateBoard(msg.body);
        result.value = msg.body.win;
        ws.close();
        break;
      default:
        break
    }
  });
} else {
  errList?.value.push({
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

const updateBoard = (body: any): void => {
  if (i.value && body.board) {
    board.value = body.board;
    placed.value = board.value.reduce((a: number[], c: number) => [a[0] + ((c == i.value) ? 1 : 0), a[1] + ((c && c != 3 && c != i.value) ? 1 : 0)], [0, 0]);
  }
}
</script>

<template>
  <div :class="$style.root">
    <p v-if="i">あなたは {{ i - 1 ? "白" : "黒" }} です。</p>
    <div :class="$style.board">
      <a v-for="(s, i) in board" :class="$style.cell" @click="putStone(i)">
        <span :class="[{ [$style.brack]: s == 1 }, { [$style.white]: s == 2 }, {[$style.placeable]: s == 3}]" v-show="s">{{ s }}</span>
      </a>
    </div>
    <div :class="$style.info">
      <p>あなた: {{ placed[0] }}</p>
      <p>あいて: {{ placed[1] }}</p>
    </div>
  </div>
  <ResultCard v-if="result !== undefined" :isWin="result" :count="placed" />
</template>

<style module lang="scss">
.root {
  display: flex;
  flex-direction: column;
  align-items: center;

  width: 100%;

  p {
    margin: 0;
  }

  .board {
    display: grid;
    grid-template-columns: repeat(8, 1fr);
    grid-gap: 1px;

    padding: 1px;

    margin: 1% 0;

    width: 90%;
    max-width: 85vh;

    .cell {
      display: flex;
      align-items: center;
      justify-content: center;

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

      span {
        display: block;

        height:85%;
        width: 85%;

        border-radius: 50%;
        content: "";
        user-select: none;

        &.brack {  background-color: rgb(0, 0, 0);}
        &.white  { background-color: rgb(255,255,255);}

        &.placeable { border: dotted 3px; }
      }
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