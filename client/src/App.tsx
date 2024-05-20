import ReactFlow, { Node } from "reactflow";
import { useState, useEffect, FormEvent } from "react";
import "reactflow/dist/style.css";
import './App.css';

const generateNodes = (numNodes: number, faultyNodes: Set<number>): Node[] => {
    const radius = 500;
    const centerX = 0;
    const centerY = 0;
    return Array.from({ length: numNodes }, (_, i) => {
        const angle = (i / numNodes) * 2 * Math.PI;
        const x = centerX + radius * Math.cos(angle);
        const y = centerY + radius * Math.sin(angle);
        const isFaulty = faultyNodes.has(i);
        return {
            id: `${i}`,
            type: "default",
            position: { x, y },
            data: { label: `Node ${i}` },
            style: { backgroundColor: isFaulty ? "red" : "green" }
        };
    });
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

function App() {
    const [numNodes, setNumNodes] = useState(5);
    const [numFaulty, setNumFaulty] = useState(1);
    const [data, setData] = useState("");
    const [nodes, setNodes] = useState<Node[]>(generateNodes(numNodes, new Set()));

    useEffect(() => {
        const faultyNodes = selectRandomFaultyNodes(numNodes, numFaulty);
        setNodes(generateNodes(numNodes, faultyNodes));
    }, [numNodes, numFaulty]);

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
        const response = await fetch("/simulation", {
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
    }

    return (
        <div className="p-6 w-screen h-screen flex">
            <form className="p-6">
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
                    The cat picture you want to own!
                    <input type="text" id="data"/>
                </label>
                <input type="submit" value="submit" className="border px-2 py-1 rounded-lg"/>
            </form>
            <div className="w-full h-full">
                <ReactFlow nodes={nodes} fitView className="bg-red-100"/>
            </div>
        </div>
    );
}

export default App
