const db = require('./database')

async function insertMetrica(runs_id, dispositivo_id, dispositivo_metrica) {
    await db.connect()

    const queryMetrica = `INSERT INTO metrica (runs_id, dispositivo_id, dispositivo_metrica) VALUES ($1, $2, $3)`

    await db.query(queryMetrica, [runs_id, dispositivo_id, dispositivo_metrica])

    console.log('insert de metrica concluido!')

}

module.exports = insertMetrica