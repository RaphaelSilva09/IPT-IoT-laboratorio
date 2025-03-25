const mqtt = require("mqtt"); // Biblioteca MQTT
const pool = require("../db/database"); // Conexão com o banco de dados PostgreSQL

// Conexão com o broker MQTT
const client = mqtt.connect("mqtt://broker.hivemq.com");

// Tópicos a serem subscritos
const TOPICS = [
  "inteli_beggiana_temperature",
  "inteli_beggiana_distance",
  "inteli_beggiana_vibration",
  "inteli_beggiana_interval",
  "inteli_beggiana_oil_change", // Define o tempo_uso
];

// Evento de conexão ao broker MQTT
client.on("connect", () => {
  console.log("Conectado ao HiveMQ!");
  TOPICS.forEach((topic) => {
    client.subscribe(topic, (err) => {
      if (!err) console.log(`Inscrito no tópico: ${topic}`);
      else console.error(`Erro ao subscrever ao tópico ${topic}:`, err);
    });
  });
});

// Callback para mensagens recebidas
client.on("message", async (topic, message) => {
  console.log(`Mensagem recebida no tópico ${topic}: ${message.toString()}`);

  try {
    // Tratamento da mensagem recebida
    let data;
    try {
      data = JSON.parse(message.toString());
    } catch (parseError) {
      console.warn(" Mensagem não está no formato JSON. Usando valor direto.");
      data = message.toString();
    }

    // Extração das informações
    const dispositivoNome = "desconhecido";
    const valorRecebido = parseFloat(data?.valor || data) || 0;
    const dispositivoMetrica = topic.split("_").pop();

    console.log(
      "Valores extraídos:",
      "\n - Dispositivo:",
      dispositivoNome,
      "\n - Valor Recebido:",
      valorRecebido,
      "\n - Métrica:",
      dispositivoMetrica
    );

    if (valorRecebido === 0) {
      console.warn("Aviso: O valor recebido é 0. Verifique os dados enviados no MQTT.");
    }

    // Insere ou atualiza o dispositivo
    let query = `
      INSERT INTO dispositivo (nome, metrica, limite_metrica, status)
      VALUES ($1, $2, $3, $4)
      ON CONFLICT (nome)
      DO UPDATE SET nome = EXCLUDED.nome
      RETURNING id;
    `;
    let result = await pool.query(query, [dispositivoNome, "sensor", 100.0, "ativo"]);
    const dispositivoId = result.rows[0]?.id;

    if (!dispositivoId) {
      console.error("Erro: ID do dispositivo não encontrado.");
      return;
    }

    // Insere uma nova linha na tabela "runs"
    query = `
      INSERT INTO runs (dispositivo_id, tempo_uso, media_metrica)
      VALUES ($1, ${topic === "inteli_beggiana_oil_change" ? "$2" : "NULL"}, ${topic === "inteli_beggiana_oil_change" ? "NULL" : "$2"})
      RETURNING id;
    `;
    const runsResult = await pool.query(query, [dispositivoId, valorRecebido]);
    const runsId = runsResult.rows[0]?.id;

    if (!runsId) {
      console.error("Erro: ID do registro de runs não encontrado.");
      return;
    }

    // Insere uma nova linha na tabela "metrica"
    query = `
      INSERT INTO metrica (runs_id, dispositivo_id, dispositivo_metrica, time_stamp)
      VALUES ($1, $2, $3, CURRENT_TIMESTAMP);
    `;
    await pool.query(query, [runsId, dispositivoId, dispositivoMetrica]);

    console.log("Dados inseridos com sucesso nas tabelas runs e metrica!");
  } catch (err) {
    console.error("Erro ao processar ou armazenar a mensagem:", err);
  }
});

// Função para buscar dispositivo_id pelo nome
async function getDeviceId(nome) {
  const query = `SELECT id FROM dispositivo WHERE nome = $1;`;
  const result = await pool.query(query, [nome]);
  return result.rows[0]?.id;
}

// Tratamento de erros de conexão
client.on("error", (err) => {
  console.error("Erro na conexão com o broker MQTT:", err);
});

// Encerrar conexões no fim do processo
process.on("exit", () => {
  client.end();
  pool.end();
});
