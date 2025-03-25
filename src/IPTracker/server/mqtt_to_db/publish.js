const mqtt = require('mqtt');
const client = mqtt.connect('mqtt://broker.hivemq.com'); // Substitua se estiver usando outro broker

// Tópicos para testar
const TOPICS = [
  "inteli_beggiana_temperature",
  "inteli_beggiana_distance",
  "inteli_beggiana_vibration",
  "inteli_beggiana_interval",
  "inteli_beggiana_oil_change",
];

// Dados simulados para cada tópico
const TEST_DATA = {
  inteli_beggiana_temperature: { dispositivo_id: 1, payload: "25.3°C" },
  inteli_beggiana_distance: { dispositivo_id: 1, payload: "123.45cm" },
  inteli_beggiana_vibration: { dispositivo_id: 1, payload: "Low" },
  inteli_beggiana_interval: { dispositivo_id: 1, payload: "10s" },
  inteli_beggiana_oil_change: { dispositivo_id: 1, payload: "200h" },
};

client.on('connect', () => {
  console.log('Conectado ao broker MQTT');

  // Publicar mensagens para cada tópico
  TOPICS.forEach((topic, index) => {
    setTimeout(() => {
      const message = JSON.stringify(TEST_DATA[topic]);
      client.publish(topic, message, () => {
        console.log(`Mensagem publicada no tópico ${topic}:`, message);
        if (index === TOPICS.length - 1) client.end(); // Fecha a conexão ao final
      });
    }, index * 1000); // Intervalo de 1 segundo entre mensagens
  });
});
