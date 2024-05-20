const startSimulation = require("./binding/lib/binding.js");
const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
app.use(express.json());
const port = process.env.PORT || 3000;
app.use(express.static(path.join(__dirname, "client/dist")));

app.use(cors());

app.get("*", (req, res) => {
    return res.sendFile(path.join(__dirname, "client/dist", "index.html"));
});

app.post("/simulation", (req, res) => {
    console.log("here!");
    console.log(req.body);
    const { numNodes, numFaulty, data } = req.body;
    try {
        const log = startSimulation(numNodes, numFaulty, data);
        console.log("ran sim!");
        return res.status(200).json({ log });
    } catch (err) {
        return res.status(500).json({ err: err.message });
    }
});

app.listen(port, () => {
    console.log(`Server running at port ${port}`);
});
