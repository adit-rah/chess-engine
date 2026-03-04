(function() {
  const PIECES = { K: '♔', Q: '♕', R: '♖', B: '♗', N: '♘', P: '♙', k: '♚', q: '♛', r: '♜', b: '♝', n: '♞', p: '♟' };
  var Module = null;
  var selectedSquare = null;
  var whitePlayerType = 'human';
  var blackPlayerType = 'human';

  var EMPTY_FEN = '8/8/8/8/8/8/8/8';

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
    var grid = fenToGrid(fen || EMPTY_FEN);
    if (!grid) grid = fenToGrid(EMPTY_FEN);
    if (!grid) return;
    const boardEl = document.getElementById('board');
    boardEl.innerHTML = '';
    var files = ['', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
    for (var r = 0; r < 9; r++) {
      for (var c = 0; c < 9; c++) {
        var cell = document.createElement('div');
        if (r === 0) {
          cell.className = 'board-label';
          cell.textContent = files[c];
        } else if (c === 0) {
          cell.className = 'board-label';
          cell.textContent = 9 - r;
        } else {
          var br = r - 1;
          var bc = c - 1;
          cell.className = 'square ' + ((br + bc) % 2 === 0 ? 'light' : 'dark');
          cell.dataset.row = br;
          cell.dataset.col = bc;
          var piece = grid[br] && grid[br][bc] !== undefined ? grid[br][bc] : '';
          cell.textContent = piece && PIECES[piece] ? PIECES[piece] : '';
          if (piece && piece === piece.toLowerCase()) cell.classList.add('piece-black');
          cell.dataset.square = String.fromCharCode(97 + bc) + (8 - br);
          cell.addEventListener('click', function(ev) { onSquareClick(ev.currentTarget); });
        }
        boardEl.appendChild(cell);
      }
    }
  }

  function sqToNotation(row, col) {
    return String.fromCharCode(97 + col) + (row + 1);
  }

  function onSquareClick(sqEl) {
    if (!Module || !Module.isRunning()) return;
    var turn = Module.getTurn();
    if (!isHumanTurn(turn)) return;
    var square = sqEl.dataset.square;
    if (selectedSquare) {
      const ok = Module.humanMove(selectedSquare, square);
      clearSelection();
      if (ok) {
        updateStatus();
        renderBoard(Module.getBoard());
        if (Module.isRunning() && !isHumanTurn(Module.getTurn())) {
          setTimeout(computerMove, 100);
        }
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
    var move = Module.computerMove();
    if (move && move.length >= 4) {
      updateStatus();
      renderBoard(Module.getBoard());
      if (Module.isRunning() && !isHumanTurn(Module.getTurn())) {
        setTimeout(computerMove, 150);
      }
    }
  }

  function updateStatus() {
    const el = document.getElementById('status');
    if (!Module) {
      el.textContent = 'Loading...';
      return;
    }
    if (!Module.isRunning()) {
      el.textContent = 'Game over. Click New Game to play again.';
      return;
    }
    var turn = Module.getTurn();
    var suffix = isHumanTurn(turn) ? '' : ' (thinking...)';
    el.textContent = (turn === 'white' ? 'White' : 'Black') + ' to move' + suffix;
  }

  var VALID_PLAYERS = ['human', 'computer1', 'computer2', 'computer3', 'computer4'];

  function startGame() {
    if (!Module) return;
    var white = document.getElementById('whitePlayer').value;
    var black = document.getElementById('blackPlayer').value;
    if (VALID_PLAYERS.indexOf(white) === -1 || VALID_PLAYERS.indexOf(black) === -1) return;
    whitePlayerType = white;
    blackPlayerType = black;
    Module.init(whitePlayerType, blackPlayerType);
    clearSelection();
    document.getElementById('setup').classList.add('hidden');
    document.getElementById('gameControls').classList.remove('hidden');
    updateStatus();
    renderBoard(Module.getBoard());
    if (Module.isRunning() && !isHumanTurn(Module.getTurn())) {
      setTimeout(computerMove, 100);
    }
  }

  function isHumanTurn(turn) {
    return (turn === 'white' && whitePlayerType === 'human') || (turn === 'black' && blackPlayerType === 'human');
  }

  function newGame() {
    if (!Module) return;
    Module.reset();
    clearSelection();
    document.getElementById('setup').classList.remove('hidden');
    document.getElementById('gameControls').classList.add('hidden');
    document.getElementById('status').textContent = 'Choose players and start.';
    renderBoard(EMPTY_FEN);
  }

  document.getElementById('startGame').addEventListener('click', startGame);
  document.getElementById('newGame').addEventListener('click', newGame);

  if (typeof createChessModule !== 'function') {
    document.getElementById('status').textContent = 'Chess engine not built. Run "cd web && make" or deploy via GitHub Pages.';
    throw new Error('createChessModule not found');
  }

  var scriptDir = (function() {
    var s = document.querySelector('script[src*="chess"]');
    if (s && s.src) return s.src.replace(/[^/]+$/, '');
    var base = location.origin + location.pathname;
    return base.endsWith('/') ? base : base + '/';
  })();

  console.log('[Chess] Loading module...');
  var loadPromise = createChessModule({
    locateFile: function(path) {
      if (path.endsWith('.wasm')) return scriptDir + path;
      return path;
    }
  });
  setTimeout(function() {
    if (!Module) {
      console.warn('[Chess] Still loading after 10s - may be slow or stuck');
    }
  }, 10000);
  loadPromise.then(function(mod) {
    console.log('[Chess] Module loaded');
    Module = mod;
    try {
      newGame();
    } catch (e) {
      console.error('Game init error:', e);
      document.getElementById('status').textContent = 'Failed to initialize game.';
    }
  }).catch(function(err) {
    var msg = err ? (err.message || err.toString()) : 'Unknown';
    console.error('[Chess] Load failed:', msg, err);
    document.getElementById('status').textContent = 'Failed to load game.';
  });
})();
