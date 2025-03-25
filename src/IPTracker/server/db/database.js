const Pool = require('pg-pool')

// Configurações de conexão
const pool = new Pool({
    dialect: 'postgresql',
    user: 'admin',
    host: 'dpg-ct6phut2ng1s7396nacg-a.oregon-postgres.render.com',
    database: 'iptracker_vqu2',
    password: 'tVygVESf9l927myx4vEZZaCOfWcltklt',
    port: 5432,
    ssl: { rejectUnauthorized: false },
});

// Testar a conexão
(async () => {
    try {
        const client = await pool.connect(); // Conecta ao banco
        console.log('Conexão bem-sucedida!');
        const result = await client.query('SELECT NOW()'); // Exemplo de consulta
        console.log('Data e hora do servidor:', result.rows[0]);
        client.release(); // Libera o cliente após a consulta
    } catch (err) {
        console.error('Erro ao conectar:', err);
    } 
})();

module.exports = pool

