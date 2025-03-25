const db = require('./database');
const dispositivo = require('../models/dispositivos')
const runs = require('../models/runs')
const metrica = require('../models/metrica')


async function createTables() {
    await db.connect()
    await db.query(dispositivo) // cria a tabela dispositivos

    await db.query(runs) // cria a tabela runs

    await db.query(metrica) // cria a tabela métrica

    console.log('tabelas criadas');
}

createTables()

module.exports = createTables