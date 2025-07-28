import { Hono } from "hono";
import { zValidator as zv } from "@hono/zod-validator";
import { z } from "zod";
import { serveStatic } from "@hono/node-server/serve-static";
import { serve } from "@hono/node-server";
import ibft from "./binding/lib/binding.js";

const app = new Hono();

app.use("*", serveStatic({ root: "./client/dist" }));

const simulationArgs = z.object({
    numNodes: z.number().int(),
    numFaulty: z.number().int(),
    data: z.string()
});

app.post("/simulation", zv("json", simulationArgs), async c => {
    const { numNodes, numFaulty, data } = c.req.valid("json");
    const log = ibft(numNodes, numFaulty, data);
    return c.json({ log }, 200);
});

serve({
    port: process.env.PORT ?? 8080,
    fetch: app.fetch
}, (info) => {
    console.log(`IBFT Simulation running on port: ${info.port}`)
});