const db = require('./database')

async function insertRun(dispositivo_id, tempo_uso, media_metrica) {
    await db.connect()

    const queryRuns = `INSERT INTO runs (tempo_uso, media_metrica) VALUES ($1, $2)`

    await db.query(queryRuns, [dispositivo_id, tempo_uso, media_metrica])

    console.log('insert de run concluido!')

}

module.exports = insertRun