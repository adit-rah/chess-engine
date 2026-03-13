(function() {
  'use strict';

  const PIECES = {
    K: '♔', Q: '♕', R: '♖', B: '♗', N: '♘', P: '♙',
    k: '♚', q: '♛', r: '♜', b: '♝', n: '♞', p: '♟'
  };
  const VALID_PLAYERS = ['human', 'computer1', 'computer2', 'computer3', 'computer4', 'computer5'];
  const EMPTY_FEN = '8/8/8/8/8/8/8/8';

  let Module = null;
  let selectedSquare = null;
  let whitePlayerType = 'human';
  let blackPlayerType = 'human';
  let selectedPiece = null;   // for setup: K, Q, R, B, N, P or k,q,r,b,n,p
  let removeMode = false;
  let autoplayEnabled = true;
  let pendingSetupStart = false;  // after setup done, waiting for Start

  function fenToGrid(fen) {
    if (!fen || typeof fen !== 'string' || fen === '') return null;
    const grid = [];
    const rows = fen.split('/').slice(0, 8);
    for (const row of rows) {
      const line = [];
      for (const c of row) {
        if (/\d/.test(c)) {
          const n = Math.min(parseInt(c, 10) || 0, 8);
          for (let i = 0; i < n; i++) line.push('');
        } else if (/^[KQRBNPkqrbnp]$/.test(c)) {
          line.push(c);
        }
      }
      grid.push(line.slice(0, 8));
    }
    return grid.length === 8 ? grid : null;
  }

  function renderBoard(fen) {
    const grid = fenToGrid(fen || EMPTY_FEN) || fenToGrid(EMPTY_FEN);
    if (!grid) return;
    const boardEl = document.getElementById('board');
    boardEl.innerHTML = '';
    const files = ['', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
    const isSetup = Module && Module.isSetupMode && Module.isSetupMode();
    for (let r = 0; r < 9; r++) {
      for (let c = 0; c < 9; c++) {
        const cell = document.createElement('div');
        if (r === 0) {
          cell.className = 'board-label';
          cell.textContent = files[c];
        } else if (c === 0) {
          cell.className = 'board-label';
          cell.textContent = 9 - r;
        } else {
          const br = r - 1, bc = c - 1;
          cell.className = 'square ' + ((br + bc) % 2 === 0 ? 'light' : 'dark');
          cell.dataset.row = br;
          cell.dataset.col = bc;
          const piece = grid[br] && grid[br][bc] !== undefined ? grid[br][bc] : '';
          cell.textContent = piece && PIECES[piece] ? PIECES[piece] : '';
          if (piece && piece === piece.toLowerCase()) cell.classList.add('piece-black');
          cell.dataset.square = String.fromCharCode(97 + bc) + (8 - br);
          cell.addEventListener('click', () => onSquareClick(cell));
          if (isSetup) cell.classList.add('setup-clickable');
        }
        boardEl.appendChild(cell);
      }
    }
  }

  function formatMove(s) {
    if (!s || s.length < 4) return s;
    return s.slice(0, 2) + '→' + s.slice(2, 4);
  }

  function renderTimeline() {
    const emptyEl = document.getElementById('timelineEmpty');
    const movesEl = document.getElementById('timelineMoves');
    if (!emptyEl || !movesEl) return;
    let history = [];
    try {
      if (Module && Module.getMoveHistory) {
        const raw = Module.getMoveHistory();
        history = typeof raw === 'string' ? JSON.parse(raw) : (raw || []);
      }
    } catch (e) { history = []; }
    movesEl.innerHTML = '';
    if (history.length === 0) {
      emptyEl.classList.remove('hidden');
      return;
    }
    emptyEl.classList.add('hidden');
    let moveNum = 1, rowMoves = [];
    for (let i = 0; i < history.length; i++) {
      const isWhite = i % 2 === 0;
      const chip = document.createElement('span');
      chip.className = 'timeline-chip ' + (isWhite ? 'white' : 'black');
      chip.textContent = formatMove(history[i]);
      rowMoves.push(chip);
      if (isWhite) {
        const row = document.createElement('div');
        row.className = 'timeline-row';
        const numSpan = document.createElement('span');
        numSpan.className = 'timeline-move-num';
        numSpan.textContent = moveNum + '.';
        row.appendChild(numSpan);
        const moveDiv = document.createElement('div');
        moveDiv.className = 'timeline-move';
        rowMoves.forEach(m => moveDiv.appendChild(m));
        row.appendChild(moveDiv);
        movesEl.appendChild(row);
        rowMoves = [];
        moveNum++;
      }
    }
    if (rowMoves.length > 0) {
      const row = document.createElement('div');
      row.className = 'timeline-row';
      const numSpan = document.createElement('span');
      numSpan.className = 'timeline-move-num';
      numSpan.textContent = moveNum + '...';
      row.appendChild(numSpan);
      const moveDiv = document.createElement('div');
      moveDiv.className = 'timeline-move';
      rowMoves.forEach(m => moveDiv.appendChild(m));
      row.appendChild(moveDiv);
      movesEl.appendChild(row);
    }
  }

  function onSquareClick(sqEl) {
    if (!Module) return;
    if (Module.isSetupMode && Module.isSetupMode()) {
      const square = sqEl.dataset.square;
      if (removeMode) {
        Module.processCommand('- ' + square);
      } else if (selectedPiece) {
        Module.processCommand('+ ' + selectedPiece + ' ' + square);
      }
      renderBoard(Module.getBoard());
      return;
    }
    if (!Module.isRunning()) return;
    const turn = Module.getTurn();
    if (!isHumanTurn(turn)) return;
    const square = sqEl.dataset.square;
    if (selectedSquare) {
      const ok = Module.humanMove(selectedSquare, square);
      clearSelection();
      if (ok) {
        refreshUI();
        maybeAutoMove();
      }
    } else {
      selectedSquare = square;
      sqEl.classList.add('selected');
    }
  }

  function clearSelection() {
    selectedSquare = null;
    document.querySelectorAll('.square.selected').forEach(s => s.classList.remove('selected'));
  }

  function computerMove() {
    if (!Module || !Module.isRunning()) return;
    const move = Module.computerMove();
    if (move && move.length >= 4) {
      refreshUI();
      maybeAutoMove();
    }
  }

  function maybeAutoMove() {
    if (!autoplayEnabled || !Module || !Module.isRunning()) return;
    if (!isHumanTurn(Module.getTurn())) {
      setTimeout(computerMove, 150);
    }
  }

  function updateStatus() {
    const el = document.getElementById('status');
    if (!Module) {
      el.textContent = 'Loading...';
      return;
    }
    if (Module.isSetupMode && Module.isSetupMode()) {
      el.textContent = 'Edit the board, then click Done.';
      return;
    }
    if (pendingSetupStart) {
      el.textContent = 'Select players and click Start Game.';
      return;
    }
    if (!Module.isRunning()) {
      el.textContent = 'Game over.';
      return;
    }
    const turn = Module.getTurn();
    const suffix = isHumanTurn(turn) ? '' : ' (thinking...)';
    el.textContent = (turn === 'white' ? 'White' : 'Black') + ' to move' + suffix;
  }

  function refreshUI() {
    updateStatus();
    renderBoard(Module.getBoard());
    renderTimeline();
    updateGameControlsVisibility();
  }

  function isHumanTurn(turn) {
    return (turn === 'white' && whitePlayerType === 'human') ||
           (turn === 'black' && blackPlayerType === 'human');
  }

  function isAiVsAi() {
    return whitePlayerType.startsWith('computer') && blackPlayerType.startsWith('computer');
  }

  function updateGameControlsVisibility() {
    const nextBtn = document.getElementById('nextMove');
    const autoplayCheck = document.getElementById('autoplay');
    if (nextBtn && autoplayCheck) {
      if (Module.isRunning() && isAiVsAi()) {
        nextBtn.classList.remove('hidden');
        nextBtn.disabled = autoplayEnabled;
      } else {
        nextBtn.classList.add('hidden');
      }
    }
  }

  function startGame() {
    if (!Module) return;
    const white = document.getElementById('whitePlayer').value;
    const black = document.getElementById('blackPlayer').value;
    if (VALID_PLAYERS.indexOf(white) === -1 || VALID_PLAYERS.indexOf(black) === -1) return;
    whitePlayerType = white;
    blackPlayerType = black;
    if (pendingSetupStart) {
      Module.processCommand('game ' + whitePlayerType + ' ' + blackPlayerType);
      pendingSetupStart = false;
    } else {
      Module.init(whitePlayerType, blackPlayerType);
    }
    clearSelection();
    document.getElementById('setup').classList.add('hidden');
    document.getElementById('setupMode').classList.add('hidden');
    document.getElementById('gameControls').classList.remove('hidden');
    autoplayEnabled = document.getElementById('autoplay')?.checked ?? false;
    refreshUI();
    maybeAutoMove();
  }

  function customBoard() {
    if (!Module) return;
    Module.reset();
    Module.processCommand('setup');
    document.getElementById('setup').classList.add('hidden');
    document.getElementById('setupMode').classList.remove('hidden');
    document.getElementById('gameControls').classList.add('hidden');
    selectedPiece = 'K';
    removeMode = false;
    document.querySelectorAll('.piece-btn').forEach(b => b.classList.remove('active'));
    document.querySelector('.piece-btn[data-piece="K"]')?.classList.add('active');
    document.getElementById('removeMode')?.classList.remove('active');
    refreshUI();
  }

  function setupDone() {
    if (!Module || !Module.isSetupMode()) return;
    Module.processCommand('done');
    if (!Module.isSetupMode()) {
      document.getElementById('setupMode').classList.add('hidden');
      document.getElementById('setup').classList.remove('hidden');
      pendingSetupStart = true;
      document.getElementById('status').textContent = 'Select players and click Start Game.';
      renderBoard(Module.getBoard());
    } else {
      document.getElementById('status').textContent = 'Invalid setup: need exactly 1 white king, 1 black king, no pawns on back rank, no king in check.';
    }
  }

  function newGame() {
    if (!Module) return;
    Module.reset();
    clearSelection();
    pendingSetupStart = false;
    document.getElementById('setup').classList.remove('hidden');
    document.getElementById('setupMode').classList.add('hidden');
    document.getElementById('gameControls').classList.add('hidden');
    document.getElementById('status').textContent = 'Choose players and start.';
    renderBoard(EMPTY_FEN);
    renderTimeline();
  }

  function doResign() {
    if (!Module || !Module.isRunning()) return;
    Module.resign();
    refreshUI();
  }

  function nextMove() {
    if (!Module || !Module.isRunning()) return;
    computerMove();
  }

  document.getElementById('startGame').addEventListener('click', startGame);
  document.getElementById('customBoard').addEventListener('click', customBoard);
  document.getElementById('setupDone').addEventListener('click', setupDone);
  document.getElementById('newGame').addEventListener('click', newGame);
  document.getElementById('resignBtn').addEventListener('click', doResign);
  document.getElementById('nextMove').addEventListener('click', nextMove);

  document.querySelectorAll('.piece-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      removeMode = false;
      document.getElementById('removeMode').classList.remove('active');
      selectedPiece = btn.dataset.piece;
      document.querySelectorAll('.piece-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
    });
  });
  document.getElementById('removeMode').addEventListener('click', () => {
    removeMode = true;
    selectedPiece = null;
    document.querySelectorAll('.piece-btn').forEach(b => b.classList.remove('active'));
    document.getElementById('removeMode').classList.add('active');
  });

  document.getElementById('setTurnWhite').addEventListener('click', () => {
    if (Module && Module.isSetupMode()) Module.processCommand('= white');
  });
  document.getElementById('setTurnBlack').addEventListener('click', () => {
    if (Module && Module.isSetupMode()) Module.processCommand('= black');
  });

  document.getElementById('autoplay').addEventListener('change', (e) => {
    autoplayEnabled = e.target.checked;
    updateGameControlsVisibility();
    if (autoplayEnabled && Module && Module.isRunning() && !isHumanTurn(Module.getTurn())) {
      maybeAutoMove();
    }
  });

  if (typeof createChessModule !== 'function') {
    document.getElementById('status').textContent = 'Chess engine not built. Run "cd web && make" or deploy via GitHub Pages.';
    throw new Error('createChessModule not found');
  }

  const scriptDir = (function() {
    const s = document.querySelector('script[src*="chess"]');
    if (s && s.src) return s.src.replace(/[^/]+$/, '');
    const base = location.origin + location.pathname;
    return base.endsWith('/') ? base : base + '/';
  })();

  console.log('[Chess] Loading module...');
  createChessModule({ locateFile: path => path.endsWith('.wasm') ? scriptDir + path : path })
    .then(function(mod) {
      console.log('[Chess] Module loaded');
      Module = mod;
      try {
        newGame();
      } catch (e) {
        console.error('Game init error:', e);
        document.getElementById('status').textContent = 'Failed to initialize game.';
      }
    })
    .catch(function(err) {
      const msg = err ? (err.message || err.toString()) : 'Unknown';
      console.error('[Chess] Load failed:', msg, err);
      document.getElementById('status').textContent = 'Failed to load game.';
    });
})();
