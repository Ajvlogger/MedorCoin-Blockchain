/**
 * MEDORCOIN INDUSTRIAL MINING MASTER - ULTIMATE EDITION
 * Resolves: Block Spillover, Adaptive Buffering, and Reward Anomaly Detection.
 * Integrated: Real-time HTTP & WebSocket Frontend Gateway for miners.html
 */

const cluster = require('cluster');
const os = require('os');
const http = require('http');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws');

const TransactionEngine = require('./transaction_engine.cjs');
const ProofOfWork = require('./proof_of_work.cjs');
const MempoolManager = require('./mempool.cjs');
const config = require('./consensus_config.json');
const logger = require('./log_transport.cjs');

if (cluster.isMaster) {
    const engine = new TransactionEngine({ nodeId: "medor-master-industrial" });
    const mempool = new MempoolManager(engine.db);
    const numWorkers = config.consensus.pow.threads || os.cpus().length;
    
    let isShuttingDown = false;
    let isDispatching = false;
    let webSocketServer = null;
    
    // Two-Tier Submission Queue (Active + Spillover)
    const MAX_PRIMARY_QUEUE = 50; 
    const submissionQueue = []; 
    const spilloverBuffer = []; 

    let currentDifficulty = BigInt(config.consensus.pow.initialDifficulty || 6);
    let lastBlockHash = "0".repeat(64); 
    let currentHeight = 0;

    const metrics = { 
        totalHashes: 0n, 
        blocksFound: 0, 
        lastBlockConfirmedTime: Date.now(),
        workerStats: new Map(),
        fallbackMinerCount: 0,
        consecutiveFallbacks: 0 
    };

    function startFrontendGateway() {
    const server = http.createServer((req, res) => {
        
        // 1. Handle CSRF Token Request (Placed at the very top)
        if (req.url === '/csrf-token' && req.method === 'GET') {
            res.writeHead(200, { 'Content-Type': 'application/json' });
            return res.end(JSON.stringify({ csrfToken: "your_generated_token_here" }));
        }

        // 2. Handle Registration Form Data (Placed at the very top)
        if (req.url === '/signup' && req.method === 'POST') {
            // Process your signup logic here
            res.writeHead(200, { 'Content-Type': 'application/json' });
            return res.end(JSON.stringify({ success: true, claimId: "abc", address: "xyz" }));
        }

        // Your original file-serving logic runs immediately after if the URLs don't match
        let targetFile = req.url === '/' ? 'miners.html' : req.url;
        let filePath = path.join(__dirname, targetFile);
        
        fs.readFile(filePath, (err, content) => {
            if (err) {
                res.writeHead(404, { 'Content-Type': 'text/plain' });
                res.end('File Not Found');
            } else {
                let contentType = 'text/html';
                if (filePath.endsWith('.js')) contentType = 'application/javascript';
                if (filePath.endsWith('.css')) contentType = 'text/css';
                
                res.writeHead(200, { 'Content-Type': contentType });
                res.end(content, 'utf-8');
            }
        });
    });

    // The rest of your WebSocket connection management code remains untouched below this line
        webSocketServer = new WebSocket.Server({ server });
        
        webSocketServer.on('connection', (ws) => {
            logger.shipToTransport("SYSTEM", "MASTER", "Frontend UI (miners.html) connected via WebSocket.");
            
            // Sync current network metrics immediately on slide load
            ws.send(JSON.stringify({
                type: 'STATE_SYNC',
                height: currentHeight,
                hash: lastBlockHash,
                difficulty: currentDifficulty.toString(),
                hps: Array.from(metrics.workerStats.values()).reduce((acc, w) => acc + Number(w.hashes), 0)
            }));
        });

        server.listen(8080, () => {
            logger.shipToTransport("SYSTEM", "MASTER", "Industrial Cluster web portal online at http://localhost:8080");
        });
    }

    // Push analytics loop to streaming clients every 1 second
    setInterval(() => {
        if (!webSocketServer || webSocketServer.clients.size === 0) return;
        
        const currentHps = Array.from(metrics.workerStats.values()).reduce((acc, w) => acc + Number(w.hashes), 0);
        
        const statsPayload = JSON.stringify({
            type: 'STATS_UPDATE',
            height: currentHeight,
            hash: lastBlockHash,
            difficulty: currentDifficulty.toString(),
            hps: currentHps
        });

        webSocketServer.clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(statsPayload);
            }
        });
    }, 1000);

    // --- MAIN INITIALIZATION LOOP ---
    async function init() {
        logger.shipToTransport("SYSTEM", "MASTER", `Booting Industrial Cluster...`);
        
        try {
            await engine.recoverFromCrash();
            currentHeight = Number(engine.currentHeight) || 0;
            lastBlockHash = String(engine.lastBlockHash || "0".repeat(64));
            metrics.lastBlockConfirmedTime = Date.now(); 
            
            // Fire up your live UI gateway
            startFrontendGateway();

        } catch (e) {
            logger.shipToTransport("CRITICAL", "DB", `Recovery Failure: ${e.message}`);
        }

        for (let i = 0; i < numWorkers; i++) spawnWorker();

        const dispatchLoop = async () => {
            if (isShuttingDown || isDispatching) return;
            isDispatching = true;
            
            try {
                const txs = await mempool.getPrioritized(config.consensus.validation.maxTransactionsPerBlock || 1000).catch(() => []);
                const targetMiner = await getTargetMiner();
                
                const workLoad = {
                    previousHash: String(lastBlockHash),
                    difficulty: String(currentDifficulty),
                    minerAddress: String(targetMiner),
                    height: currentHeight + 1,
                    mempool: txs
                };

                for (const id in cluster.workers) {
                    const w = cluster.workers[id];
                    if (w && w.isConnected()) {
                        try { w.send({ type: 'NEW_WORK', workLoad }); } catch (e) {}
                    }
                }
            } finally {
                isDispatching = false;
                if (!isShuttingDown) setTimeout(dispatchLoop, 1500);
            }
        };
        dispatchLoop();
        processQueue(); 
    }

    async function processQueue() {
        if (isShuttingDown) return;

        // Drain spillover into primary if space permits
        if (submissionQueue.length < MAX_PRIMARY_QUEUE && spilloverBuffer.length > 0) {
            submissionQueue.push(spilloverBuffer.shift());
        }

        if (submissionQueue.length > 0) {
            const { block, txs } = submissionQueue.shift();
            try {
                if (block.height > currentHeight) {
                    const ok = await engine.confirmBlock(txs, block.minerAddress, block.hash, block.height);
                    if (ok) {
                        metrics.blocksFound++;
                        const now = Date.now();
                        const blockInterval = (now - metrics.lastBlockConfirmedTime) / 1000;
                        adjustDifficulty(blockInterval);
                        
                        metrics.lastBlockConfirmedTime = now;
                        currentHeight = Number(engine.currentHeight);
                        lastBlockHash = String(engine.lastBlockHash);
                    }
                }
            } catch (e) {
                logger.shipToTransport("ERROR", "CONSENSUS", `Submission Failed: ${e.message}`);
            }
        }
        setImmediate(processQueue);
    }

    function spawnWorker() {
        const worker = cluster.fork();
        metrics.workerStats.set(worker.id, { hashes: 0n });

        worker.on('message', (msg) => {
            if (msg.type === 'BLOCK_FOUND') {
                if (submissionQueue.length >= MAX_PRIMARY_QUEUE) {
                    if (spilloverBuffer.length < 100) {
                        spilloverBuffer.push({ block: msg.block, txs: msg.mempool });
                        logger.shipToTransport("WARN", "QUEUE", "Primary queue full. Block moved to spillover buffer.");
                    }
