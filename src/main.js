const startSimulation = require("../binding/lib/binding.js");
const express = require("express");

const app = express();
app.use(express.json());
const port = 3000;

app.get("/", (req, res) => {
    try {
        startSimulation(5, 0, "lol");
        return res.status(200).json({
            lol: "lol"
        });
    } catch (err) {
        return res.status(500).send(err.message);
    }
});

app.listen(port, () => {
    console.log(`Server running at port ${port}`);
});
