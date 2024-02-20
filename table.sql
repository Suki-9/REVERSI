CREATE TABLE IF NOT EXISTS game_instance (
  create_at      BIGINT       NOT NULL,
  current_player INTEGER      default 1,
  player_ids     TEXT[2]      NOT NULL,
  board          INTEGER[64]  NOT NULL,
  pass           TEXT         NOT NULL,
  instance_id    TEXT         NOT NULL,
  is_matched     BOOLEAN      default false, 
  PRIMARY KEY (instance_id)
);