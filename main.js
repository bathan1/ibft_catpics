const startSimulation = require("./binding/lib/binding.js");
const express = require("express");
const cors = require("cors");
const path = require("path");
const axios = require("axios");

const app = express();
app.use(express.json());
const port = process.env.PORT || 3000;
app.use(express.static(path.join(__dirname, "client/dist")));

app.use(cors());

app.get("/cat", async (req, res) => {
    const response = await axios.get("https://api.thecatapi.com/v1/images/search", {
        headers: {
            "x-api-key": process.env.CAT_API_KEY
        }
    });
    const payload = response.data;
    if (!payload) {
        return res.status(500).json({
            error: "Internal Server Error",
            message: "The cat picture could not be queried right now :("
        });
    }
    return res.status(200).json({ image: payload[0] });
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
