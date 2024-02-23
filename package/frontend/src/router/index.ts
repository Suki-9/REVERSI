import { createRouter, createWebHistory } from "vue-router";

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: "/",
      name: "Home",
      component: () => import("../pages/Home.vue"),
    },
    {
      path: "/play",
      name: "Play",
      component: () => import("../pages/Play.vue"),
    },
    {
      path: "/setting",
      name: "Setting",
      component: () => import("../pages/Setting.vue"),
    }
  ]
});

export default router;