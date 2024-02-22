export default {
  read(k: string): string | undefined {
    return document.cookie
      .split("; ")
      .map(e => e.split("="))
      .reduce((a: { [k: string]: string }, kv: string[]) => {
        return { ...a, [kv[0]]: kv[1] };
      }, {})[k];
  },
  write(k: string, v: string | undefined): void {
    v && (document.cookie = `${k}=${v}`);
  },
  delete(k: string): void {
    document.cookie = `${k}=;max-age=0`;
  },
};