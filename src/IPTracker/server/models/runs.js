// define a query de criação da tabela de runs
table = `CREATE TABLE IF NOT EXISTS runs (
       id SERIAL PRIMARY KEY,
       dispositivo_id INT,
       foreign key (dispositivo_id) references dispositivo(id), 
       tempo_uso INT NULL,
       media_metrica DECIMAL
    );`

    module.exports = table;