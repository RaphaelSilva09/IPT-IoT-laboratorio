const express = require("express");
const mqtt = require("mqtt");

const router = express.Router();

// Conexão com o broker MQTT
const client = mqtt.connect("mqtt://broker.hivemq.com");

// Tratamento de erros na conexão MQTT
client.on("connect", () => {
  console.log("Conectado ao HiveMQ!");
});

client.on("error", (err) => {
  console.error("Erro na conexão MQTT:", err);
});

// Rota para criar um novo dispositivo e enviar dados ao broker MQTT
router.post("/", async (req, res) => {
  try {
    const { metrica, limite_metrica, nome } = req.body;

    // Validação de campos obrigatórios
    if (!metrica || !limite_metrica || !nome) {
      return res.status(400).json({
        success: false,
        message: "Parâmetros inválidos. Verifique os dados enviados.",
      });
    }

    // Validação das métricas permitidas
    const metricasValidas = ["vibracao", "distancia"];
    if (!metricasValidas.includes(metrica)) {
      return res.status(400).json({
        success: false,
        message: 'Métrica inválida. Use "vibracao" ou "distancia".',
      });
    }

    // Verificação de limite numérico
    const limiteNumerico = Number(limite_metrica);
    if (isNaN(limiteNumerico) || limiteNumerico <= 0) {
      return res.status(400).json({
        success: false,
        message: "Limite deve ser um número positivo.",
      });
    }

    // Publicar mensagens nos tópicos MQTT correspondentes
    const topic = `larissamartins_inteli_1/${metrica}`;
    const payload = JSON.stringify({
      nome,
      limite: limiteNumerico,
    });

    client.publish(topic, payload, (err) => {
      if (err) {
        console.error("Erro ao publicar no broker MQTT:", err);
        return res.status(500).json({
          success: false,
          message: "Erro ao enviar dados ao broker MQTT.",
        });
      }

      console.log(`Publicado no tópico ${topic}:`, payload);
      res.status(201).json({
        success: true,
        message:
          "Dispositivo criado e dados enviados ao broker MQTT com sucesso.",
        device: {
          nome,
          metrica,
          limite: limiteNumerico,
        },
      });
    });
  } catch (error) {
    console.error("Erro interno do servidor:", error);
    res.status(500).json({
      success: false,
      message: "Erro interno do servidor.",
    });
  }
});

module.exports = router;
