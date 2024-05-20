const startSimulation = require("./binding/lib/binding.js");
const express = require("express");

const app = express();
app.use(express.json());
const port = process.env.PORT || 3000;

app.post("/simulation", (req, res) => {
    const { numNodes, numFaulty, hash } = req.body;
    try {
        const log = startSimulation(numNodes, numFaulty, hash);
        return res.status(200).json({ log });
    } catch (err) {
        return res.status(500).send(err.message);
    }
});

app.listen(port, () => {
    console.log(`Server running at port ${port}`);
});
