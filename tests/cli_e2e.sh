#!/usr/bin/env bash
set -euo pipefail

# Simple end-to-end CLI test harness
HERE="$PWD"
DOCKER_DIR="$HERE/docker"
if command -v docker >/dev/null 2>&1 && docker compose version >/dev/null 2>&1; then
  COMPOSE="docker compose -f $DOCKER_DIR/docker-compose.yml"
else
  COMPOSE="docker-compose -f $DOCKER_DIR/docker-compose.yml"
fi

echo "Starting docker test IMAP server..."
pushd "$DOCKER_DIR" >/dev/null
if [[ "$COMPOSE" == "docker compose -f $DOCKER_DIR/docker-compose.yml" || "$COMPOSE" == "docker-compose -f $DOCKER_DIR/docker-compose.yml" ]]; then
  ${COMPOSE} up -d --build
  DOCKER_MODE="compose"
  popd >/dev/null
else
  # No compose available; build the image and run container directly
  docker build -t imap-kanban-dovecot "$DOCKER_DIR"
  docker run -d --name imap-kanban-dovecot -p 143:143 -p 993:993 imap-kanban-dovecot
  DOCKER_MODE="run"
  popd >/dev/null
fi

# Wait for IMAP SSL port 993 to be ready
echo "Waiting for IMAP (993) to accept connections..."
for i in {1..30}; do
  if timeout 1 bash -c "</dev/tcp/localhost/993" 2>/dev/null; then
    echo "IMAP port open"
    break
  fi
  sleep 1
done

if ! timeout 1 bash -c "</dev/tcp/localhost/993" 2>/dev/null; then
  echo "IMAP port 993 not open after wait" >&2
  exit 2
fi

# Location of CLI binary
CLI_BIN="$HERE/build/imap-kanban-cli"
if [ ! -x "$CLI_BIN" ]; then
  echo "CLI binary not found at $CLI_BIN" >&2
  exit 2
fi

# Use tests/imap_test.conf as INI; convert to QSettings INI structure expected by Settings::loadFromFile
CONF_INI="$HERE/tests/imap_test.ini"
cat > "$CONF_INI" <<'EOF'
[imap]
server=localhost
port=993
ssl=true
username=testuser
password=testpass

[kanban]
visibleMailboxes=TODO,DOING,DONE,BACKLOG

[ui]
refreshInterval=30
EOF

# Run list-mailboxes
echo "Running list-mailboxes..."
"$CLI_BIN" --config "$CONF_INI" list-mailboxes | tee /tmp/imap_list.txt
if ! grep -q "TODO" /tmp/imap_list.txt; then
  echo "Expected mailbox TODO not found in output" >&2
  exit 3
fi

echo "Running show-cards (TODO)..."
"$CLI_BIN" --config "$CONF_INI" show-cards -m TODO | tee /tmp/imap_cards.txt
# If no cards are present it's okay; we just ensure command runs

echo "Testing verbose output (should show IMAP protocol details)..."
"$CLI_BIN" --verbose --config "$CONF_INI" list-mailboxes 2>&1 | head -10 | tee /tmp/imap_verbose.txt
if ! grep -q "IMAP RECV\|IMAP SEND" /tmp/imap_verbose.txt; then
  echo "Expected verbose IMAP logging not found in output" >&2
  exit 3
fi
echo "Verbose logging test passed"

# If there are cards, try move the first one
CARD_UID=$(grep -oP "^UID: \K.*" /tmp/imap_cards.txt | head -n1 || true)
if [ -n "$CARD_UID" ]; then
  echo "Found card UID=$CARD_UID, attempting move to DONE"
  "$CLI_BIN" --config "$CONF_INI" move-card -u "$CARD_UID" -f TODO -t DONE
  echo "Move command completed"
fi

# Tear down docker
echo "Tearing down docker containers..."
pushd "$DOCKER_DIR" >/dev/null
if [ "$DOCKER_MODE" = "compose" ]; then
  ${COMPOSE} down -v
  popd >/dev/null
else
  docker rm -f imap-kanban-dovecot || true
  popd >/dev/null
fi

echo "E2E test finished"
