import ReactFlow, { Edge, MarkerType, Node } from "reactflow";
import { useState, useEffect, FormEvent } from "react";
import "reactflow/dist/style.css";
import './App.css';

type LogEntry = {
    messageType: string,
    round: number,
    senderId: string,
    receiverId: string
}

const generateNodes = (numNodes: number, faultyNodes: Set<number>): Node[] => {
    const radius = 500;
    const centerX = 0;
    const centerY = 0;
    const nodes: Node[] = [];
    for (let i = 0; i < numNodes; i++) {
        const angle = (i / numNodes) * 2 * Math.PI;
        const x = centerX + radius * Math.cos(angle) * -1;
        const y = centerY + radius * Math.sin(angle) * -1;
        const isFaulty = faultyNodes.has(i);
        nodes.push({
            id: `${i}`,
            type: "default",
            position: { x, y },
            data: { label: `Node ${i} ${i === 0 ? "(you)" : ""}` },
            style: { backgroundColor: isFaulty ? "red" : "green" }
        });
    }
    return nodes;
};

const selectRandomFaultyNodes = (numNodes: number, numFaulty: number) => {
    const allNodes = Array.from({ length: numNodes }, (_, i) => i);
    const faultyNodes = new Set<number>();
    while (faultyNodes.size < numFaulty) {
        const randomIndex = Math.floor(Math.random() * numNodes);
        faultyNodes.add(allNodes[randomIndex]);
    }
    return faultyNodes;
}

function createEdgeFromLog(log: LogEntry, index: number): Edge {
    return {
        id: `e${log.senderId}-${log.receiverId}-${index}`,
        source: `${log.senderId}`,
        target: `${log.receiverId}`,
        label: log.messageType,
        animated: true,
        style: { stroke: log.messageType === 'PRE_PREPARE' ? 'blue' : log.messageType === 'PREPARE' ? 'orange' : 'purple' },
        markerEnd: MarkerType.ArrowClosed
    }
}

function App() {
    const [numNodes, setNumNodes] = useState(5);
    const [numFaulty, setNumFaulty] = useState(1);
    const [data, setData] = useState("");
    const [nodes, setNodes] = useState<Node[]>(generateNodes(numNodes, new Set()));
    const [edges, setEdges] = useState<Edge[]>([]);
    const [logData, setLogData] = useState<LogEntry[]>([]);
    const [currentLogIndex, setCurrentLogIndex] = useState(0);

    useEffect(() => {
        const faultyNodes = selectRandomFaultyNodes(numNodes, numFaulty);
        setNodes(generateNodes(numNodes, faultyNodes));
    }, [numNodes, numFaulty]);

    useEffect(() => {
        const interval = setInterval(() => {
            setEdges((prevEdges) => {
                if (currentLogIndex < logData.length) {
                    const log = logData[currentLogIndex];
                    const newEdge = createEdgeFromLog(log, currentLogIndex);
                    setCurrentLogIndex((prevIndex) => prevIndex + 1);
                    return [...prevEdges, newEdge];
                } else {
                    clearInterval(interval);
                    return prevEdges;
                }
            });
        }, 500);

        return () => clearInterval(interval);
    }, [currentLogIndex, logData]);

    function stop(e: React.DragEvent<HTMLInputElement>) {
        e.preventDefault();
        e.nativeEvent.stopImmediatePropagation();
    }

    function handleNumNodesChange(e: React.ChangeEvent<HTMLInputElement>) {
        e.preventDefault();
        const numberOfNodes = parseInt(e.target.value, 10);
        setNumNodes(numberOfNodes);
        const newMaxFaulty = Math.floor((numberOfNodes - 1) / 3);
        setNumFaulty(Math.min(numFaulty, newMaxFaulty));
    }

    async function handleSubmit(e: FormEvent) {
        e.preventDefault();
        const response = await fetch(`/simulation`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                numNodes,
                numFaulty,
                data
            })
        });
        const payload = await response.json();
        setLogData(payload.log);
    }

    return (
        <div className="flex w-screen h-screen p-6">
            <form className="p-6" onSubmit={handleSubmit}>
                <label htmlFor="numnodes" className="block mb-4">
                    Number of nodes (n) (minimum 4, max 10): <strong>{numNodes}</strong>
                    <input 
                        draggable={true}
                        type="range" 
                        id="numnodes"
                        value={numNodes}
                        onChange={handleNumNodesChange}
                        onDragStart={stop}
                        min="4"
                        max="10"
                    />
                </label>
                <label htmlFor="numfaulty" className="block mb-4">
                    Number of faulty nodes (max = (n - 1) / 3): <strong>{numFaulty}</strong>
                    <input 
                        draggable={true}
                        type="range" 
                        id="numFaulty"
                        value={numFaulty}
                        onChange={(e) => setNumFaulty(parseInt(e.target.value, 10))}
                        onDragStart={stop}
                        max={Math.floor((numNodes - 1) / 3)}
                    />
                </label>
                <label htmlFor="data" className="block mb-4">
                    The data to add to the blockchain, it can be anything!
                    <input 
                        type="text" 
                        id="data" 
                        className="p-2 rounded-lg"
                        onChange={(e) => setData(e.target.value)}
                    />
                </label>
                <input type="submit" value="submit" className="px-2 py-1 border rounded-lg"/>
            </form>
            <div className="w-full h-full">
                <ReactFlow nodes={nodes} edges={edges} fitView className="bg-red-100"/>
            </div>
        </div>
    );
}

export default App
