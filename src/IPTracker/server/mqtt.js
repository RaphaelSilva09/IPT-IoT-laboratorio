import mqtt from "mqtt";
import { app } from "./server";

// tópicod
const topico1 = "larissamartins_inteli_1/isAcceleration";
const topico2 = "larissamartins_inteli_1/isDistance";
const topico3 = "larissamartins_inteli_1/interval";
const topico4 = "larissamartins_inteli_1/maxTemperature";
const topico5 = "larissamartins_inteli_1/maxVibration";

const client = mqtt.connect("http://broker.hivemq.com/");
const topics = [topico1, topico2, topico3, topico4, topico5];

app.post("/mqtt", (req, res) => {
  client.on("connect", () => {
    console.log("Connected to MQTT broker");

    topics.forEach((topic) => {
      client.subscribe(topic);
      console.log(`Subscribed to topic: ${topic}`);
    });
  });
});
