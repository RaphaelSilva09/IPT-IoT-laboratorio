// define a query de criação da tabela de METRICA

table = `CREATE table if not EXISTS metrica (
        id SERIAL PRIMARY KEY,
        runs_id INT,
        FOREIGN KEY (runs_id) REFERENCES runs(id),
        dispositivo_id INT,
        dispositivo_metrica VARCHAR(100),
        FOREIGN KEY (dispositivo_id, dispositivo_metrica) REFERENCES dispositivo(id, metrica),
        time_stamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    );`

module.exports = table