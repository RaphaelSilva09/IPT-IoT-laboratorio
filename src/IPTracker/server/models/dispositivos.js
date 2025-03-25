// define a query de criação da tabela de dispositivos
table = `CREATE TABLE IF NOT EXISTS dispositivo (
        id SERIAL PRIMARY key,
        reparo VARCHAR(100) NULL,
        metrica VARCHAR(100) NOT NULL,
        limite_metrica FLOAT NOT NULL,
        nome VARCHAR(100) UNIQUE,
        ultimaRevisao DATE NULL,
        ultimaTrocaOleo DATE NULL,
        ultimaManutencao DATE NULL,
        status VARCHAR(100) null,
        UNIQUE (id, metrica)
    );`

module.exports = table;