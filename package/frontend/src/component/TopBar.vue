<template>
  <div :class="$style.root">
    <RouterLink to="/">
      <h2>Reversi App</h2>
    </RouterLink>
    <div :class="$style.r_container">
      <RouterLink to="/setting"><span class="material-symbols-outlined">settings</span></RouterLink>
      <a><span class="material-symbols-outlined">help</span></a>
      <a href="https://github.com/Suki-9/REVERSI"><span class="material-symbols-outlined">code</span></a>
    </div>
  </div>
</template>

<style module lang="scss">
.root {
  display: flex;
  justify-content: space-between;

  width: 100%;

  border-bottom: solid 1px rgb(0, 0, 0);
  .r_container {
    display: flex;
    justify-content: space-around;
    align-items: center;

    width: 10%;

    user-select: none;

    a {
      line-height: 0;
      border-radius: 9999px;

      &:hover {
        scale: 1.3;
      }
      &:active {
        scale: 1.2;
      }
    }
  }
}
</style>