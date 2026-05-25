#!/usr/bin/env python3
"""Fetch filtered chess games from Lichess for a list of players."""

import argparse
import datetime
import io
import sys

import berserk
import chess.pgn


EXCLUDED_SPEEDS = {"bullet", "blitz", "ultrabullet"}


def parse_date(s):
    return datetime.datetime.strptime(s, "%Y-%m-%d")


def parse_args():
    parser = argparse.ArgumentParser(
        description="Fetch filtered Lichess games for a list of players."
    )
    parser.add_argument(
        "handles_file",
        help="File containing player handles, one per line",
    )
    parser.add_argument(
        "start_date",
        type=parse_date,
        help="Start date (YYYY-MM-DD)",
    )
    parser.add_argument(
        "end_date",
        type=parse_date,
        help="End date (YYYY-MM-DD)",
    )
    parser.add_argument(
        "--min-elo",
        type=int,
        default=2500,
        help="Minimum opponent ELO rating (default: 2500)",
    )
    return parser.parse_args()


def read_handles(path):
    with open(path) as f:
        return [line.strip() for line in f if line.strip()]


def opponent_elo(game, username):
    white = game.headers.get("White", "")
    black = game.headers.get("Black", "")
    if white.lower() == username.lower():
        return int(game.headers.get("BlackElo", "0"))
    elif black.lower() == username.lower():
        return int(game.headers.get("WhiteElo", "0"))
    return 0


def fetch_games(client, username, since_ms, until_ms):
    return client.games.export_by_player(
        username,
        as_pgn=True,
        since=since_ms,
        until=until_ms,
        moves=True,
        tags=True,
        clocks=True,
        evals=True,
        opening=True,
    )


def is_excluded_speed(event):
    event_lower = event.lower()
    return any(speed in event_lower for speed in EXCLUDED_SPEEDS)


def main():
    args = parse_args()
    handles = read_handles(args.handles_file)
    since_ms = int(args.start_date.timestamp() * 1000)
    until_ms = int(args.end_date.timestamp() * 1000)

    client = berserk.Client()
    seen_ids = set()

    for username in handles:
        print(f"Fetching games for {username} ...", file=sys.stderr)
        try:
            games_iter = fetch_games(client, username, since_ms, until_ms)
        except berserk.exceptions.ResponseError as e:
            print(f"Warning: error fetching games for {username}: {e}",
                  file=sys.stderr)
            continue
        try:
            games = list(games_iter)
        except berserk.exceptions.ResponseError as e:
            print(f"Warning: player not found or API error for {username}: {e}",
                  file=sys.stderr)
            continue
        for pgn_text in games:
            game = chess.pgn.read_game(io.StringIO(pgn_text))
            if game is None:
                continue

            event = game.headers.get("Event", "")
            if is_excluded_speed(event):
                continue

            site = game.headers.get("Site", "")
            game_id = site.rsplit("/", 1)[-1] if "/" in site else None
            if game_id and game_id in seen_ids:
                continue
            if game_id:
                seen_ids.add(game_id)

            variant = game.headers.get("Variant", "Standard")
            if variant not in ("Standard", "standard", ""):
                continue

            termination = game.headers.get("Termination", "")
            if termination != "Normal":
                continue

            opp_elo = opponent_elo(game, username)
            if opp_elo < args.min_elo:
                continue

            print(game, end="\n\n")

    print(f"Done. Processed {len(handles)} player(s).", file=sys.stderr)


if __name__ == "__main__":
    main()
