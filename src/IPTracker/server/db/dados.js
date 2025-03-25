const db = require('./database')

async function listDados(dispositivo_id) {
    await db.connect();
    
    // Corrigindo a query para passar o parâmetro corretamente
    const result = await db.query('SELECT media_metrica FROM runs WHERE dispositivo_id = $1', [dispositivo_id]);

    // Acessando os resultados da consulta
    let dados = result.rows;

    console.log('Lista de dados disponível:', dados);
    return dados;
}

listDados()

module.exports = listDados